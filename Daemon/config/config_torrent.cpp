#include "config_torrent.h"

using namespace config;

ConfigTorrent::ConfigTorrent()
    : cnfg(Config::GetInstance())
    , keys({
               {eKeys::kPort, sectionName + ".port"},
               {eKeys::kInterface, sectionName + ".interface"},
               {eKeys::kDownloadFolder, sectionName + ".downloadFolder"},
           })
{
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
    return cnfg.GetString(keys(eKeys::kDownloadFolder), defDownloadDir);
}

void ConfigTorrent::SetDownloadDirectory(const std::string &dir)
{
    cnfg.WriteString(keys(eKeys::kDownloadFolder), dir);
}
