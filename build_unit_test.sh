#!/bin/bash
#=============================================================
# Name do Script : build_unit_test.sh
# Description    : Prepar and start the build process of unit tests of the project. 
# Autor          : Mauricio Peres Ramos
# Data de criação: 04/05/2026
# Versão         : 1.0
# Uso            : ./build_unit_test.sh [opções]
# Requisitos     : bash >= 4.0, comandos necessários (ex: curl, awk)
#=============================================================
# This script is used to build the project's unit testes using CMake and Make. It assumes that you have CMake and Make installed on your system.
# Create a build directory if it doesn't exist
mkdir -p ./unitTest/build
# Change to the build directory
cd ./unitTest/build
# Run CMake to configure the project
cmake ..
# Build the project using Make
make

./mytests

