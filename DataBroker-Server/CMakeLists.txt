cmake_minimum_required(VERSION 3.15)
project(DataBroker
        VERSION 1.0.0
        DESCRIPTION "Data-Broker"
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


include_directories(include)


find_library(ZMQ_LIB zmq REQUIRED)
find_path(ZMQ_INCLUDE_DIR zmq.hpp
    PATHS
        /usr/local/include
        /usr/include
    REQUIRED
)


find_package(OpenSSL REQUIRED)
find_package(Boost REQUIRED COMPONENTS system)
find_package(jsoncpp REQUIRED)
find_package(CURL REQUIRED)
find_package(Threads REQUIRED)



add_library(websocketpp INTERFACE)
target_include_directories(websocketpp 
    INTERFACE 
    ${CMAKE_SOURCE_DIR}/websocketpp
)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(
        -O3
        -march=native
        -Wall
        -Wextra
        -Wpedantic
        -ffast-math
    )
elseif(MSVC)
    add_compile_options(
        /O2
        /W4
        /arch:AVX2
    )
endif()


include(CheckIPOSupported)
check_ipo_supported(RESULT IPO_SUPPORTED)
if(IPO_SUPPORTED)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

file(GLOB_RECURSE PROVIDERS_SOURCES src/Providers/*.cpp)
file(GLOB_RECURSE FETCHER_HEADERS include/Fetcher/*.hpp)
file(GLOB_RECURSE PROVIDERS_HEADERS include/Providers/*.hpp)
file(GLOB_RECURSE UTILITIES_HEADERS include/Utilities/*.hpp)
file(GLOB_RECURSE WEBSOCKET_HEADERS include/WebSockets/*.hpp)
file(GLOB_RECURSE WEBSOCKET_SOURCES src/WebSockets/*.cpp)


set(SOURCES
    src/main.cpp
    ${PROVIDERS_SOURCES}
    ${WEBSOCKET_SOURCES}
)

set(HEADERS
    ${FETCHER_HEADERS}
    ${PROVIDERS_HEADERS}
    ${UTILITIES_HEADERS}
    ${WEBSOCKET_HEADERS}
)


add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)


target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${ZMQ_INCLUDE_DIR}
        ${OPENSSL_INCLUDE_DIR}
        ${Boost_INCLUDE_DIRS}
        ${JSONCPP_INCLUDE_DIRS}
        ${CURL_INCLUDE_DIRS}
)


target_link_libraries(${PROJECT_NAME}
    PRIVATE
        ${ZMQ_LIB}
        CURL::libcurl
        jsoncpp_lib
        websocketpp
        Threads::Threads
        OpenSSL::SSL
        OpenSSL::Crypto
        ${Boost_LIBRARIES}
)


if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.16)
    target_precompile_headers(${PROJECT_NAME}
        PRIVATE
        <vector>
        <string>
        <memory>
        <map>
        <unordered_map>
        <zmq.hpp>
    )
endif()


install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)