#ifndef CONFIG_H
#define CONFIG_H

#include "common/noncopyable.h"
#include <string>
#include <map>
#include <mutex>
#include <boost/property_tree/ptree.hpp>
#include <plog/Log.h>

namespace config {

namespace additions {

template<typename T>
class Key {
public:
    Key(const std::map<T, std::string> & initKeys) noexcept
        : keys(initKeys)
    {
    }

    Key(std::map<T, std::string> && initKeys) noexcept
        : keys(std::move(initKeys))
    {
    }

    std::string operator()(const T & key) const noexcept(false)
    {
        auto resKey = keys.find(key);
        if(keys.end() == resKey) {
            std::string msg;
            msg = "Key " + std::to_string(static_cast<int>(key)) + " does not found in config keys";
            PLOG_WARNING << msg;
            throw std::invalid_argument(msg);
        }
        return resKey->second;
    }
private:
    std::map<T, std::string> keys;
};

class Section {
public:
    Section(const std::string & sectionName) noexcept : name(sectionName){}
    Section(std::string && sectionName) noexcept : name(std::move(sectionName)){}
    std::string operator()(const std::string & key) const noexcept {return name + "." + key;}
private:
    std::string name;
};
} //namespace additions

class Config : private common::noncopyable::NonCopyable
{
    Config();
public:
    static Config & GetInstance();

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
    std::mutex mtx;
};

} //namespace config
#endif // CONFIG_H
