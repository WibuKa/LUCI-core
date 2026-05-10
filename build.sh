#!/usr/bin/env bash
set -e

mkdir -p build
cmake --build build

cp game sample/game

echo "Done!"
