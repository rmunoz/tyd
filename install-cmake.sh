#!/bin/bash
#
# Install cmake 3.5 under travis
CMAKE_URL="http://www.cmake.org/files/v3.5/cmake-3.5.0-Linux-x86_64.tar.gz"
wget ${CMAKE_URL} --no-check-certificate
mkdir cmake
tar -xzf cmake-3.5.0-Linux-x86_64.tar.gz -C cmake --strip-components=1
