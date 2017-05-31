#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <string>
#include <vector>
#include <fcntl.h>

namespace common {
namespace filesystem {

/**
 * @brief GetFileName - Function returns filename without folder part.
 */
std::string GetFileName(const std::string &fullPath);

/**
 * @brief GetDirectory - Function returns full directory path to the file without filename.
 */
std::string GetDirectory(const std::string &fullPath);

/**
 * @brief CreateFolder - Function recursively creates folders for log file.
 */

void CreateFolder(const std::string &dir);

/**
 * @brief GetFilesInFolder - Function searches for files by specified regexp in the specified directory.
 * @param dir - specified directory to search.
 * @param fileMask - Regular expression for file matching.
 * @return - Returns array of all matched files.
 */
std::vector<std::string> GetFilesInFolder(const std::string &dir, const std::string &fileMask);

bool IsFileExist(const std::string &filePath) noexcept;
void Remove(const std::string &filePath) noexcept(false);

/**
 * @brief The File class - Class to work with files on filesystem.
 * It is the c++ wrapper around standard c functions (open, write, close).
 * Functions could generate std::ios_base::failure exception if some errors will be happen.
 */
class File
{
public:
    /**
     * @brief File - Associate file on filesystems with current class.
     * @param m_fullPath - full path to file on filesystem.
     * @param params - Values are constructed by a bitwise-inclusive OR of flags.
     * To get more information about available flags read linux man page about open function (https://linux.die.net/man/3/open).
     */
    File(const std::string &fullPath, int params = O_CREAT | O_RDWR | O_TRUNC);
    ~File();

    File(const File &) = delete;
    File & operator=(const File &) = delete;

    void Open() noexcept(false);
    bool IsOpened() const;

    void Write(const std::string &data, ssize_t length = -1) noexcept(false);
    void Trunc() noexcept(false);
    void Remove() noexcept(false);
    void Close() noexcept(false);
    void ForceClose() noexcept;
    void Move(std::string &newPath) noexcept(false);
    int GetFileSize() const noexcept(false);

    std::string GetFullPath() const noexcept;

    /**
     * @brief GetFileName - Function returns filename without folder part.
     */
    std::string GetFileName() const noexcept;

    /**
     * @brief GetDirectory - Function returns full directory path to the file without filename.
     */
    std::string GetDirectory() const noexcept;

private:
    void SetFileDescriptor(int fd) noexcept;
    int GetFileDescriptor() const noexcept;
    void SetFileParameters(int params) noexcept;
    int GetFileParameters() const noexcept;
private:
    int m_openFileParams = O_CREAT | O_RDWR | O_TRUNC;
    std::string m_fullPath;
    std::string m_fileName;
    std::string m_dirName;
    int m_fileDescriptor = -1;
};

} //namespace filesystem
} //namespace common
#endif // FILESYSTEM_H
