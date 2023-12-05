# Versión mínima de CMake para construir este proyecto
cmake_minimum_required(VERSION 3.15) # Utilizar una versión más reciente para mejores características

# Declaración y nombramiento del proyecto
project(c3ms LANGUAGES CXX)

# Especificar el compilador y sus opciones
set(CMAKE_CXX_STANDARD 17) # Especificar el estándar de C++, ajustar según sea necesario
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Opciones de compilación
set(CMAKE_CXX_FLAGS_DEBUG "-Wall -Wextra -Werror -pedantic -g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -Wall -Wextra -pedantic")

# Definición de subdirectorios
add_subdirectory(src/)