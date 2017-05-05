#ifndef CONFIG_H
#define CONFIG_H

#include "Daemon/noncopyable.h"
#include <string>
#include <map>
#include <mutex>
#include <boost/property_tree/ptree.hpp>

namespace config {

namespace additions {

void CreateFolder(const std::string &dir);

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

    std::string operator()(const T & key) const
    {
        auto resKey = keys.find(key);
        if(keys.end() == resKey) {
            std::ostringstream msg;
            msg << "Key " << static_cast<int>(key) << " does not found in config keys";
            throw std::invalid_argument(msg.str());
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

class Config : private noncopyable::NonCopyable
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
