#!/bin/bash
gdb -ex="handle SIG40 noprint nostop" --args ./torrent_grpc_test
