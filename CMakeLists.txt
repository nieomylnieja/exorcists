cmake_minimum_required(VERSION 3.21)
project(exorcists C)

set(CMAKE_C_STANDARD 23)

add_executable(exorcists
        main.h main.c
        exorcist.h
        state.h
        log.h log.c
        message.h message.c
        request_handlers.h request_handlers.c
        state_handlers.h state_handlers.c
        setup.c setup.h)

find_package(MPI REQUIRED)
find_package(Threads REQUIRED)
target_link_libraries(exorcists PUBLIC MPI::MPI_C Threads::Threads)
