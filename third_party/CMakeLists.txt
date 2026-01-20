#
# EuropaTools
#
# (C) 2021-2026 modeco80 <lily.modeco80@protonmail.ch>
#
# SPDX-License-Identifier: MIT
#

# TODO: Don't do this if a project already has mcolib included.
# With an option, it'll presumably tell us if it has or not.
# (Alternatively, a find_package setup should be done to make system mcolib installation possible..)
add_subdirectory(mcolib)

# TODO: Only enable when building tools.
# libeuropa doesn't need this directly.
add_library(lodepng
    lodepng/lodepng.cpp
)

target_include_directories(lodepng PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/lodepng
)
