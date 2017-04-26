#include "daemon.h"
#include <stdexcept>

using namespace daemonspace;
using namespace std;

Daemon::Daemon()
{
}

Daemon &Daemon::GetInstance()
{
    static Daemon daemon;
    return daemon;
}
