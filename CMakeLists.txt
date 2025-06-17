cmake_minimum_required(VERSION 3.29)

project(CdnRegulationGame LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

message(STATUS "Default RUNTIME_OUTPUT_DIRECTORY")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

find_package(Threads REQUIRED)

add_executable(CdnRegulationGame
        src/main.cpp
        src/enums/tax_mode.h
        src/headers/cdn.fwd.h
        src/headers/region.fwd.h
        src/headers/service_provider.fwd.h
        src/lib/config.h
        src/lib/config.cpp
        src/lib/argument_parser.h
        src/lib/argument_parser.cpp
        src/lib/logger.cpp
        src/lib/logger.h
        src/lib/file_operator.cpp
        src/lib/file_operator.h
        src/models/cdn.h
        src/models/cdn.cpp
        src/models/service_provider.h
        src/models/service_provider.cpp
        src/models/region.h
        src/models/region.cpp
        src/models/sub_game.h
        src/models/sub_game.cpp
        src/models/best_response.cpp
        src/models/best_response.h
        src/tools/time.h
        src/tools/types.h
        src/tools/vector.h
        src/tools/string.h
        src/lib/manager.cpp
        src/lib/manager.h
)

target_link_libraries(CdnRegulationGame PRIVATE Threads::Threads)

set_target_properties(CdnRegulationGame PROPERTIES
        OUTPUT_NAME "game$<$<CONFIG:Debug>:d>"
)