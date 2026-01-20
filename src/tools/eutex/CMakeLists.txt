#
# EuropaTools
#
# (C) 2021-2025 modeco80 <lily.modeco80@protonmail.ch>
#
# SPDX-License-Identifier: MIT
#

add_executable(eutex
		main.cpp
		
		# commands
		DumpCommand.cpp
)

target_link_libraries(eutex PUBLIC
		europa
		toollib
		lodepng
		)

configure_file(EutexConfig.hpp.in
		${CMAKE_CURRENT_BINARY_DIR}/EutexConfig.hpp
)

target_include_directories(eutex PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(eutex PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

mco_target(eutex)

install(TARGETS eutex
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
