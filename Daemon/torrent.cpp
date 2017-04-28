#include "torrent.h"

#include <libtorrent/session.hpp>
#include <libtorrent/add_torrent_params.hpp>
#include <libtorrent/torrent_handle.hpp>
#include <libtorrent/settings_pack.hpp>

using namespace torrent;
namespace lt = libtorrent;

Torrent::Torrent()
    : cfg(config::Torrent::GetInstance())
{
}

void Torrent::Start()
{
    lt::settings_pack stp;
    stp.set_str(lt::settings_pack::listen_interfaces, cfg.GetInterface() + ":" + cfg.GetPort());
}
