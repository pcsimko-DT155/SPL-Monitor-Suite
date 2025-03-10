cmake_minimum_required(VERSION 3.10)

project(SPLSuite VERSION 0.1)

set(CMAKE_INSTALL_PREFIX "/opt/SPL-Suite")

add_subdirectory(server)
add_subdirectory(kiosk)
