#ifndef VERAI_CONFIG_DAEMON_H
#define VERAI_CONFIG_DAEMON_H

#include "config.h"
#include "common/noncopyable.h"

namespace config {

class ConfigDaemon : private common::noncopyable::NonCopyable {
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
    std::string GetLogFolder() const;
private:
    void Init();
    void CreateUUID();

private:
    Config &cnfg;
    const additions::Section section;
    additions::Key<eKeys> keys;
};

} //namespace config

#endif //VERAI_CONFIG_DAEMON_H
