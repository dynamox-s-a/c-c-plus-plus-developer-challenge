# Use uma imagem base mais recente
FROM ubuntu:22.04

# Instala dependências
RUN apt-get update && apt-get install -y \
    g++ \
    build-essential \
    && rm -rf /var/lib/apt/lists/*

# Define o diretório de trabalho
WORKDIR /app

# Copia o executável App para o container
COPY App .

# Comando para executar a aplicação
ENTRYPOINT ["./App"]