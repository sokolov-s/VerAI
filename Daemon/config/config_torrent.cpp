#include "config_torrent.h"

using namespace config;

ConfigTorrent::ConfigTorrent()
    : cnfg(Config::GetInstance())
    , section("torrent")
    , keys({
               {eKeys::kPort, section("port")},
               {eKeys::kInterface, section("interface")},
               {eKeys::kDownloadFolder, section("downloadFolder")},
           })
{
    Init();
}

ConfigTorrent &ConfigTorrent::GetInstance()
{
    static ConfigTorrent torrent;
    return torrent;
}

std::string ConfigTorrent::GetPort() const
{
    return cnfg.GetString(keys(eKeys::kPort), defPort);
}

void ConfigTorrent::SetPort(const std::string &port)
{
    cnfg.WriteString(keys(eKeys::kPort), port);
}

std::string ConfigTorrent::GetInterface() const
{
    return cnfg.GetString(keys(eKeys::kInterface), defInterface);
}

void ConfigTorrent::SetInterface(const std::string &interface)
{
    cnfg.WriteString(keys(eKeys::kInterface), interface);
}

std::string ConfigTorrent::GetDownloadDirectory() const
{
    return cnfg.GetString(keys(eKeys::kDownloadFolder), cnfg.GetFolder() + "/" + defDownloadDir);
}

void ConfigTorrent::SetDownloadDirectory(const std::string &dir)
{
    cnfg.WriteString(keys(eKeys::kDownloadFolder), dir);
}

void ConfigTorrent::Init()
{
    additions::CreateFolder(GetDownloadDirectory());
}
