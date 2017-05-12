#include "filesystem.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <regex>
#include <boost/filesystem.hpp>

using namespace common::filesystem;

void FileSystem::CreateFolder(const std::string &dir)
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

std::vector<std::string> FileSystem::GetFilesInFolder(const std::string &dir, const std::string &fileMask)
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
