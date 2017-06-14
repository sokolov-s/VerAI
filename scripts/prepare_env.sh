#!/bin/bash
sudo apt-get install python-xattr cmake automake autoconf libtool bison gcc g++ libboost-all-dev libboost-dev python libicu-dev openssl libssl-dev checkinstall \
apt-transport-https ca-certificates curl software-properties-common linux-image-extra-$(uname -r) linux-image-extra-virtual libprotobuf-dev protobuf-compiler pkg-config \
google-mock libgflags-dev libgtest-dev clang libc++-dev golang-any

#curl -fsSL https://download.docker.com/linux/ubuntu/gpg | sudo apt-key add -

#sudo add-apt-repository "deb [arch=amd64] https://download.docker.com/linux/ubuntu $(lsb_release -cs) stable"

#sudo apt-get update
#sudo apt-get install docker-ce

pushd ..

git submodule update --init

pushd grpc
git checkout v1.3.1 || exit -1
git submodule update --init

pushd third_party/protobuf
git checkout v3.2.0 || exit -1
./autogen.sh || exit -1
./configure || exit -1
#make -j$(nproc --all) || exit -1
#make install || exit -1

popd
#make -j$(nproc --all) || exit -1
#make install || exit -1

popd

pushd libtorrent
git checkout libtorrent-1_1_3 || exit -1

