#!/bin/bash
sudo apt-get install python-xattr cmake automake autoconf libtool bison gcc g++ libboost-all-dev libboost-dev python libicu-dev openssl libssl-dev checkinstall \
apt-transport-https ca-certificates curl software-properties-common linux-image-extra-$(uname -r) linux-image-extra-virtual libprotobuf-dev protobuf-compiler pkg-config \
google-mock

curl -fsSL https://download.docker.com/linux/ubuntu/gpg | sudo apt-key add -

sudo add-apt-repository "deb [arch=amd64] https://download.docker.com/linux/ubuntu $(lsb_release -cs) stable"

sudo apt-get update
sudo apt-get install docker-ce
