cmake_minimum_required(VERSION 3.10)

project(TCPServer VERSION 0.1)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(TCPServer
  main.cpp
  SoundMonitor.cpp
  TCPServer.cpp
  TCPConnection.cpp
  )
