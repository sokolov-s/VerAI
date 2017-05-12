#include "config.h"
#include "common/filesystem.h"
#include <exception>
#include <sstream>
#include <boost/lexical_cast.hpp>
#include <boost/property_tree/json_parser.hpp>

using namespace config;
namespace pt = boost::property_tree;
using namespace std;

Config::Config()
{
    try {
        Init();
    } catch(runtime_error &err) {
        PLOG_FATAL << err.what();
        throw;
    }
}

Config &Config::GetInstance()
{
    static Config cfg;
    return cfg;
}

int Config::GetInt(const string &key, const int &defValue)
{
    return boost::lexical_cast<int>(GetString(key, boost::lexical_cast<string>(defValue)));
}

void Config::WriteInt(const string &key, const int value)
{
    WriteString(key, boost::lexical_cast<string>(value));
}

string Config::GetString(const string &key, const string &defValue)
{
    lock_guard<mutex> locker(mtx);
    string res = defValue;
    try {
        res = root.get<string>(key, defValue);
    } catch(const pt::ptree_bad_path &) {
        WriteString(key, defValue);
    }

    return res;
}

void Config::WriteString(const string &key, const string &value)
{
    lock_guard<mutex> locker(mtx);
    root.put(key, value);
    pt::write_json(GetConfigPath(), root);
}

string Config::GetFolder() const
{
    return defFolder;
}

void Config::Init()
{
    struct stat buffer;
    if(stat(GetConfigPath().c_str(), &buffer) == 0) {
        try {
            pt::read_json(GetConfigPath(), root);
        } catch(...) {
            throw runtime_error("Can't parse config file : " + GetConfigPath());
        }
    } else {
        common::filesystem::FileSystem::CreateFolder(GetFolder());
    }
}

string Config::GetConfigPath() const
{
    return defConfigPath;
}
