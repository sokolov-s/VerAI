#ifndef DAEMON_H
#define DAEMON_H

#include <string>
#include <unordered_map>
#include <vector>
#include <mutex>

namespace daemonspace {

class Daemon
{
    Daemon();
    Daemon(const Daemon &) = delete;
    Daemon& operator=(const Daemon &) = delete;
public:
    static Daemon &GetInstance();

private:
private:
};

} //namespace daemon
#endif // DAEMON_H
