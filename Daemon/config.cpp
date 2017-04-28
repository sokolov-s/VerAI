#include "config.h"
#include <exception>
#include <sstream>
#include <boost/lexical_cast.hpp>
#include <boost/property_tree/json_parser.hpp>

using namespace config;
namespace pt = boost::property_tree;

Config::Config()
    : isInit(false)
{
}

Config &Config::GetInstance()
{
    static Config cfg;
    cfg.Init();
    return cfg;
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

void Config::Init()
{
    std::lock_guard<std::mutex> locker(mtx);
    if(isInit)
        return;
    struct stat buffer;
    if(stat(GetConfigPath().c_str(), &buffer) != 0) {
        int fd = open(GetConfigPath().c_str(), O_CREAT | O_RDWR, 0666);
        if(fd > 0) {
            char buf[] = "{\n}\n";
            write(fd, buf, sizeof(buf));
            close(fd);
        } else {
            throw std::runtime_error("Can't create config file : " + GetConfigPath());
        }
    }
    pt::read_json(GetConfigPath(), root);
}

std::string Config::GetConfigPath() const
{
    return configPath;
}


Torrent::Torrent()
    : cnfg(Config::GetInstance())
    , keys({
{eKeys::Port, "torrent.port"},
{eKeys::Interface, "torrent.interface"},
{eKeys::DownloadFolder, "torrent.downloadFolder"},
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
    return cnfg.GetString(Key(eKeys::Port), defPort);
}

void Torrent::SetPort(const std::string &port)
{
    cnfg.WriteString(Key(eKeys::Port), port);
}

std::string Torrent::GetInterface() const
{
    return cnfg.GetString(Key(eKeys::Interface), defInterface);
}

void Torrent::SetInterface(const std::string &interface)
{
    cnfg.WriteString(Key(eKeys::Interface), interface);
}

std::string Torrent::GetDownloadDirectory() const
{
    return cnfg.GetString(Key(eKeys::DownloadFolder), defDownloadDir);
}

void Torrent::SetDownloadDirectory(const std::string &dir)
{
    cnfg.WriteString(Key(eKeys::DownloadFolder), dir);
}

std::string Torrent::Key(const Torrent::eKeys key) const
{
    auto resKey = keys.find(key);
    if(keys.end() == resKey) {
        std::ostringstream msg;
        msg << "Key " << static_cast<int>(key) << " does not found in torrent config keys";
        throw std::invalid_argument(msg.str());
    }
    return resKey->second;
}
