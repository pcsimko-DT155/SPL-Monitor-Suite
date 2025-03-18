cmake_minimum_required(VERSION 3.10)

project(SPLServer VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(SPLServer
  main.cpp
  I2CDevice.cpp
  SoundMonitor.cpp
  TCPServer.cpp
  TCPConnection.cpp
  )

install(TARGETS SPLServer DESTINATION bin)
