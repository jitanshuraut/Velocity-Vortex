cmake_minimum_required(VERSION 3.15)

# Set toolchain BEFORE project() command
set(VCPKG_TARGET_TRIPLET "x64-windows" CACHE STRING "")
set(CMAKE_TOOLCHAIN_FILE "C:/Users/lenovo/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")

project(IOBroker
        VERSION 1.0.0
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Output directories - set BEFORE add_executable
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

if(MSVC)
    add_compile_options(
        /W4
        /MP
        /EHsc
        /utf-8
        $<$<CONFIG:Debug>:/Od>
        $<$<CONFIG:Debug>:/RTC1>
        $<$<CONFIG:Debug>:/Zi>
        $<$<CONFIG:Release>:/O2>
        $<$<CONFIG:Release>:/GL>
    )
    add_compile_definitions(
        _WIN32_WINNT=0x0601
        WIN32_LEAN_AND_MEAN
        NOMINMAX
        _CRT_SECURE_NO_WARNINGS
    )
endif()

# ===== Dependencies =====
find_package(hiredis CONFIG REQUIRED)
find_package(unofficial-sqlite3 CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(Crow CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

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

# Core IO-Server sources (subscriber.cpp excluded — it has its own main())
set(ALL_SOURCES
    src/main.cpp
    src/api_router.cpp
    src/SQLiteDB.cpp
    src/Redissubscriber.cpp
    src/calculation_registry.cpp
    src/calculations/rca_handler.cpp
    src/calculations/mca_handler.cpp
)

foreach(GROUP ${SOURCE_GROUPS})
    file(GLOB_RECURSE GROUP_SOURCES
        CONFIGURE_DEPENDS
        "src/${GROUP}/*.cpp"
        "src/${GROUP}/*.hpp"
    )
    list(APPEND ALL_SOURCES ${GROUP_SOURCES})
    source_group(TREE src/${GROUP} PREFIX ${GROUP} FILES ${GROUP_SOURCES})
endforeach()

add_executable(${PROJECT_NAME} ${ALL_SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE include)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        hiredis::hiredis
        unofficial::sqlite3::sqlite3
        spdlog::spdlog
        fmt::fmt
        Crow::Crow
        nlohmann_json::nlohmann_json
)

# ── Copy runtime assets next to the exe so relative paths always work ────────
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/Schema"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/Schema"
    COMMENT "Copying Schema/ to output directory"
)
