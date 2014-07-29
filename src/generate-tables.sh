#!/bin/sh

cd main/table
odb -q --generate-prepared -d sqlite *.cpp
