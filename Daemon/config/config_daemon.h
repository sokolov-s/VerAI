#ifndef VERAI_CONFIG_DAEMON_H
#define VERAI_CONFIG_DAEMON_H

#include "../noncopyable.h"
#include "config.h"

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
    Config &cnfg;
    const additions::Section section;
    additions::Key <eKeys> keys;
};

} //namespace config

#endif //VERAI_CONFIG_DAEMON_H
