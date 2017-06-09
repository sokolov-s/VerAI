#include "torrent_info.h"

using namespace torrent;

TorrentInfo::TorrentInfo(const std::string &torrentId)
    : tId(torrentId)
{
}

TorrentInfo::TorrentInfo(const TorrentInfo &info) noexcept
{
    Copy(info);
}

TorrentInfo & TorrentInfo::operator=(const TorrentInfo &info) noexcept
{
    std::lock_guard<std::mutex> locker(mtx);
    if(&info != this) {
        Copy(info);
    }
    return *this;
}

std::string TorrentInfo::GetId() const noexcept
{
    std::lock_guard<std::mutex> locker(mtx);
    return tId;
}

void TorrentInfo::SetLink(const std::string &link) noexcept
{
    std::lock_guard<std::mutex> locker(mtx);
    tLink = link;
}

std::string TorrentInfo::GetLink() const noexcept
{
    std::lock_guard<std::mutex> locker(mtx);
    return tLink;
}

void TorrentInfo::SetPathToProject(const std::string &path) noexcept
{
    std::lock_guard<std::mutex> locker(mtx);
    pPath = path;
}

std::string TorrentInfo::GetPathToProject() const noexcept
{
    std::lock_guard<std::mutex> locker(mtx);
    return pPath;
}

void TorrentInfo::SetPathToTFile(const std::string &path) noexcept
{
    std::lock_guard<std::mutex> locker(mtx);
    tPath = path;
}

std::string TorrentInfo::GetPathToTFile() const noexcept
{
    std::lock_guard<std::mutex> locker(mtx);
    return tPath;
}

void TorrentInfo::SetStatus(const TorrentInfo::Status &status) noexcept
{
    std::lock_guard<std::mutex> locker(mtx);
    tStatus = status;
}

TorrentInfo::Status TorrentInfo::GetStatus() const noexcept
{
    std::lock_guard<std::mutex> locker(mtx);
    return tStatus;
}

void TorrentInfo::SetProgress(const int &progress) noexcept
{
    std::lock_guard<std::mutex> locker(mtx);
    tProgress = progress;
}

uint TorrentInfo::GetProgress() const noexcept
{
    std::lock_guard<std::mutex> locker(mtx);
    return tProgress;
}

void TorrentInfo::Copy(const TorrentInfo &info)
{
    tId = info.GetId();
    tLink = info.GetLink();
    tPath = info.GetPathToTFile();
    pPath = info.GetPathToProject();
    tStatus = info.GetStatus();
    tProgress = info.GetProgress();
}
