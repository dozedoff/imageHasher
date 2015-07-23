#!/bin/sh

cd src/main/table
odb -q --std c++11 --generate-prepared -d sqlite -s *.hpp
