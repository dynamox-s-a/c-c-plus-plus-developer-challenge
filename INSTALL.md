# Guia de Instalação – Dynamox Arithmetic Calculator

## Início Rápido (Windows)

### Opção 1: MinGW-w64 (Recomendado)

1. Baixe o MinGW-w64 em: https://www.mingw-w64.org/downloads/
2. Escolha "MingW-W64-builds" → faça o download do instalador
3. Execute o instalador com estas configurações:
   - Arquitetura: x86_64 (64 bits) ou i686 (32 bits)
   - Threads: posix
   - Exception: seh (para x86_64) ou dwarf (para i686)
4. Adicione o diretório `bin` do MinGW ao PATH:
   - Local padrão: `C:\mingw64\bin` ou `C:\Program Files\mingw-w64\...\bin`
   - Adicione em Variáveis de Ambiente do Sistema → PATH
5. Abra um novo Prompt de Comando/PowerShell e verifique: `gcc --version`

### Opção 2: MSYS2 (Alternativa)

1. Baixe o MSYS2 em: https://www.msys2.org/
2. Instale e abra o terminal do MSYS2
3. Atualize a base de pacotes: `pacman -Syu`
4. Instale o GCC: `pacman -S mingw-w64-x86_64-gcc`
5. Adicione `C:\msys64\mingw64\bin` ao PATH

### Opção 3: TDM-GCC (Mais simples)

1. Baixe o TDM-GCC em: https://jmeubank.github.io/tdm-gcc/
2. Execute o instalador (ele adiciona ao PATH automaticamente)
3. Verifique a instalação: `gcc --version`

### Opção 4: Visual Studio Build Tools

1. Baixe o "Build Tools for Visual Studio" no site da Microsoft
2. Instale com o workload "C++ build tools"
3. Use `cl.exe` em vez de `gcc` (o script de build detecta automaticamente)

## Como Compilar o Projeto

### Método 1: Script de build (Windows)

```cmd
.\build.bat
```

### Método 2: Compilação manual

```cmd
gcc -Wall -Wextra -Wpedantic -std=c11 -O2 -o calculator.exe main.c lib/matrix/matrix.c lib/operations/operations.c lib/registry/registry.c lib/logger/logger.c lib/hmi/hmi.c -lm
```

### Método 3: Usando Make (se disponível)

```cmd
make
```

## Executando a Aplicação

Após compilar com sucesso:

```cmd
.\calculator.exe
```

## Solução de Problemas

### "gcc is not recognized"

- O GCC não está no PATH. Siga as etapas de instalação acima.
- Tente usar o caminho completo: `C:\mingw64\bin\gcc.exe ...`

### "undefined reference to `pow`" ou erros matemáticos

- Adicione a flag `-lm` para linkar a biblioteca matemática
- No Windows com MinGW isso costuma funcionar automaticamente

### Erros de "Permission denied"

- Execute o Prompt de Comando como Administrador
- Verifique se o antivírus não está bloqueando a compilação

### Erros "No such file or directory"

- Certifique-se de estar na pasta raiz do projeto
- Confira se todos os arquivos fonte existem em seus respectivos diretórios `lib/`

## Linux/macOS

### Ubuntu/Debian

```bash
sudo apt update
sudo apt install build-essential
make
./calculator
```

### CentOS/RHEL/Fedora

```bash
sudo yum install gcc make  # CentOS/RHEL
sudo dnf install gcc make  # Fedora
make
./calculator
```

### macOS

```bash
# Instale as Xcode Command Line Tools
xcode-select --install
make
./calculator
```

## Verificação

Após a instalação, confirme seu ambiente:

```cmd
gcc --version
```

Deve aparecer algo como:

```
gcc (MinGW-W64 x86_64-posix-seh, built by Brecht Sanders) 12.2.0
```

Depois compile e execute:

```cmd
.\build.bat
.\calculator.exe
```
