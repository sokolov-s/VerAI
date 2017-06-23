#!/bin/bash
packages="python-xattr cmake automake autoconf libtool bison gcc g++ libboost-all-dev libboost-dev python libicu-dev openssl libssl-dev checkinstall \
apt-transport-https ca-certificates curl software-properties-common linux-image-extra-$(uname -r) linux-image-extra-virtual pkg-config \
google-mock libgflags-dev libgtest-dev clang libc++-dev golang-any python3-pip python3-dev python3-virtualenv"

function CheckBinary()
{
    local result=`which $1`;
    if [ -z "${result}" ]
    then
        return 1
    else
        return 0
    fi
}

if ! $(CheckBinary "python3.5")
then
    echo "Add python ver 3.5"
    packages="$packages,python3.5,python3-setuptools"
fi

if ! $(CheckBinary docker)
then
    echo "Add docker repository ..."
    curl -fsSL https://download.docker.com/linux/ubuntu/gpg | sudo apt-key add -

    sudo add-apt-repository "deb [arch=amd64] https://download.docker.com/linux/ubuntu $(lsb_release -cs) stable"

    packages="$packages,docker-ce"
fi

echo "install packages: ${packages}"
sudo apt-get update
sudo apt-get install ${packages}

pushd ..

git submodule update --init

pushd grpc
git checkout v1.3.1 || exit -1
git submodule update --init

pushd third_party/protobuf
git checkout v3.2.0 || exit -1
./autogen.sh || exit -1
./configure --prefix=/usr || exit -1
make -j$(nproc --all) || exit -1
sudo make install || exit -1
sudo ldconfig || exit -1

popd
make -j$(nproc --all) || exit -1
sudo make install || exit -1

popd

pushd libtorrent
git checkout libtorrent-1_1_3 || exit -1

popd
pip3 install tensorflow numpy

