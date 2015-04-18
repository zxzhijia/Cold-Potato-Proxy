cmake_minimum_required(VERSION 3.1)
project(Cold_Potato_Proxy)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

set(SOURCE_FILES
    src/Address.cpp
    src/Address.h
    src/Base64.cpp
    src/Base64.h
    src/Common.cpp
    src/Common.h
    src/ProxyConnection.cpp
    src/ProxyConnection.h
    src/Constants.h
    src/main.cpp
    src/ProxyServer.cpp
    src/ProxyServer.h
    src/Sha1.cpp
    src/Sha1.h
    src/Socket.cpp
    src/Socket.h
    src/Util.cpp
    src/Util.h)

include_directories(src)

find_package (Threads)

add_executable(Cold_Potato_Proxy ${SOURCE_FILES} src/Relay.cpp src/Relay.h src/ListenServer.cpp src/ListenServer.h)

target_link_libraries (Cold_Potato_Proxy ${CMAKE_THREAD_LIBS_INIT})