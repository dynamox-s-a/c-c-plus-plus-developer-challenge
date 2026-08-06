#!/bin/bash

set -e  # encerra se der erro

echo "Atualizando repositórios..."
sudo apt update

echo "Instalando dependências..."
sudo apt install -y \
  build-essential \
  cmake \
  libspdlog-dev \
  libssl-dev \
  cppcheck \
  graphviz \
  doxygen

echo "Dependências instaladas com sucesso ✅"
