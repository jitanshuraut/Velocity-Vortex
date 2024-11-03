# cmake_minimum_required(VERSION 3.15)
# project(VelocityVortex)

# set(CMAKE_CXX_STANDARD 20)
# set(CMAKE_CXX_STANDARD_REQUIRED True)
# set(CMAKE_PREFIX_PATH "/usr/local/libtorch")


# find_package(Torch REQUIRED)
# include_directories(include)
# find_package(Python3 COMPONENTS Interpreter Development NumPy REQUIRED)

# include_directories(${Python3_INCLUDE_DIRS})
# include_directories(${Python3_NumPy_INCLUDE_DIRS})


# find_package(OpenSSL REQUIRED)
# find_package(Boost REQUIRED COMPONENTS system)
# find_package(jsoncpp REQUIRED)
# find_package(CURL REQUIRED)


# find_library(HIREDIS_LIB hiredis REQUIRED)
# find_library(SQLITE3_LIB sqlite3 REQUIRED)
# find_package(Threads REQUIRED)


# add_library(websocketpp INTERFACE)
# target_include_directories(websocketpp INTERFACE ${CMAKE_SOURCE_DIR}/websocketpp)


# file(GLOB_RECURSE ALGO_ENGINE_SOURCES "src/AlgoEngine-Core/**/*.cpp")
# file(GLOB_RECURSE DATA_FETCHER_SOURCES "src/Data-Fetcher-Core/*.cpp")
# file(GLOB_RECURSE IO_BROKER_SOURCES "src/IO-Broker-Core/*.cpp")
# file(GLOB_RECURSE ORDER_MANAGER_SOURCES "src/Order-Manager-Core/*.cpp")
# file(GLOB_RECURSE RISK_ANALYSIS_SOURCES "src/Risk-Analysis-Core/*.cpp")
# file(GLOB_RECURSE VELOCITY_BOT_SOURCES "src/Velocity-Bot/*.cpp")
# file(GLOB_RECURSE BACKTESTING_BOT_SOURCES "src/Backtesting-Bot/*.cpp")
# file(GLOB_RECURSE ORDERBOOK_SOURCES "src/Orderbook/*.cpp")
# file(GLOB_RECURSE UTILITY_SOURCES "src/Utilities/*.cpp")


# set(UTILITIES_SOURCES
#     include/Utilities/Bar.hpp
#     include/Utilities/OHLCV.hpp
#     include/Utilities/Quote.hpp
#     include/Utilities/SignalResult.hpp
#     include/Utilities/Trade.hpp
#     include/Utilities/Utilities.hpp
# )


# set(SOURCES
#     ${ALGO_ENGINE_SOURCES}
#     ${DATA_FETCHER_SOURCES}
#     ${IO_BROKER_SOURCES}
#     ${ORDER_MANAGER_SOURCES}
#     ${RISK_ANALYSIS_SOURCES}
#     ${VELOCITY_BOT_SOURCES}
#     ${BACKTESTING_BOT_SOURCES}
#     ${ORDERBOOK_SOURCES}
#     ${UTILITY_SOURCES}
#     src/main.cpp  
# )


# add_executable(VelocityVortex ${SOURCES})


# target_link_libraries(VelocityVortex
#     ${TORCH_LIBRARIES}
#     CURL::libcurl
#     jsoncpp_lib
#     ${HIREDIS_LIB}
#     ${SQLITE3_LIB}
#     ${Python3_LIBRARIES}
#     websocketpp
#     Threads::Threads
#     OpenSSL::SSL
#     OpenSSL::Crypto
#     ${Boost_LIBRARIES}
# )


# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/..)

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")


cmake_minimum_required(VERSION 3.15)
project(VelocityVortex
        VERSION 1.0.0
        DESCRIPTION "High-frequency trading system"
        LANGUAGES CXX)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


include(CheckIPOSupported)
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
    "AlgoEngine-Core"
    "Data-Fetcher-Core"
    "IO-Broker-Core"
    "Order-Manager-Core"
    "Risk-Analysis-Core"
    "Velocity-Bot"
    "Backtesting-Bot"
    "Orderbook"
    "Utilities"
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


set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} "/usr/local/libtorch")
find_package(Torch REQUIRED)
find_package(Python3 COMPONENTS Interpreter Development NumPy REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Boost REQUIRED COMPONENTS system)
find_package(jsoncpp REQUIRED)
find_package(CURL REQUIRED)
find_package(Threads REQUIRED)



find_library(HIREDIS_LIB hiredis REQUIRED)
find_library(SQLITE3_LIB sqlite3 REQUIRED)



add_library(websocketpp INTERFACE)
target_include_directories(websocketpp 
    INTERFACE 
    ${CMAKE_SOURCE_DIR}/websocketpp
)


target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${Python3_INCLUDE_DIRS}
        ${Python3_NumPy_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        ${TORCH_LIBRARIES}
        CURL::libcurl
        jsoncpp_lib
        ${HIREDIS_LIB}
        ${SQLITE3_LIB}
        ${Python3_LIBRARIES}
        websocketpp
        Threads::Threads
        OpenSSL::SSL
        OpenSSL::Crypto
        ${Boost_LIBRARIES}
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

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)