#
# EuropaTools
#
# (C) 2021-2025 modeco80 <lily.modeco80@protonmail.ch>
#
# SPDX-License-Identifier: MIT
#

FetchContent_Declare(daw_json_link
        GIT_REPOSITORY https://github.com/beached/daw_json_link.git
		# latest released version as of 8/4/25
		GIT_TAG v3.30.2
		GIT_PROGRESS ON
        )

FetchContent_MakeAvailable(daw_json_link)

add_executable(eupak
		main.cpp
		Utils.cpp

		# eupak commands
		CreateCommand.cpp
		ListCommand.cpp
		ExtractCommand.cpp
)

target_link_libraries(eupak PUBLIC
		europa
		toollib
		indicators::indicators
		daw::json_link
		)

configure_file(EupakConfig.hpp.in
		${CMAKE_CURRENT_BINARY_DIR}/EupakConfig.hpp
)

target_include_directories(eupak PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(eupak PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

mco_target(eupak)

install(TARGETS eupak
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
