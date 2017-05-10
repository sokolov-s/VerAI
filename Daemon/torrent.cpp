#include "torrent.h"

#include "libtorrent/entry.hpp"
#include "libtorrent/bencode.hpp"
#include "libtorrent/session.hpp"
#include "libtorrent/torrent_info.hpp"
#include "libtorrent/settings_pack.hpp"

#include <boost/make_shared.hpp>

using namespace torrent;
using namespace libtorrent;
namespace lt = libtorrent;

Torrent::Torrent()
    : cfg(config::ConfigTorrent::GetInstance())
{
}

void Torrent::Start()
{

    std::string tfile = "/home/serhii/Downloads/rutracker.org.t4976066.torrent";

    settings_pack sett;
    sett.set_str((int)settings_pack::listen_interfaces, std::string("0.0.0.0:6881"));
    lt::session s(sett);
    add_torrent_params p;
    p.save_path = "./";
    error_code ec;
    p.ti = boost::make_shared<torrent_info>(tfile, boost::ref(ec), 0);
    if (ec) {
        throw std::runtime_error("Can't init torrent file : " + ec.message());
    }
    s.add_torrent(p, ec);
    if (ec) {
        throw std::runtime_error("Can't download torrent file : " + ec.message());
    }
}
