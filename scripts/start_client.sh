#!/bin/bash
CUR_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$CUR_DIR" gdb -ex="set follow-fork-mode child" -ex="handle SIG40 noprint nostop" -ex="set disable-randomization off" --args ./VerAIDaemon
