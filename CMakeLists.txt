cmake_minimum_required(VERSION 3.30)
project(ProjetoLP1 C)

SET(CMAKE_C_FLAGS  "-Wall -Wextra -g -Wvla -Wpedantic -Wdeclaration-after-statement -lm")

add_executable(ProjetoLP1 main.c
        modulo.h
        input.c
        input.h
        simulacao.c
        simulacao.h
        impressao.c
        impressao.h
        interface.c
        interface.h
        memoria.c
        memoria.h
        estruturas.h
)
