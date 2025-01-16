#
# EuropaTools
#
# (C) 2021-2025 modeco80 <lily.modeco80@protonmail.ch>
#
# SPDX-License-Identifier: MIT
#

add_library(lodepng
    lodepng/lodepng.cpp
)

target_include_directories(lodepng PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/lodepng
)