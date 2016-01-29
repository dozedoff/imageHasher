#!/bin/sh

odb -q --std c++11 --generate-prepared -d sqlite -o src/main/table -s src/main/table/*.hpp
