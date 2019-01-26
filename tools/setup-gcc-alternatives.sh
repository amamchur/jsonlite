#!/usr/bin/env bash

apt-get install gcc-8 gcc-7 gcc-6 gcc-5 g++-8 g++-7 g++-6 g++-5
update-alternatives --remove-all gcc
update-alternatives --remove-all g++
update-alternatives --remove-all gcov
update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-5 10
update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-6 20
update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-7 30
update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-8 40
update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-5 10
update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-6 20
update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-7 30
update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-8 40
update-alternatives --install /usr/bin/gcov gcov /usr/bin/gcov-5 10
update-alternatives --install /usr/bin/gcov gcov /usr/bin/gcov-6 20
update-alternatives --install /usr/bin/gcov gcov /usr/bin/gcov-7 30
update-alternatives --install /usr/bin/gcov gcov /usr/bin/gcov-8 40