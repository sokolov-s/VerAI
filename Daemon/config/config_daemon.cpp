#include "config_daemon.h"
#include <sstream>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

using namespace config;
using namespace additions;
using namespace std;

ConfigDaemon::ConfigDaemon()
    : cnfg(Config::GetInstance())
    , section("daemon")
    , keys({
        {eKeys::kVersion, section("version")},
        {eKeys::kUUID, section("uuid")},
        {eKeys::kKey, section("key")},
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
        GenerateUUID();
    }
}

void ConfigDaemon::GenerateUUID()
{
    boost::uuids::uuid uuid = boost::uuids::random_generator()();
    ostringstream str;
    str << uuid;
    cnfg.WriteString(keys(eKeys::kUUID), str.str());
    PLOG_DEBUG << "Generated new daemon uuid : " + str.str();
}


string config::ConfigDaemon::GetLogFolder() const
{
    return cnfg.GetFolder();
}
