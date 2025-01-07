#
# EuropaTools
#
# (C) 2021-2025 modeco80 <lily.modeco80@protonmail.ch>
#
# SPDX-License-Identifier: MIT
#

add_executable(eupak
		main.cpp

		Utils.cpp

		# Tasks
		tasks/InfoTask.cpp
		tasks/CreateTask.cpp
		tasks/ExtractTask.cpp
)

target_link_libraries(eupak PUBLIC
		europa
		argparse::argparse
		indicators::indicators
		)

configure_file(EupakConfig.hpp.in
		${CMAKE_CURRENT_BINARY_DIR}/EupakConfig.hpp
)

target_include_directories(eupak PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(eupak PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})