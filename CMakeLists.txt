cmake_minimum_required(VERSION 3.1)
project(Cold_Potato_Proxy)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

set(SOURCE_FILES
    src/Base64.cpp
    src/Base64.h
    src/Common.cpp
    src/Common.h
    src/proxy/ProxyConnection.cpp
    src/proxy/ProxyConnection.h
    src/proxy/ProxyServer.cpp
    src/proxy/ProxyServer.h
    src/Constants.h
    src/ConnectionData.cpp
    src/main.cpp
    src/relay/RelayConnection.cpp
    src/relay/RelayConnection.h
    src/Sha1.cpp
    src/Sha1.h
    src/Socket.cpp
    src/Socket.h
    src/Util.cpp
    src/Util.h)

include_directories(src)

find_package (Threads)

add_executable(Cold_Potato_Proxy ${SOURCE_FILES} src/relay/RelayServer.cpp src/relay/RelayServer.h src/ListenServer.cpp src/ListenServer.h src/ConnectionData.h src/Connection.cpp src/Connection.h src/proxy/RelayForwarder.cpp src/proxy/RelayForwarder.h)

target_link_libraries (Cold_Potato_Proxy ${CMAKE_THREAD_LIBS_INIT})