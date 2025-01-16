#
# EuropaTools
#
# (C) 2021-2025 modeco80 <lily.modeco80@protonmail.ch>
#
# SPDX-License-Identifier: MIT
#

cmake_minimum_required(VERSION 3.10)

# Prohibit in-source tree builds.
if(" ${CMAKE_SOURCE_DIR}" STREQUAL " ${CMAKE_BINARY_DIR}")
    message(FATAL_ERROR "In-source builds are strictly prohibited.")
endif()

project(EuropaTools
        VERSION 1.0.0
        LANGUAGES C CXX
        DESCRIPTION "Tools for working with LEC Europa based games (Star Wars: Starfighter & Star Wars: Jedi Starfighter)"
        )

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

include(Policies)
include(ProjectFuncs)
include(CompilerFlags)

include(FetchContent)

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
