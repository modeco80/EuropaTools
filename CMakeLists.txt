#
# EuropaTools
#
# (C) 2021-2026 modeco80 <lily.modeco80@protonmail.ch>
#
# SPDX-License-Identifier: MIT
#

cmake_minimum_required(VERSION 3.10)

# Prohibit in-source tree builds.
if(" ${CMAKE_SOURCE_DIR}" STREQUAL " ${CMAKE_BINARY_DIR}")
    message(FATAL_ERROR "In-source builds are strictly prohibited.")
endif()

# Later on I think it might be a good idea to do clang-cl, but mingw's fine too.
if(WIN32 AND MSVC)
        message(FATAL_ERROR "Windows is only supported with the MinGW toolchain.")
endif()

project(EuropaTools
        VERSION 0.8.0
        LANGUAGES C CXX
        DESCRIPTION "Tools for working with LEC Europa based games (Star Wars: Starfighter & Star Wars: Jedi Starfighter)"
        )

include(FetchContent)
include(GNUInstallDirs)

# declare dependencies
FetchContent_Declare(indicators
        GIT_REPOSITORY https://github.com/p-ranav/indicators.git
        )

FetchContent_Declare(argparse
        GIT_REPOSITORY https://github.com/p-ranav/argparse.git
        )

FetchContent_MakeAvailable(indicators argparse)

add_subdirectory(third_party)

add_subdirectory(src/libeuropa)
add_subdirectory(src/tools)
