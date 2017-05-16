#include "filesystem.h"
#include <ios>
#include <cstdlib>
#include <cstring>
#include <regex>
#include <stdexcept>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

using namespace common::filesystem;

std::string common::filesystem::GetFileName(const std::string &fullPath)
{
    size_t pos = fullPath.find_last_of('/');
    if(std::string::npos != pos) {
        return fullPath.substr(pos + 1);
    } else {
        return fullPath;
    }
}

std::string common::filesystem::GetDirectory(const std::string &fullPath)
{
    size_t pos = fullPath.find_last_of('/');
    if(std::string::npos != pos) {
        return fullPath.substr(0, pos);
    } else {
        return "./";
    }
}

void common::filesystem::CreateFolder(const std::string &dir)
{
    std::string next_part(dir);
    std::string created_dir;
    struct stat buffer;
    if(dir.empty() || stat(dir.c_str(), &buffer) == 0) {
        return;
    }
    if(next_part[0] == '/') {
        created_dir += "/";
        next_part.erase(0, 1);
    }
    while (!next_part.empty()) {
        size_t pos = next_part.find("/");
        if (pos != std::string::npos) {
            created_dir += next_part.substr(0, pos);
            next_part.erase(0, pos + 1);
        } else {
            created_dir += next_part;
            next_part.clear();
        }
        if(!created_dir.empty() && stat(created_dir.c_str(), &buffer) != 0) {
            if (mkdir(created_dir.c_str(), S_IRWXU | S_IRWXG | S_IRWXO) != 0) {
                return;
            }
            chmod (created_dir.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
        }
        created_dir += "/";
    }
}

std::vector<std::string> common::filesystem::GetFilesInFolder(const std::string &dir, const std::string &fileMask)
{
    std::vector<std::string> fileList;

    boost::filesystem::directory_iterator endItr;
    for(boost::filesystem::directory_iterator it(dir); it != endItr; ++it)
    {
        if(!boost::filesystem::is_regular_file(it->status()))
            continue;

        const std::regex filter(fileMask);
        if(!std::regex_match(it->path().filename().string(), filter))
            continue;

        fileList.push_back(it->path().filename().string());
    }
    return fileList;
}

File::File(const std::string &fullPath, int params)
    : m_openFileParams(params)
    , m_fullPath(fullPath)
{
    size_t pos = fullPath.find_last_of('/');
    if(std::string::npos != pos) {
        m_dirName = fullPath.substr(0, pos);
        m_fileName = fullPath.substr(pos + 1);
    } else {
        m_dirName = "./";
        m_fileName = fullPath;
    }
}

File::~File()
{
    Close();
}

void File::Open()
{
    if(IsOpened()) {
        return;
    }
    CreateFolder(GetDirectory());
    int fd = open(GetFullPath().c_str(), GetFileParameters() , 0666);
    if(-1 == fd) {
        throw std::ios_base::failure("Can't open file: " + GetFullPath());
    }
    SetFileDescriptor(fd);
}

void File::Write(const std::string &data, ssize_t length)
{
    if(!IsOpened()) {
        throw std::ios_base::failure("File does not open: " + GetFullPath());
    }
    ssize_t bufLength = (-1 == length) ? data.length() : length;
    if(bufLength > 0) {
        if(-1 == write(GetFileDescriptor(), data.c_str(), sizeof(char) * bufLength)) {
            throw std::ios_base::failure("Can't write data to file: " + GetFileName());
        }
    }
}

void File::Trunc()
{
    if(-1 == truncate(GetFullPath().c_str(), 0)) {
        throw std::ios_base::failure("Can't truncate file: " + GetFullPath());
    }
}

void File::Remove()
{
    Close();
    if(-1 == remove(GetFullPath().c_str())) {
        throw std::ios_base::failure("Can't remove file: " + GetFullPath());
    }
}

void File::Close()
{
    if(IsOpened()) {
        if(-1 == close(GetFileDescriptor())) {
            throw std::ios_base::failure("Can't close file: " + GetFullPath());
        }
        SetFileDescriptor(-1);
    }
}

void File::ForceClose()
{
    try {
        Close();
    } catch (const std::ios_base::failure &) {
        SetFileDescriptor(-1);
    }
}

void File::Move(std::string &newPath)
{
    if(IsOpened()){
        Close();
    }
    if(-1 == rename(GetFullPath().c_str(), newPath.c_str())) {
        throw std::ios_base::failure("Can't rename " + GetFullPath() + " to " + newPath);
    }
}

int File::GetFileSize() const
{
    struct stat status;
    if(fstat(GetFileDescriptor(), &status) == -1){
        throw std::ios_base::failure("Can't get stat of file: " + GetFullPath());
    }
    return status.st_size;
}

std::string File::GetFullPath() const
{
    return m_fullPath;
}

std::string File::GetFileName() const
{
    return m_fileName;
}

std::string File::GetDirectory() const
{
    return m_dirName;
}

bool File::IsOpened() const
{
    return (-1 != GetFileDescriptor());
}

void File::SetFileDescriptor(int fd)
{
    m_fileDescriptor = fd;
}

int File::GetFileDescriptor() const
{
    return m_fileDescriptor;
}

void File::SetFileParameters(int params)
{
    m_openFileParams = params;
}

int File::GetFileParameters() const
{
    return m_openFileParams;
}
