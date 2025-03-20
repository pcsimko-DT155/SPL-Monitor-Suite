cmake_minimum_required(VERSION 3.10)

project(SPLSuite VERSION 0.1)

set(CMAKE_INSTALL_PREFIX "/opt/SPL-Suite")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(server)
add_subdirectory(kiosk)
