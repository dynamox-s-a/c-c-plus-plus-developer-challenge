#!/bin/bash
cd build/
#cppcheck --project=compile_commands.json
cppcheck --std=c++17 --enable=all --inconclusive --force symbol.h