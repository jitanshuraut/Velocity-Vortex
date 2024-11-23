cmake_minimum_required(VERSION 3.15)
project(OrderManager
        VERSION 1.0.0
        DESCRIPTION "High-frequency trading system"
        LANGUAGES CXX)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_library(ZMQ_LIB zmq REQUIRED)
find_path(ZMQ_INCLUDE_DIR zmq.hpp
    PATHS
        /usr/local/include
        /usr/include
    REQUIRED
)

include(CheckIPOSupported)
include_directories(${ZeroMQ_INCLUDE_DIRS})
check_ipo_supported(RESULT IPO_SUPPORTED)
if(IPO_SUPPORTED)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()


if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()


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


set(SOURCE_GROUPS
    ""
    "Executor-Manager-Core"
    "Queue"
    "Scheduler"
)


set(ALL_SOURCES src/main.cpp)
foreach(GROUP ${SOURCE_GROUPS})
    file(GLOB_RECURSE GROUP_SOURCES
        "src/${GROUP}/*.cpp"
        "src/${GROUP}/*.hpp"
    )
    list(APPEND ALL_SOURCES ${GROUP_SOURCES})
    source_group(${GROUP} FILES ${GROUP_SOURCES})
endforeach()


add_executable(${PROJECT_NAME} ${ALL_SOURCES})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)



find_package(OpenSSL REQUIRED)
find_package(Boost REQUIRED COMPONENTS system)
find_package(jsoncpp REQUIRED)
find_package(CURL REQUIRED)
find_package(Threads REQUIRED)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${ZMQ_INCLUDE_DIR}
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    
        CURL::libcurl
        jsoncpp_lib
        Threads::Threads
        OpenSSL::SSL
        OpenSSL::Crypto
        ${Boost_LIBRARIES}
        ${ZMQ_LIB}
)


target_compile_options(${PROJECT_NAME} PRIVATE ${TORCH_CXX_FLAGS})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pipe")

if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.16)
    target_precompile_headers(${PROJECT_NAME}
        PRIVATE
        <vector>
        <string>
        <memory>
        <map>
        <unordered_map>
    )
endif()

# Add support for CodeQL
# if(DEFINED ENV{CODEQL_EXTRACTOR_CXX_TRACER_CONFIGURATION})
#     message(STATUS "CodeQL build detected")
#     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g")
# endif()

# # Ensure all required libraries are installed
# include(ExternalProject)
# ExternalProject_Add(
#     hiredis
#     GIT_REPOSITORY https://github.com/redis/hiredis.git
#     GIT_TAG v1.0.0
#     CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/hiredis
# )
# ExternalProject_Add(
#     sqlite3
#     GIT_REPOSITORY https://github.com/sqlite/sqlite.git
#     GIT_TAG version-3.35.5
#     CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/sqlite3
# )

# # Link the installed libraries
# add_dependencies(${PROJECT_NAME} hiredis sqlite3)
# target_link_libraries(${PROJECT_NAME}
#     PRIVATE
#         ${CMAKE_BINARY_DIR}/hiredis/lib/libhiredis.a
#         ${CMAKE_BINARY_DIR}/sqlite3/lib/libsqlite3.a
# )

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)