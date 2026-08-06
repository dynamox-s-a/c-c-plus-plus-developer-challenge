#!/bin/bash
#=============================================================
# Name do Script : build.sh
# Description    : Prepar and start the build process of application. 
# Autor          : Mauricio Peres Ramos
# Data de criação: 04/05/2026
# Versão         : 1.0
# Uso            : ./build.sh [opções]
# Requisitos     : bash >= 4.0, comandos necessários (ex: curl, awk)
#=============================================================

# Ativar modo seguro (opcional, mas recomendado)
# -e: encerra o script se um comando falhar.
# -u: erro se usar variável não definida.
# -o pipefail: falha se qualquer comando no pipe falhar.
set -euo pipefail
# evita problemas com espaços em nomes de arquivos.
IFS=$'\n\t'

# Create a build directory if it doesn't exist
mkdir -p build
# Change to the build directory
cd build
# Run CMake to configure the project - (-DCMAKE_EXPORT_COMPILE_COMMANDS=ON) - Export compile comands
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_CXX_COMPILER=/usr/bin/g++ ..
# Build the project using Make
make    
