#include "config_daemon.h"
#include <sstream>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

using namespace config;

ConfigDaemon::ConfigDaemon()
    : cnfg(Config::GetInstance())
    , keys({
        {eKeys::kVersion, "version"},
        {eKeys::kUUID, "uuid"},
        {eKeys::kKey, "key"},
    })
{
}

ConfigDaemon &ConfigDaemon::GetInstance()
{
    static ConfigDaemon daemonCfg;
    daemonCfg.Init();
    return daemonCfg;
}

std::string ConfigDaemon::GetVersion() const
{
    return cnfg.GetString(keys(eKeys::kVersion));
}

std::string ConfigDaemon::GetUUID() const
{
    return cnfg.GetString(keys(eKeys::kUUID));
}

std::string ConfigDaemon::GetKey() const
{
    return cnfg.GetString(keys(eKeys::kKey));
}

void ConfigDaemon::Init()
{
    std::lock_guard<std::recursive_mutex> locker(mtx);
    if(isInit)
        return;
    std::string curVersion = GetVersion();
    if(curVersion.empty() || curVersion != VERSION) {
        cnfg.WriteString(keys(eKeys::kVersion), VERSION);
        //TODO: Add update code if it necessary
    }
    if(GetUUID().empty()) {
        GenerateUUID();
    }
    isInit = true;
}

void ConfigDaemon::GenerateUUID()
{
    boost::uuids::uuid uuid = boost::uuids::random_generator()();
    std::ostringstream str;
    str << uuid;
    cnfg.WriteString(keys(eKeys::kUUID), str.str());
}

