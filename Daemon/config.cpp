#include "config.h"
#include <exception>
#include <sstream>
#include <boost/lexical_cast.hpp>
#include <boost/property_tree/json_parser.hpp>

using namespace config;
namespace pt = boost::property_tree;

void CreateFolder(const std::string &dir)
{
    std::string next_part(dir);
    std::string created_dir;
    struct stat buffer;
    if(dir.empty() || stat(dir.c_str(), &buffer) == 0) {
        return;
    }
    if(next_part[0] == '/') {
        created_dir += "/";
        next_part.erase(0, 1);
    }
    while (!next_part.empty()) {
        size_t pos = next_part.find("/");
        if (pos != std::string::npos) {
            created_dir += next_part.substr(0, pos);
            next_part.erase(0, pos + 1);
        } else {
            created_dir += next_part;
            next_part.clear();
        }
        if(!created_dir.empty() && stat(created_dir.c_str(), &buffer) != 0) {
            if (mkdir(created_dir.c_str(), S_IRWXU | S_IRWXG | S_IRWXO) != 0) {
                return;
            }
            chmod (created_dir.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
        }
        created_dir += "/";
    }
}

Config::Config()
    : isInit(false)
    , keys({
    {eKeys::kVersion, "version"},
           })
{
    CreateFolder(GetFolder());
}

Config &Config::GetInstance()
{
    static Config cfg;
    cfg.Init();
    return cfg;
}

std::string Config::GetVersion() const
{
    return compileVersion;
}

int Config::GetInt(const std::string &key, const int &defValue)
{
    return boost::lexical_cast<int>(GetString(key, boost::lexical_cast<std::string>(defValue)));
}

void Config::WriteInt(const std::string &key, const int value)
{
    WriteString(key, boost::lexical_cast<std::string>(value));
}

std::string Config::GetString(const std::string &key, const std::string &defValue)
{
    std::lock_guard<std::mutex> locker(mtx);
    std::string res = defValue;
    try {
        res = root.get<std::string>(key, defValue);
    } catch(const pt::ptree_bad_path &) {
        WriteString(key, defValue);
    }

    return res;
}

void Config::WriteString(const std::string &key, const std::string &value)
{
    std::lock_guard<std::mutex> locker(mtx);
    root.put(key, value);
    pt::write_json(GetConfigPath(), root);
}

std::string Config::GetFolder() const
{
    return defFolder;
}

void Config::Init()
{
    std::lock_guard<std::mutex> locker(mtx);
    if(isInit)
        return;
    struct stat buffer;
    if(stat(GetConfigPath().c_str(), &buffer) != 0) {
        try {
            root.put("Version", GetVersion());
            pt::write_json(GetConfigPath(), root);
        } catch(...) {
            throw std::runtime_error("Can't create config file : " + GetConfigPath());
        }
    } else {
        try {
            pt::read_json(GetConfigPath(), root);
            if (GetString(keys(eKeys::kVersion), compileVersion) != compileVersion) {
                //TODO: write correct updater here if it necessary
                WriteString("Version", GetVersion());
            }
        } catch(...) {
            throw std::runtime_error("Can't parse config file : " + GetConfigPath());
        }
    }
}

std::string Config::GetConfigPath() const
{
    return defConfigPath;
}


Torrent::Torrent()
    : cnfg(Config::GetInstance())
    , keys({
{eKeys::kPort, "torrent.port"},
{eKeys::kInterface, "torrent.interface"},
{eKeys::kDownloadFolder, "torrent.downloadFolder"},
          })
{
}

Torrent &Torrent::GetInstance()
{
    static Torrent torrent;
    return torrent;
}

std::string Torrent::GetPort() const
{
    return cnfg.GetString(keys(eKeys::kPort), defPort);
}

void Torrent::SetPort(const std::string &port)
{
    cnfg.WriteString(keys(eKeys::kPort), port);
}

std::string Torrent::GetInterface() const
{
    return cnfg.GetString(keys(eKeys::kInterface), defInterface);
}

void Torrent::SetInterface(const std::string &interface)
{
    cnfg.WriteString(keys(eKeys::kInterface), interface);
}

std::string Torrent::GetDownloadDirectory() const
{
    return cnfg.GetString(keys(eKeys::kDownloadFolder), defDownloadDir);
}

void Torrent::SetDownloadDirectory(const std::string &dir)
{
    cnfg.WriteString(keys(eKeys::kDownloadFolder), dir);
}
