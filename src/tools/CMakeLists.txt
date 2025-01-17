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

add_executable(texdump texdump.cpp)
target_link_libraries(texdump PUBLIC
		europa
		lodepng
		)
europa_target(texdump)


add_executable(jsfscramble jsfscramble.cpp)
target_link_libraries(jsfscramble PUBLIC
		europa
		)
europa_target(jsfscramble)


# Temporary test target.
add_executable(toollib_test 
	toollib_test.cpp
	toollib_test_cmd.cpp
)

target_link_libraries(toollib_test PUBLIC
		europa
		toollib
)

europa_target(toollib_test)

