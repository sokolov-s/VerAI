#include "torrent.h"
#include "common/filesystem.h"
#include "common/uuid.h"
#include <plog/Log.h>
#include <chrono>
#include <thread>
#include <future>
#include <fstream>

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
    handlersThread = std::thread(std::bind(&Torrent::Handler, this));
    addThread = std::thread(std::bind(&Torrent::FindTFilesAndAdd, this));
    isWork = true;
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
    params.clear();
    torrents.clear();
    PLOG_INFO << "Torrent stoped";
}

std::string Torrent::PrepareMagnetAsync(const std::string &path)
{
    auto uuid = common::GenerateUUID();
    std::thread(std::bind(&Torrent::PrepareMagnet, this, path, uuid)).detach();
    return uuid;
}

void Torrent::PrepareMagnet(const std::string &path, const std::string &uuid)
{
    lt::file_storage fs;
    lt::add_files(fs, path);
    if (fs.num_files() == 0)
    {
        std::string errMsg = "No files specified for " + path;
        PLOG_WARNING << errMsg;
        throw std::runtime_error(errMsg);
    }

    lt::create_torrent t(fs);
    std::vector<char> tFile;
    auto e = t.generate();
    if (e.type() == lt::entry::undefined_t) {
        std::string errMsg = "Can't create torent file for " + path;
        PLOG_ERROR << errMsg;
        throw std::runtime_error(errMsg);
    }

    lt::bencode(std::back_inserter(tFile), e);

    std::string fileName = cfg.GetTorrentFilesDirectory() + "/" + common::filesystem::GetFileName(path) + ".torrent";
    common::filesystem::File outFile(fileName);
    outFile.Write(tFile.data(), tFile.size());
    AddTorrent(fileName, uuid);
}

std::string Torrent::GetMagnet(const std::string &uuid) const
{
    std::lock_guard<std::mutex> locker(paramsMtx);
    std::string magnet;
    if(torrents.find(uuid) != torrents.end()) {
        magnet = lt::make_magnet_uri(*torrents.at(uuid)->ti);
        PLOG_DEBUG << "Magnet link for " << uuid << " : " << magnet;
    }
    PLOG_DEBUG_IF(magnet.empty()) << "Can't find uuid " << uuid;
    return magnet;
}

void Torrent::DownloadAsync(const std::string &link)
{
    lt::add_torrent_params param;

    lt::error_code ec;
    lt::parse_magnet_uri(link, param, ec);

    param.save_path = cfg.GetDownloadDirectory();
    std::ifstream ifs(GetResumeFilePath(param), std::ios_base::binary);
    ifs.unsetf(std::ios_base::skipws);
    // TODO: use own File class
    param.resume_data.assign(std::istream_iterator<char>(ifs)
                           , std::istream_iterator<char>());
    if (ec) {
        std::string errMsg = "Invalid magnet URI: " + ec.message();
        PLOG_ERROR << errMsg;
        throw std::runtime_error(errMsg);
    }
    AddTorrent(std::move(param));
}

void Torrent::Handler()
{
    clk::time_point lastSaveResume = clk::now();
    while(IsWork()) {
        std::vector<lt::alert*> alerts;
        session->pop_alerts(&alerts);

        for (lt::alert const* a : alerts) {
            if (auto at = lt::alert_cast<lt::add_torrent_alert>(a)) {
                tHandlers[std::string(at->torrent_name())] = at;
            }

            if (auto t = lt::alert_cast<lt::torrent_finished_alert>(a)) {
                t->handle.save_resume_data();
                tHandlers.erase(t->torrent_name());
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

//            if (auto st = lt::alert_cast<lt::state_update_alert>(a)) {
//                if (st->status.empty()) continue;

                // we only have a single torrent, so we know which one
                // the status is for
//                lt::torrent_status const& s = st->status[0];
//                cout << "\r" << State(s.state) << " "
//                          << (s.download_payload_rate / 1000) << " kB/s "
//                          << (s.total_done / 1000) << " kB ("
//                          << (s.progress_ppm / 10000) << "%) downloaded\x1b[K";
//                cout.flush();
//            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(200));

        // ask the session to post a state_update_alert, to update our
        // state output for the torrent
        session->post_torrent_updates();

        // save resume data once every 30 seconds
        if (clk::now() - lastSaveResume > std::chrono::seconds(30)) {
            for(const auto &it : tHandlers) {
                if(it.second) {
                    it.second->handle.save_resume_data();
                } else {
                    tHandlers.erase(it.first);
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

void Torrent::FindTFilesAndAdd()
{
    auto files = common::filesystem::GetFilesInFolder(cfg.GetTorrentFilesDirectory(), ".*\\.torrent");
    for(const auto &f : files) {
        if(!IsWork())
            return;
        try {
            AddTorrent(cfg.GetDownloadDirectory() + "/" + f);
        } catch(std::runtime_error) {
            continue;
        }
    }
}

void Torrent::AddTorrent(const std::string &fullPath, const std::string &uuid)
{
    lt::add_torrent_params param;
    param.save_path = cfg.GetDownloadDirectory();

    lt::error_code ec;
    param.ti = boost::make_shared<lt::torrent_info>(fullPath, boost::ref(ec), 0);
    if (ec) {
        std::string errMsg = "Can't init torrent file : " + ec.message();
        PLOG_ERROR << errMsg;
        throw std::runtime_error(errMsg);
    }
    AddTorrent(std::move(param), uuid);
}

void Torrent::AddTorrent(lt::add_torrent_params && param, const std::string &uuid)
{
    std::lock_guard<std::mutex> locker(paramsMtx);
    session->async_add_torrent(param);
    params.push_back(std::move(param));
    std::string curUUID(uuid);
    if(curUUID.empty())
        curUUID = common::GenerateUUID();
    params_type::iterator it = params.end();
    torrents[curUUID] = --it;
}

bool Torrent::IsWork() const
{
    std::lock_guard<std::recursive_mutex> locker(workMtx);
    return isWork;
}
