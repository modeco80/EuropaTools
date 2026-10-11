#
# EuropaTools
#
# (C) 2021-2026 modeco80 <lily.modeco80@protonmail.ch>
#
# SPDX-License-Identifier: MIT
#

# HACK!!!! Latest HEAD breaks daw_json_link!!!!
FetchContent_Declare(
        daw_header_libraries
        GIT_REPOSITORY https://github.com/beached/header_libraries.git
        GIT_TAG v2.136.0
)

FetchContent_Declare(daw_json_link
        GIT_REPOSITORY https://github.com/beached/daw_json_link.git
		# latest released version as of 10/11/26
		GIT_TAG v3.38.1
		GIT_PROGRESS ON
        )

FetchContent_MakeAvailable(daw_header_libraries daw_json_link)

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
