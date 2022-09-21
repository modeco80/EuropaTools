#
# EuropaTools
#
# (C) 2021-2022 modeco80 <lily.modeco80@protonmail.ch>
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

cmake_minimum_required(VERSION 3.10)

# Prohibit in-source tree builds.
if(" ${CMAKE_SOURCE_DIR}" STREQUAL " ${CMAKE_BINARY_DIR}")
    message(FATAL_ERROR "In-source builds are strictly prohibited.")
endif()

include(cmake/Policies.cmake)

project(EuropaTools
        VERSION 0.0.1 # Placeholder for sem-ver usage. Replace with real value later.
        LANGUAGES C CXX
        DESCRIPTION "Tools for working with LEC Europa based games (Star Wars: Starfighter & Star Wars: Jedi Starfighter)"
        )

include(FetchContent)

# declare dependencies
FetchContent_Declare(indicators
        GIT_REPOSITORY https://github.com/p-ranav/indicators.git
        )

FetchContent_Declare(argparse
        GIT_REPOSITORY https://github.com/p-ranav/argparse.git
        )

# Future (maybe when libpixel has provisioning for installs :v)
#FetchContent_Declare(libpixel
#        GIT_REPOSITORY https://github.com/modeco80/libpixel.git
#        )

FetchContent_MakeAvailable(indicators argparse)

add_subdirectory(third_party/libpixel)

add_subdirectory(src/libeuropa)
add_subdirectory(src/tools)
