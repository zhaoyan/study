cmake_minimum_required(VERSION 3.21)
project(CppCoroutines-Tasks)

set(CMAKE_CXX_STANDARD 20)

#include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
#conan_basic_setup()

find_package(httplib REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(nlohmann_json REQUIRED)

add_executable(httpdemo
        main.cpp
        io_utils.cpp)

#target_link_libraries(httpdemo ${CONAN_LIBS})
target_link_libraries(httpdemo httplib::httplib openssl::openssl nlohmann_json::nlohmann_json)