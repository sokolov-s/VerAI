#ifndef VERAI_CONFIG_TORRENT_H
#define VERAI_CONFIG_TORRENT_H

#include "config.h"
#include "common/noncopyable.h"

namespace config {

class ConfigTorrent : private common::noncopyable::NonCopyable
{
    enum class eKeys {
        kPort,
        kInterface,
        kDownloadFolder,
        kTFilesFolder,
        kProjectsFolder
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

    std::string GetTorrentFilesDirectory() const;
    void SetTorrentFilesDirectory(const std::string &dir);

    std::string GetProjectsDirectory() const;
    void SetProjectsDirectory(const std::string &dir);
private:
    void Init();

private:
    Config &cnfg;
    const additions::Section section;
    additions::Key<eKeys> keys;
    const std::string defPort = "6881";
    const std::string defInterface = "0.0.0.0";
    const std::string defDownloadDir = "downloads";
    const std::string defTFilesDir = "torrents";
    const std::string defProjectsDir = "projects";
};

} //namespace config
#endif //VERAI_CONFIG_TORRENT_H
