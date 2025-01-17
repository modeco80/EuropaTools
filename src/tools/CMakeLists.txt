#
# EuropaTools
#
# (C) 2021-2025 modeco80 <lily.modeco80@protonmail.ch>
#
# SPDX-License-Identifier: MIT
#

# Toollib
add_subdirectory(toollib)

add_subdirectory(eupak)
add_subdirectory(eutex)

add_executable(jsfscramble jsfscramble.cpp)
target_link_libraries(jsfscramble PUBLIC
		europa
		)
europa_target(jsfscramble)
