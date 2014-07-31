#!/bin/sh

cd src/main/table
odb -q --generate-prepared -d sqlite *.hpp
