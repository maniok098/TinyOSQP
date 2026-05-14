#!/usr/bin/bash
rm -rf ./build && cmake -B build && cmake --build build -j && ./build/OSQP_Example 