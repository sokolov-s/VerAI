#ifndef VERAI_CONFIG_TORRENT_H
#define VERAI_CONFIG_TORRENT_H

#include "config.h"
#include <Daemon/noncopyable.h>

namespace config {

class ConfigTorrent : private noncopyable::NonCopyable
{
    enum class eKeys {
        kPort,
        kInterface,
        kDownloadFolder
    };

    ConfigTorrent();
public:
    static ConfigTorrent & GetInstance();

    std::string GetPort() const;
    void SetPort(const std::string &port);

    std::string GetInterface() const;
    void SetInterface(const std::string &interface);

    std::string GetDownloadDirectory() const;
    void SetDownloadDirectory(const std::string &dir);
private:

private:
    const std::string sectionName = "torrent";
    const std::string defPort = "6881";
    const std::string defInterface = "0.0.0.0";
    const std::string defDownloadDir = "./";
    Config &cnfg;
    additions::Key<eKeys> keys;
};

} //namespace config
#endif //VERAI_CONFIG_TORRENT_H
