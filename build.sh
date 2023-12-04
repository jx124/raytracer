#!/bin/sh

case $1 in
  --debug|-d)
    mkdir -p ./build/debug
    cmake -H. -B build/debug -DCMAKE_BUILD_TYPE=debug
    cmake --build ./build/debug && ./build/debug/raytracer
    ;;
  --release|-r)
    mkdir -p ./build/release
    cmake -H. -B build/release -DCMAKE_BUILD_TYPE=release
    cmake --build ./build/release && ./build/release/raytracer
    ;;
  *)
    echo "Usage: ./build.sh [--release|-r|--debug|-d]"
    ;;
esac