#include "torrent_info.h"

using namespace torrent;

TorrentInfo::TorrentInfo(const std::string &torrentId)
    : tId(torrentId)
{
}

TorrentInfo::TorrentInfo(const TorrentInfo &info) noexcept
    : tId(info.tId)
    , tLink(info.tLink)
    , tPath(info.tPath)
    , tStatus(info.tStatus)
    , tProgress(info.tProgress)
{

}

TorrentInfo::TorrentInfo(TorrentInfo &&info) noexcept
    : tId(std::move(info.tId))
    , tLink(std::move(info.tLink))
    , tPath(std::move(info.tPath))
    , tStatus(std::move(info.tStatus))
    , tProgress(std::move(info.tProgress))
{
}

TorrentInfo & TorrentInfo::operator=(const TorrentInfo &info) noexcept
{
    if(&info != this) {
        tId = info.tId;
        tLink = info.tLink;
        tPath = info.tPath;
        tStatus = info.tStatus;
        tProgress = info.tProgress;
    }
    return *this;
}

TorrentInfo & TorrentInfo::operator=(TorrentInfo &&info) noexcept
{
    if(&info != this) {
        tId = (std::move(info.tId));
        tLink = (std::move(info.tLink));
        tPath = (std::move(info.tPath));
        tStatus = (std::move(info.tStatus));
        tProgress = (std::move(info.tProgress));
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

void TorrentInfo::SetPath(const std::string &path) noexcept
{
    std::lock_guard<std::mutex> locker(mtx);
    tPath = path;
}

std::string TorrentInfo::GetPath() const noexcept
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
