cmake_minimum_required(VERSION 3.10)

project(TCPServer VERSION 0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(TCPServer
  main.cpp
  I2CDevice.cpp
  SoundMonitor.cpp
  TCPServer.cpp
  TCPConnection.cpp
  )
