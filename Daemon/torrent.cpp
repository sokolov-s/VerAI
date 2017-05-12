#include "torrent.h"
#include "common/filesystem.h"
#include <plog/Log.h>
#include <chrono>
#include <thread>
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
    std::lock_guard<std::mutex> locker(handlersMtx);
    if(isWork) {
        return;
    }
    lt::settings_pack settings;
    settings.set_str(lt::settings_pack::listen_interfaces, cfg.GetInterface() + ":" + cfg.GetPort());
    settings.set_int(lt::settings_pack::alert_mask
                 , lt::alert::error_notification
                 | lt::alert::storage_notification
                 | lt::alert::status_notification);

    session.reset(new lt::session(settings));
    //TODO: get magnet links from server.
    auto files = common::filesystem::FileSystem::GetFilesInFolder(cfg.GetDownloadDirectory(), ".*\\.torrent");
    for(const auto &f : files) {
        libtorrent::add_torrent_params param;
        param.save_path = cfg.GetDownloadDirectory();

        lt::error_code ec;
        param.ti = boost::make_shared<lt::torrent_info>(cfg.GetDownloadDirectory() + "/" + f, boost::ref(ec), 0);
        if (ec) {
            throw std::runtime_error("Can't init torrent file : " + ec.message());
        }
        session->async_add_torrent(param);
        params.push_back(std::move(param));
    }
    handlersThread = std::thread(std::bind(&Torrent::Handler, this));
    isWork = true;
    PLOG_INFO << "Torrent started";
}

void Torrent::Stop()
{
    if(handlersThread.joinable()) {
        {
            std::lock_guard<std::mutex> locker(handlersMtx);
            isWork = false;
        }
        handlersThread.join();
    }
    session.release();
    params.clear();
    PLOG_INFO << "Torrent stoped";
}

void Torrent::Handler()
{
    clk::time_point lastSaveResume = clk::now();
    for (;;) {
        {
            std::lock_guard<std::mutex> locker(handlersMtx);
            if(!isWork)
                break;
        }
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
                std::ofstream of(cfg.GetDownloadDirectory() + "/." + rd->torrent_name() + ".resume_file", std::ios_base::binary);
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

std::string Torrent::State(libtorrent::torrent_status::state_t s)
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
