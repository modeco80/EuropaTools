#
# EuropaTools
#
# (C) 2021-2022 modeco80 <lily.modeco80@protonmail.ch>
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

add_executable(europa_pack_extractor europa_pack_extractor.cpp)

target_link_libraries(europa_pack_extractor PUBLIC
		europa
		indicators::indicators
		)

add_executable(pakcreate pakcreate.cpp)

target_link_libraries(pakcreate PUBLIC
		europa
		indicators::indicators
		)

add_executable(texdump texdump.cpp)

target_link_libraries(texdump PUBLIC
		europa
		)

add_executable(paktest paktest.cpp)

target_link_libraries(paktest PUBLIC
		europa
		)
