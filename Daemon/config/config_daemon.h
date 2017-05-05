#ifndef VERAI_CONFIG_DAEMON_H
#define VERAI_CONFIG_DAEMON_H

#include <Daemon/noncopyable.h>
#include "config.h"
#include <mutex>

namespace config {

class ConfigDaemon : private noncopyable::NonCopyable {
    enum class eKeys {
        kVersion,
        kUUID,
        kKey
    };

    ConfigDaemon();

public:
    static ConfigDaemon &GetInstance();
    std::string GetVersion() const;
    std::string GetUUID() const;
    std::string GetKey() const;

private:
    void Init();
    void GenerateUUID();
private:
    const std::string sectionName = "daemon";
    Config &cnfg;
    additions::Key <eKeys> keys;
    bool isInit = false;
    std::recursive_mutex mtx;
};

} //namespace config

#endif //VERAI_CONFIG_DAEMON_H
