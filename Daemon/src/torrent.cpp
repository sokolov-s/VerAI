#include "torrent.h"
#include "common/filesystem.h"
#include "common/uuid.h"
#include <plog/Log.h>
#include <thread>
#include <future>
#include <fstream>
#include <functional>
#include <chrono>

#include <libtorrent/entry.hpp>
#include <libtorrent/bencode.hpp>
#include <libtorrent/session.hpp>
#include <libtorrent/torrent_info.hpp>
#include <libtorrent/torrent_handle.hpp>
#include <libtorrent/settings_pack.hpp>
#include <libtorrent/add_torrent_params.hpp>
#include <libtorrent/magnet_uri.hpp>
#include <libtorrent/error_code.hpp>
#include <libtorrent/file.hpp>
#include <libtorrent/file_pool.hpp>
#include <libtorrent/storage.hpp>
#include <libtorrent/hasher.hpp>
#include <libtorrent/hex.hpp>
#include <libtorrent/create_torrent.hpp>
#include <libtorrent/time.hpp>

#include <boost/make_shared.hpp>

using namespace torrent;
namespace lt = libtorrent;
using clk = std::chrono::steady_clock;

std::string ToString(lt::sha1_hash const& s)
{
    std::stringstream ret;
    ret << s;
    return ret.str();
}

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
//    settings.set_str(lt::settings_pack::listen_interfaces, cfg.GetInterface() + ":" + cfg.GetPort());
    settings.set_str(lt::settings_pack::user_agent, "VerAI_client/" LIBTORRENT_VERSION);
    settings.set_int(lt::settings_pack::alert_mask
                     , lt::alert::error_notification
                     | lt::alert::storage_notification
                     | lt::alert::status_notification
                     | lt::alert::progress_notification
                     | lt::alert::dht_notification
                     | lt::alert::dht_log_notification);
    settings.set_bool(lt::settings_pack::enable_dht, true);
    settings.set_bool(lt::settings_pack::enable_lsd, true);
    settings.set_bool(lt::settings_pack::enable_natpmp, true);
    settings.set_bool(lt::settings_pack::enable_upnp, true);
    settings.set_bool(lt::settings_pack::enable_incoming_tcp, true);
    settings.set_bool(lt::settings_pack::enable_outgoing_tcp, true);
    settings.set_bool(lt::settings_pack::enable_incoming_utp, true);
    settings.set_bool(lt::settings_pack::enable_outgoing_utp, true);
//    settings.set_str(lt::settings_pack::dht_bootstrap_nodes, "router.utorrent.com:6881,router.bittorrent.com:6881,dht.transmissionbt.com:6881,router.bitcomet.com:6881,dht.aelitis.com:6881");

    session.reset(new lt::session(settings));

    lt::dht_settings dht;
    dht.privacy_lookups = true;
    session->set_dht_settings(dht);

    isWork = true;
    handlersThread = std::thread(&Torrent::HandlerAlerts, this);
    PLOG_DEBUG << "Torrent started";
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
    session.release();
    std::lock_guard<std::recursive_mutex> lockerList(torrentIdListMtx);
    torrentsIdList.clear();
    PLOG_DEBUG << "Torrent stoped";
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
    PLOG_INFO << "CreateTorrent : id = " << uuid << "; path to project = " << path;
    auto info = infoManager.Create(uuid);
    info->SetStatus(TorrentInfo::Status::GENERATING);
    info->SetPathToProject(path);

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
//    t.add_node(std::make_pair("router.utorrent.com", 6881));
//    t.add_node(std::make_pair("192.168.150.130", 6881));
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
        auto tInfo = CreateInfoFromFile(fileName);
        std::string magnet = lt::make_magnet_uri(tInfo);
        info->SetLink(magnet);
        info->SetPathToTFile(fileName);
        info->SetProgress(100);
        info->SetStatus(TorrentInfo::Status::GENERATED);
        PLOG_DEBUG << "Torrent " << info->GetId() << " generating status : success";
    } catch(const std::runtime_error &) {
        UpdateStatus(uuid, TorrentInfo::Status::GENERATION_ERROR, 0);
        PLOG_ERROR << "Torrent " << info->GetId() << " generating status : failed";
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
}

std::string Torrent::GetIdByName(const std::string &name) const
{
    std::lock_guard<std::recursive_mutex> locker(torrentIdListMtx);
    for(const auto &it : torrentsIdList) {
        if(it.second.param.name == name) {
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
    PLOG_INFO << "Generate magnet link for " << uuid << " torrent file";
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
    PLOG_INFO << "Download torrent file " << uuid << " with magnet link : " << link;
    auto info = infoManager.Create(uuid);
    info->SetLink(link);
    info->SetStatus(TorrentInfo::Status::DOWNLOADING);

    lt::add_torrent_params param;

    lt::error_code ec;
    lt::parse_magnet_uri(link, param, ec);
    if (ec) {
        std::string errMsg = "Invalid magnet URI: " + ec.message();
        PLOG_ERROR << errMsg;
        UpdateStatus(uuid, TorrentInfo::Status::DOWNLOADING_ERROR, 0);
        throw std::runtime_error(errMsg);
    }
    info->SetPathToProject(cfg.GetDownloadDirectory() + "/" + param.name);

    param.save_path = cfg.GetDownloadDirectory();
    param.url = link;
    common::filesystem::CreateFolder(common::filesystem::GetDirectory(GetResumeFilePath(param)));
    std::ifstream ifs(GetResumeFilePath(param), std::ios_base::binary);
    ifs.unsetf(std::ios_base::skipws);
    param.resume_data.assign(std::istream_iterator<char>(ifs)
                           , std::istream_iterator<char>());
    AddTorrent(uuid, std::move(param));
}

void Torrent::HandlerAlerts()
{
    while(IsWork()) {
//        auto a = session->wait_for_alert(lt::milliseconds(1000 * 5));
//        if (a == nullptr)
//            continue;
        std::vector<lt::alert*> alerts;
        session->pop_alerts(&alerts);

        for (lt::alert const* alert : alerts) {
            if (auto ap = lt::alert_cast<lt::dht_stats_alert>(alert)) {
                PLOG_INFO << "DHT active_requests size = " << ap->active_requests.size();
                PLOG_INFO << "DHT routing_table size = " << ap->routing_table.size();
//                dht_active_requests = p->active_requests;
//                dht_routing_table = p->routing_table;
            }

            if (auto ap = lt::alert_cast<lt::add_torrent_alert>(alert)) {
                auto id = GetIdByName(ap->torrent_name());
                if (ap->error) {
                    if(!id.empty()) {
                        UpdateStatus(id, TorrentInfo::Status::DOWNLOADING_ERROR, 0);
                    }
                    PLOG_ERROR << "Failed to add torrent: "
                               << std::string(ap->params.ti ? ap->params.ti->name() : ap->params.name)
                               << std::string(" ") << ap->error.message();

                } else {
                    auto handle = ap->handle;
                    if(!id.empty()) {
                        std::lock_guard<std::recursive_mutex> locker(torrentIdListMtx);
                        torrentsIdList[id].handle = handle;
                    }
                    handle.save_resume_data(lt::torrent_handle::save_info_dict | lt::torrent_handle::only_if_modified);
                }
            }

            if (auto ap = lt::alert_cast<lt::torrent_finished_alert>(alert)) {
                ap->handle.save_resume_data();
                auto id = GetIdByName(ap->torrent_name());
                if(!id.empty()) {
                    UpdateStatus(id, TorrentInfo::Status::DOWNLOADED, 100);
                }
                auto info = infoManager.Get(id);
                info->SetPathToTFile(cfg.GetTorrentFilesDirectory() + "/" + ap->torrent_name());
                lt::torrent_handle handle = ap->handle;
                handle.save_resume_data(lt::torrent_handle::save_info_dict);
                PLOG_DEBUG << "Downloading of " << ap->torrent_name() << " has been finished : " << alert->message();
            }

            if (auto ap = lt::alert_cast<lt::torrent_error_alert>(alert)) {
                auto id = GetIdByName(ap->torrent_name());
                if(!id.empty()) {
                    UpdateStatus(id, TorrentInfo::Status::DOWNLOADING_ERROR, 0);
                }
                PLOG_ERROR << "Some error with torrent " << ap->torrent_name() << " : " << alert->message();
            }

            if (auto ap = lt::alert_cast<lt::save_resume_data_alert>(alert)) {
                lt::torrent_handle handle = ap->handle;
                if(ap->resume_data) {
                    lt::torrent_status status = handle.status(lt::torrent_handle::query_save_path);
                    std::ofstream of(GetResumeFilePath(status.info_hash), std::ios_base::binary);
                    of.unsetf(std::ios_base::skipws);
                    lt::bencode(std::ostream_iterator<char>(of)
                                , *ap->resume_data);
                    if (handle.is_valid()) {
//                            && non_files.find(h) == non_files.end()
//                            && std::find_if(files.begin(), files.end()
//                                            , boost::bind(&handles_t::value_type::second, _1) == h) == files.end())
                        session->remove_torrent(handle);
                    }
                }

            }
            if (auto ap = lt::alert_cast<lt::torrent_paused_alert>(alert)) {
                lt::torrent_handle handle = ap->handle;
                handle.save_resume_data();
            }
            //TODO: write correct code to update status
            if (auto st = lt::alert_cast<lt::state_update_alert>(alert)) {
                if (st->status.empty()) {
                    continue;
                }
                for(const auto &status : st->status) {
                    auto id = GetIdByName(status.name);
                    if(!id.empty()) {
                        auto info = infoManager.Get(id);
                        if(status.state == lt::torrent_status::state_t::finished) {
                            info->SetProgress(100);
                        } else {
                            info->SetProgress(status.progress_ppm / 10000);
                        }
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

        session->post_torrent_updates();
        session->post_session_stats();
        session->post_dht_stats();
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
    return GetResumeFilePath(param.info_hash);
}

std::string Torrent::GetResumeFilePath(lt::sha1_hash const& infoHash) const
{
    return cfg.GetDownloadDirectory() + "/.resume/" + ToString(infoHash) + ".resume";
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
