#!/bin/sh

set -xe

add-apt-repository ppa:ubuntu-toolchain-r/test -y
apt-get update -qq

apt-get install -y \
    g++-4.8

update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-4.8 90
