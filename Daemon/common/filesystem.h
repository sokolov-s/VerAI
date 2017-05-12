#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <string>
#include <vector>

namespace common {
namespace filesystem {

class FileSystem {
public:
    static void CreateFolder(const std::string &dir);

    /**
     * @brief GetFilesInFolder - Function searches for files by specified regexp in the specified directory.
     * @param dir - specified directory to search.
     * @param fileMask - Regular expression for file matching.
     * @return - Returns array of all matched files.
     */
    static std::vector<std::string> GetFilesInFolder(const std::string &dir, const std::string &fileMask);
};


} //namespace filesystem
} //namespace common
#endif // FILESYSTEM_H
