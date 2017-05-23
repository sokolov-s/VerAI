#ifndef VERAI_CONFIG_DAEMON_H
#define VERAI_CONFIG_DAEMON_H

#include "config.h"
#include "common/noncopyable.h"

namespace config {

class ConfigDaemon : private common::noncopyable::NonCopyable {
    enum class eKeys {
        kVersion,
        kUUID,
        kKey,
        kServerName,
        kServerPort
    };

    ConfigDaemon();
public:
    static ConfigDaemon &GetInstance();
    std::string GetVersion() const;
    std::string GetUUID() const;
    std::string GetKey() const;
    std::string GetLogFolder() const;
    std::string GetServerDomainName() const;
    std::string GetServerPort() const;

private:
    void Init();
    void CreateUUID();

private:
    Config &cnfg;
    const additions::Section sectionDaemon;
    const additions::Section sectionRPC;
    additions::Key<eKeys> keys;
};

} //namespace config

#endif //VERAI_CONFIG_DAEMON_H
