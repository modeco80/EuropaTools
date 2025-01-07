#
# EuropaTools
#
# (C) 2021-2025 modeco80 <lily.modeco80@protonmail.ch>
#
# SPDX-License-Identifier: MIT
#

add_subdirectory(eupak)

# Most of these utilities are being merged into eupak.
add_executable(texdump texdump.cpp)
target_link_libraries(texdump PUBLIC
		europa
		)

add_executable(jsfscramble jsfscramble.cpp)
target_link_libraries(jsfscramble PUBLIC
		europa
		)

#add_executable(paktest paktest.cpp)
#target_link_libraries(paktest PUBLIC
#		europa
#		)
