#include "config_torrent.h"
#include "common/filesystem.h"

using namespace config;
using namespace std;

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

string ConfigTorrent::GetPort() const
{
    return cnfg.GetString(keys(eKeys::kPort), defPort);
}

void ConfigTorrent::SetPort(const string &port)
{
    cnfg.WriteString(keys(eKeys::kPort), port);
}

string ConfigTorrent::GetInterface() const
{
    return cnfg.GetString(keys(eKeys::kInterface), defInterface);
}

void ConfigTorrent::SetInterface(const string &interface)
{
    cnfg.WriteString(keys(eKeys::kInterface), interface);
}

string ConfigTorrent::GetDownloadDirectory() const
{
    return cnfg.GetString(keys(eKeys::kDownloadFolder), cnfg.GetFolder() + "/" + defDownloadDir);
}

void ConfigTorrent::SetDownloadDirectory(const string &dir)
{
    cnfg.WriteString(keys(eKeys::kDownloadFolder), dir);
}

void ConfigTorrent::Init()
{
    SetDownloadDirectory(GetDownloadDirectory());
    SetInterface(GetInterface());
    SetPort(GetPort());
    common::filesystem::FileSystem::CreateFolder(GetDownloadDirectory());
}
