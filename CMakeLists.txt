cmake_minimum_required(VERSION 3.22)
project(sc_classes)

set(CMAKE_CXX_STANDARD 14)

include_directories(spdlog-1.x/include)

add_executable(sc_classes
        Buffer.cpp
        Buffer.h
        Channel.cpp
        Channel.h
        main.cpp
        Network.cpp
        Network.h
        Radar.cpp
        Radar.h
        Simulator.cpp
        Simulator.h
        User.cpp
        User.h)
