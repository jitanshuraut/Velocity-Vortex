cmake_minimum_required(VERSION 3.10)
project(VelocityVortex)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(include)


find_package(CURL REQUIRED)
find_library(HIREDIS_LIB hiredis REQUIRED)
find_library(SQLITE3_LIB sqlite3 REQUIRED)
find_package(jsoncpp REQUIRED)

file(GLOB_RECURSE ALGO_ENGINE_SOURCES
    "src/AlgoEngine-Core/indicators/*.cpp"
)

file(GLOB_RECURSE DATA_FETCHER_SOURCES
    "src/Data-Fetcher-Core/*.cpp"
)

file(GLOB_RECURSE IO_BROKER_SOURCES
    "src/IO-Broker-Core/*.cpp"
)

file(GLOB_RECURSE ORDER_MANAGER_SOURCES
    "src/Order-Manager-Core/*.cpp"
)

file(GLOB_RECURSE RISK_ANALYSIS_SOURCES
    "src/Risk-Analysis-Core/*.cpp"
)

file(GLOB_RECURSE VELOCITY_BOT_SOURCES
    "src/Velocity-Bot/*.cpp"
)


set(UTILITIES_SOURCES
    include/Utilities/Bar.hpp
    include/Utilities/OHLCV.hpp
    include/Utilities/Quote.hpp
    include/Utilities/SignalResult.hpp
    include/Utilities/Trade.hpp
    include/Utilities/Utilities.hpp
)

set(SOURCES
    ${ALGO_ENGINE_SOURCES}
    ${DATA_FETCHER_SOURCES}
    ${IO_BROKER_SOURCES}
    ${ORDER_MANAGER_SOURCES}
    ${RISK_ANALYSIS_SOURCES}
    ${VELOCITY_BOT_SOURCES}
    src/main.cpp # The main program file
)

add_executable(VelocityVortex ${SOURCES})

target_link_libraries(VelocityVortex
    CURL::libcurl
    jsoncpp_lib
    ${HIREDIS_LIB}
    ${SQLITE3_LIB}
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/..)
