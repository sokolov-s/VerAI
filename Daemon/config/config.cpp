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
    if(stat(GetConfigPath().c_str(), &buffer) == 0) {
        try {
            pt::read_json(GetConfigPath(), root);
        } catch(...) {
            throw std::runtime_error("Can't parse config file : " + GetConfigPath());
        }
    } else {
        CreateFolder(GetFolder());
    }
    isInit = true;
}

std::string Config::GetConfigPath() const
{
    return defConfigPath;
}
