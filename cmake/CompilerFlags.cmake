#
# EuropaTools
#
# (C) 2021-2025 modeco80 <lily.modeco80@protonmail.ch>
#
# SPDX-License-Identifier: MIT
#

# Core compile arguments used for the whole project
#
# This is the driver, we include compiler/platform specific files here

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	include(CompilerFlags-GNU)
else()
	message(FATAL_ERROR "Unsupported (for now?) compiler ${CMAKE_CXX_COMPILER_ID}")
endif()
