#include "config_daemon.h"
#include "common/uuid.h"

using namespace config;
using namespace additions;
using namespace std;

ConfigDaemon::ConfigDaemon()
    : cnfg(Config::GetInstance())
    , sectionDaemon("daemon")
    , sectionRPC(sectionDaemon("server"))
    , keys({
        {eKeys::kVersion, sectionDaemon("version")},
        {eKeys::kUUID, sectionDaemon("uuid")},
        {eKeys::kKey, sectionDaemon("key")},
        {eKeys::kServerName, sectionRPC("domainName")},
        {eKeys::kServerPort, sectionRPC("port")},
    })
{
    Init();
}

ConfigDaemon &ConfigDaemon::GetInstance()
{
    static ConfigDaemon daemonCfg;
    return daemonCfg;
}

string ConfigDaemon::GetVersion() const
{
    return cnfg.GetString(keys(eKeys::kVersion));
}

string ConfigDaemon::GetUUID() const
{
    return cnfg.GetString(keys(eKeys::kUUID));
}

string ConfigDaemon::GetKey() const
{
    return cnfg.GetString(keys(eKeys::kKey));
}

void ConfigDaemon::Init()
{
    string curVersion = GetVersion();
    if(curVersion.empty() || curVersion != VERSION) {
        PLOG_INFO << "A new version of daemon will running";
        cnfg.WriteString(keys(eKeys::kVersion), VERSION);
        //TODO: Add update code if it necessary
    }
    if(GetUUID().empty()) {
        CreateUUID();
    }
}

void ConfigDaemon::CreateUUID()
{
    auto uid = common::GenerateUUID();
    cnfg.WriteString(keys(eKeys::kUUID), uid);
    PLOG_DEBUG << "Generated new daemon uuid : " + uid;
}


string config::ConfigDaemon::GetLogFolder() const
{
    return cnfg.GetFolder();
}

string ConfigDaemon::GetServerDomainName() const
{
    return cnfg.GetString(keys(eKeys::kServerName), "192.168.150.130");
}

string ConfigDaemon::GetServerPort() const
{
    return cnfg.GetString(keys(eKeys::kServerPort), "50051");
}
