# Matrix Appication Project

## Arquiteture

### Project Overview

The project matrix application is a application in C++ responsable to process operations with matrices and interact with user through terminal. User through a Cli can choice a list of math operation with matrix to execut. 
The application uses some externa libraries like spdlog and nlohmann/json. 

### Organization

The application is organized in layers.

- src/ - application with the main logic.

- drv/ - drivers / abstraction of low level system to reduce the coupling  with the hardware.

- config/ - Read and management of configuration.

- libs/ - internal libraries.

> [!NOTE]
> Aviso

- **src**

- **drv**

- **config**



## Linux Setup and Instalation

The project is developed in a distro Linux Ubuntu. 

### Cloning the project
---

To clone the project you will need the git instaled.
To instal git open a terminal and execute the command:

> $ sudo apt update

> $ sudo apt install -y git


Now clone the project

> $ git clone https://github.com/peresmauricio/c-c-plus-plus-developer-challenge.git

Enter in the project diretory:
> $ cd c-c-plus-plus-developer-challenge

Initialize and Update Submodules
> $ git submodule update --init --recursive

### Instalation of dependencies and tools

To install all tools and dependencies of the project execute the script "install_deps.sh".
Enter the project directory and run th script.
> $ ./install_deps.sh

## Compiling Project

To compile the project just execute the script "build.sh"
In the project folder execute the command:
> $ ./build.sh



## Code Structure

## APIs Useds

## How to Use the System





## Future Implementation

- Improve the syntaxe analyses to accept determinant operations in a expression.
- Implement a CI flow in the GITHUB to execute regression test.
