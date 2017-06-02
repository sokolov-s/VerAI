#include "torrent.h"
#include "common/filesystem.h"
#include "common/uuid.h"
#include <plog/Log.h>
#include <chrono>
#include <thread>
#include <future>
#include <fstream>
#include <functional>

#include <libtorrent/entry.hpp>
#include <libtorrent/bencode.hpp>
#include <libtorrent/session.hpp>
#include <libtorrent/torrent_info.hpp>
#include <libtorrent/torrent_handle.hpp>
#include <libtorrent/settings_pack.hpp>
#include <libtorrent/add_torrent_params.hpp>
#include <libtorrent/magnet_uri.hpp>
#include <libtorrent/error_code.hpp>
#include "libtorrent/file.hpp"
#include "libtorrent/file_pool.hpp"
#include "libtorrent/storage.hpp"
#include "libtorrent/hasher.hpp"
#include "libtorrent/create_torrent.hpp"
#include "libtorrent/hex.hpp"

#include <boost/make_shared.hpp>

using namespace torrent;
namespace lt = libtorrent;
using clk = std::chrono::steady_clock;

Torrent::Torrent()
    : cfg(config::ConfigTorrent::GetInstance())
{
}

Torrent::~Torrent()
{
    Stop();
}

void Torrent::Start()
{
    std::lock_guard<std::recursive_mutex> locker(workMtx);
    if(IsWork()) {
        return;
    }
    lt::settings_pack settings;
    settings.set_str(lt::settings_pack::listen_interfaces, cfg.GetInterface() + ":" + cfg.GetPort());
    settings.set_int(lt::settings_pack::alert_mask
                     , lt::alert::error_notification
                     | lt::alert::storage_notification
                     | lt::alert::status_notification
                     | lt::alert::progress_notification);

    session.reset(new lt::session(settings));
    isWork = true;
    handlersThread = std::thread(&Torrent::Handler, this);
    //TODO: we have a plan to get tasks only from server. So we are going to remove code with autoadding tfiles at startup.
//    addThread = std::thread(&Torrent::FindTFilesAndAdd, this);
    PLOG_INFO << "Torrent started";
}

void Torrent::Stop()
{
    std::lock_guard<std::recursive_mutex> locker(workMtx);
    if(!IsWork())
        return;
    isWork = false;
    if(handlersThread.joinable()) {
        handlersThread.join();
    }
    if(addThread.joinable()) {
        addThread.join();
    }
    session.release();
    std::lock_guard<std::recursive_mutex> lockerList(torrentIdListMtx);
    torrentsIdList.clear();
    PLOG_INFO << "Torrent stoped";
}

void Torrent::CreateTorrentAsync(const std::string &uuid, const std::string &path)
{
    if(infoManager.Get(uuid)->GetStatus() != TorrentInfo::Status::GENERATING &&
            infoManager.Get(uuid)->GetStatus() != TorrentInfo::Status::GENERATED) {
        std::thread(&Torrent::CreateTorrent, this, uuid, path).detach();
    }
}

void Torrent::CreateTorrent(const std::string &uuid, const std::string &path)
{
    TorrentInfo info(uuid);
    info.SetStatus(TorrentInfo::Status::GENERATING);
    info.SetPath(path);
    infoManager.UpdateOrAdd(info);

    lt::file_storage fs;
    lt::add_files(fs, path);
    if (fs.num_files() == 0) {
        UpdateStatus(uuid, TorrentInfo::Status::GENERATION_ERROR, 0);
        std::string errMsg = "No files specified for " + path;
        PLOG_WARNING << errMsg;
        return;
    }

    lt::create_torrent t(fs);
    lt::error_code ec;
    lt::set_piece_hashes(t, common::filesystem::GetDirectory(path),
                         std::bind(&Torrent::UpdateCreationProgress, this, uuid,
                                   std::placeholders::_1, t.num_pieces()), ec);
    if(ec) {
        PLOG_ERROR << ec.message();
        return;
    }
    //TODO: add full information.
    // set_creator - set the created-by field
    // set_comment - sets the comment
    // set_piece_hashes -

    auto e = t.generate();
    if (e.type() == lt::entry::undefined_t) {
        UpdateStatus(uuid, TorrentInfo::Status::GENERATION_ERROR, 0);
        std::string errMsg = "Can't create torent file for " + path;
        PLOG_ERROR << errMsg;
        return;
    }
    std::string fileName = cfg.GetTorrentFilesDirectory() + "/" + common::filesystem::GetFileName(path) + ".torrent";
    common::filesystem::CreateFolder(cfg.GetTorrentFilesDirectory());
    if(common::filesystem::IsFileExist(fileName)) {
        common::filesystem::Remove(fileName);
    }
    //TODO: write correct code to generate torrent file
//    std::vector<char> torrentData;
//    lt::bencode(std::back_inserter(torrentData), e);
//    std::fstream tFile;
//    tFile.exceptions(std::ifstream::failbit);
//    tFile.open(fileName, std::ios_base::out | std::ios_base::binary);
//    tFile.write(&torrentData[0], torrentData.size());
//    tFile.close();
    std::ofstream outFile(fileName, std::ios_base::binary);
    lt::bencode(std::ostream_iterator<char>(outFile), e);
    outFile.close();
    try {
//        AddTorrent(uuid, fileName);
        info.SetProgress(100);
        info.SetStatus(TorrentInfo::Status::GENERATED);
        auto tInfo = CreateInfoFromFile(fileName);
        std::string magnet = lt::make_magnet_uri(tInfo);
        info.SetLink(magnet);
        infoManager.UpdateOrAdd(info);
    } catch(const std::runtime_error &) {
        UpdateStatus(uuid, TorrentInfo::Status::GENERATION_ERROR, 0);
    }
}

void Torrent::UpdateCreationProgress(const std::string &uuid, int curPiece, int totalPieces)
{
    uint progress = trunc(1.0 * curPiece * 100 / totalPieces);
    UpdateStatus(uuid, TorrentInfo::Status::GENERATING, progress);
}

void Torrent::UpdateStatus(const std::string &uuid, const TorrentInfo::Status &status, const uint progress)
{
    auto info = infoManager.Get(uuid);
    info->SetStatus(status);
    info->SetProgress(progress);
    infoManager.UpdateOrAdd(*info);
}

std::string Torrent::GetIdByName(const std::string &name) const
{
    std::lock_guard<std::recursive_mutex> locker(torrentIdListMtx);
    for(const auto &it : torrentsIdList) {
        if(it.second.handler->torrent_name() == name) {
            return it.first;
        }
    }
    return "";
}

TorrentInfo Torrent::GetTorrentInfo(const std::string &uuid) const
{
    return *infoManager.Get(uuid);
}

std::string Torrent::GetMagnetFromHandler(const std::string &uuid) const
{
    std::lock_guard<std::recursive_mutex> locker(torrentIdListMtx);
    std::string magnet;
    if(torrentsIdList.find(uuid) != torrentsIdList.end()) {
        magnet = lt::make_magnet_uri(*torrentsIdList.at(uuid).param.ti);
        PLOG_DEBUG << "Magnet link for " << uuid << " : " << magnet;
    }
    PLOG_DEBUG_IF(magnet.empty()) << "Can't find uuid " << uuid;
    return magnet;
}

void Torrent::DownloadAsync(const std::string &uuid, const std::string &link) noexcept(false)
{
    TorrentInfo info(uuid);
    info.SetStatus(TorrentInfo::Status::DOWNLOADING);
    infoManager.UpdateOrAdd(info);

    lt::add_torrent_params param;

    lt::error_code ec;
    lt::parse_magnet_uri(link, param, ec);

    param.save_path = cfg.GetDownloadDirectory();
    std::ifstream ifs(GetResumeFilePath(param), std::ios_base::binary);
    ifs.unsetf(std::ios_base::skipws);
    param.resume_data.assign(std::istream_iterator<char>(ifs)
                           , std::istream_iterator<char>());
    if (ec) {
        std::string errMsg = "Invalid magnet URI: " + ec.message();
        PLOG_ERROR << errMsg;
        UpdateStatus(uuid, TorrentInfo::Status::DOWNLOADING_ERROR, 0);
        throw std::runtime_error(errMsg);
    }
    AddTorrent(uuid, std::move(param));
}

void Torrent::Handler()
{
    clk::time_point lastSaveResume = clk::now();
    while(IsWork()) {
        std::vector<lt::alert*> alerts;
        std::lock_guard<std::recursive_mutex> locker(torrentIdListMtx);
        session->pop_alerts(&alerts);

        for (lt::alert const* a : alerts) {
            if (auto at = lt::alert_cast<lt::add_torrent_alert>(a)) {
                auto id = GetIdByName(at->torrent_name());
                if(!id.empty()) {
                    torrentsIdList[id].handler = at;
                }
            }

            if (auto t = lt::alert_cast<lt::torrent_finished_alert>(a)) {
                t->handle.save_resume_data();
                auto id = GetIdByName(t->torrent_name());
                if(!id.empty()) {
                    UpdateStatus(id, TorrentInfo::Status::DOWNLOADED, 100);
                    torrentsIdList[id].handler = nullptr;
                }
                PLOG_DEBUG << "Downloading of " << t->torrent_name() << " has been finished : " << a->message();
            }

            if (auto t = lt::alert_cast<lt::torrent_error_alert>(a)) {
                PLOG_ERROR << "Some error with torrent " << t->torrent_name() << " : " << a->message();
            }

            // when resume data is ready, save it
            if (auto rd = lt::alert_cast<lt::save_resume_data_alert>(a)) {
                std::ofstream of(GetResumeFilePath(rd->torrent_name()), std::ios_base::binary);
                of.unsetf(std::ios_base::skipws);
                lt::bencode(std::ostream_iterator<char>(of)
                            , *rd->resume_data);
            }
            //TODO: write correct code to update status
            if (auto st = lt::alert_cast<lt::state_update_alert>(a)) {
                if (st->status.empty()) {
                    continue;
                }
                for(const auto &status : st->status) {
                    auto id = GetIdByName(status.name);
                    if(!id.empty()) {
                        auto info = infoManager.Get(id);
                        info->SetProgress(status.progress_ppm / 10000);
                        infoManager.UpdateOrAdd(*info);
                    }
                }
//                lt::torrent_status const& s = st->status[0];
//                cout << "\r" << State(s.state) << " "
//                          << (status.download_payload_rate / 1000) << " kB/s "
//                          << (status.total_done / 1000) << " kB ("
//                          << (status.progress_ppm / 10000) << "%) downloaded\x1b[K";
//                cout.flush();
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(200));

        // ask the session to post a state_update_alert, to update our
        // state output for the torrent
        session->post_torrent_updates();

        // save resume data once every 30 seconds
        if (clk::now() - lastSaveResume > std::chrono::seconds(30)) {
            for(const auto &it : torrentsIdList) {
                if(it.second.handler) {
                    it.second.handler->handle.save_resume_data();
                }
            }
            lastSaveResume = clk::now();
        }
    }
}

std::string Torrent::State(lt::torrent_status::state_t s)
{
    switch(s) {
    case lt::torrent_status::checking_files:
        return "checking";
    case lt::torrent_status::downloading_metadata:
        return "dl metadata";
    case lt::torrent_status::downloading:
        return "downloading";
    case lt::torrent_status::finished:
        return "finished";
    case lt::torrent_status::seeding:
        return "seeding";
    case lt::torrent_status::allocating:
        return "allocating";
    case lt::torrent_status::checking_resume_data:
        return "checking resume";
    default:
        return "<>";
    }
}

std::string Torrent::GetResumeFilePath(const lt::add_torrent_params &param) const
{
    return GetResumeFilePath(param.name);
}

std::string Torrent::GetResumeFilePath(const std::string &name) const
{
    return cfg.GetDownloadDirectory() + "/" + name + ".resume_file";
}

//TODO: function is deprecated
void Torrent::FindTFilesAndAdd()
{
//    auto dir = cfg.GetTorrentFilesDirectory();
//    auto files = common::filesystem::GetFilesInFolder(dir, ".*\\.torrent");
//    for(const auto &f : files) {
//        if(!IsWork())
//            return;
//        try {
//            AddTorrent(dir + "/" + f);
//        } catch(const std::runtime_error &) {
//            continue;
//        }
//    }
}

void Torrent::AddTorrent(const std::string &uuid, const std::string &fullPath) noexcept(false)
{
    lt::add_torrent_params param;
    param.save_path = cfg.GetDownloadDirectory();
    param.ti = boost::make_shared<lt::torrent_info>(CreateInfoFromFile(fullPath));
    AddTorrent(uuid, std::move(param));
}

void Torrent::AddTorrent(const std::string &uuid, lt::add_torrent_params && param)
{
    std::lock_guard<std::recursive_mutex> locker(torrentIdListMtx);
    session->async_add_torrent(param);
    std::string curUUID(uuid);
    if(curUUID.empty())
        curUUID = common::GenerateUUID();
    torrentsIdList[curUUID].param = std::move(param);
}

lt::torrent_info Torrent::CreateInfoFromFile(const std::string &path) const
{
    lt::error_code ec;
    lt::torrent_info info(path, boost::ref(ec), 0);
    if (ec) {
        std::string errMsg = "Can't init torrent file : " + ec.message();
        PLOG_ERROR << errMsg;
        throw std::runtime_error(errMsg);
    }
    return info;
}

bool Torrent::IsWork() const
{
    std::lock_guard<std::recursive_mutex> locker(workMtx);
    return isWork;
}
