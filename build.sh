#!/bin/bash

mkdir build
cmake -S . -B build/ -G "$1" -DBUILD_UNIT_TESTS=OFF -DBUILD_EXTRAS=OFF -DBUILD_BULLET2_DEMOS=OFF
