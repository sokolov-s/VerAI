#ifndef TORRENT_INFO_H
#define TORRENT_INFO_H

#include <string>
#include <mutex>

namespace torrent {

class TorrentInfo
{
public:
    TorrentInfo(const std::string &torrentId);
    enum Status {
        UNKNOWN,
        DOWNLOADING,
        DOWNLOADED,
        DOWNLOADING_ERROR,
        GENERATING,
        GENERATED,
        GENERATION_ERROR,
        PAUSED,
    };
    TorrentInfo(const TorrentInfo & info) noexcept;
    TorrentInfo(TorrentInfo && info) noexcept;
    TorrentInfo & operator=(const TorrentInfo &info) noexcept;
    TorrentInfo & operator=(TorrentInfo &&info) noexcept;

    std::string GetId() const noexcept;
    void SetLink(const std::string &link) noexcept;
    std::string GetLink() const noexcept;
    void SetPathToProject(const std::string &path) noexcept;
    std::string GetPathToProject() const noexcept;
    void SetPathToTFile(const std::string &path) noexcept;
    std::string GetPathToTFile() const noexcept;
    void SetStatus(const Status &status) noexcept;
    Status GetStatus() const noexcept;
    void SetProgress(const int &progress) noexcept;
    uint GetProgress() const noexcept;
private:

private:
    mutable std::mutex mtx;
    std::string tId;
    std::string tLink;
    std::string tPath;
    std::string pPath;
    Status tStatus = Status::UNKNOWN;
    uint tProgress = 0;
};

} //namespace torrent
#endif // TORRENT_INFO_H
