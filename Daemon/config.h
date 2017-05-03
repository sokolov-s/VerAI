#ifndef CONFIG_H
#define CONFIG_H

#include "noncopyable.h"
#include <string>
#include <map>
#include <mutex>
#include <atomic>
#include <boost/property_tree/ptree.hpp>

namespace config {

template<typename T>
struct Key {
    std::map<T, std::string> keys;
    Key(const std::map<T, std::string> && initKeys) noexcept
        : keys(std::move(initKeys))
    {
    }

    std::string operator()(const T key) const
    {
        auto resKey = keys.find(key);
        if(keys.end() == resKey) {
            std::ostringstream msg;
            msg << "Key " << static_cast<int>(key) << " does not found in config keys";
            throw std::invalid_argument(msg.str());
        }
        return resKey->second;
    }
};

class Config : private noncopyable::NonCopyable
{
    Config();
    enum class eKeys {
        kVersion
    };
public:
    static Config & GetInstance();

    std::string GetVersion() const;

    int GetInt(const std::string &key, const int &defValue = 0);
    void WriteInt(const std::string &key, const int value);

    std::string GetString(const std::string &key, const std::string &defValue = "");
    void WriteString(const std::string &key, const std::string &value);

    std::string GetFolder() const;
private:
    void Init();
    std::string GetConfigPath() const;
private:
    const std::string defFolder = "/opt/VerAI";
    const std::string defConfigPath = defFolder + "/daemon.cfg";
    boost::property_tree::ptree root;
    std::atomic_bool isInit;
    std::mutex mtx;
    const std::string compileVersion = std::string(VERSION);
    Key<eKeys> keys;
};

class Torrent : private noncopyable::NonCopyable
{
    enum class eKeys {
        kPort,
        kInterface,
        kDownloadFolder
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

private:
    const std::string defPort = "6881";
    const std::string defInterface = "0.0.0.0";
    const std::string defDownloadDir = "./";
    Config &cnfg;
    Key<eKeys> keys;
};

} //namespace config
#endif // CONFIG_H
