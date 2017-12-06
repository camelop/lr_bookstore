cmake_minimum_required(VERSION 3.9)
project(Nymph)

set(CMAKE_CXX_STANDARD 11)

add_executable(Nymph
        array.h
        book.h
        list.h
        Nymph.cpp
        type.h
        user.h)
