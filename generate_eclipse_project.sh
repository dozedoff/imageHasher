#!/bin/sh

cmake -G Eclipse\ CDT4\ -\ Unix\ Makefiles && rm -r CMakeCache.txt CMakeFiles/ CTestTestfile.cmake Makefile cmake_install.cmake src/test/hash/ src/test/util/
