cmake_minimum_required(VERSION 3.21)
project(exorcists C)

set(CMAKE_C_STANDARD 23)

add_executable(main.c)

find_package(MPI REQUIRED)
target_link_libraries(exorcists PUBLIC MPI::MPI_C)
