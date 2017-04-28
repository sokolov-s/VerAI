#ifndef CONFIG_H
#define CONFIG_H

#include "noncopyable.h"
#include <string>
#include <map>
#include <mutex>
#include <atomic>
#include <boost/property_tree/ptree.hpp>

namespace config {

class Config : private noncopyable::NonCopyable
{
    Config();
public:
    static Config & GetInstance();

    int GetInt(const std::string &key, const int &defValue = 0);
    void WriteInt(const std::string &key, const int value);

    std::string GetString(const std::string &key, const std::string &defValue = "");
    void WriteString(const std::string &key, const std::string &value);
private:
    void Init();
    std::string GetConfigPath() const;
private:
    const std::string configPath = "./verai.cfg";
    boost::property_tree::ptree root;
    std::atomic_bool isInit;
    std::mutex mtx;
};

class Torrent : private noncopyable::NonCopyable
{
    enum class eKeys {
        Port,
        Interface,
        DownloadFolder
    };

    Torrent();
public:
    static Torrent & GetInstance();

    std::string GetPort() const;
    void SetPort(const std::string &port);

    std::string GetInterface() const;
    void SetInterface(const std::string &interface);

    std::string GetDownloadDirectory() const;
    void SetDownloadDirectory(const std::string &dir);
private:
    std::string Key(const eKeys key) const;
private:
    const std::string defPort = "6881";
    const std::string defInterface = "0.0.0.0";
    const std::string defDownloadDir = "./";
    Config &cnfg;
    std::map<eKeys, std::string> keys;
};

} //namespace config
#endif // CONFIG_H
