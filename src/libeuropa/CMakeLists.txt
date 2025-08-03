#
# EuropaTools
#
# (C) 2021-2025 modeco80 <lily.modeco80@protonmail.ch>
#
# SPDX-License-Identifier: MIT
#


add_library(europa
		base/MoveOnlyAny.cpp
		
		# VFS
		base/VirtualFileSystem.cpp

		# Host VFS
		base/HostVirtualFileSystem.posix.cpp

		# Utility
		util/ImageSurface.cpp

		# Implementation details
		io/StreamUtils.cpp

		# Pak IO
		io/pak/Reader.cpp
		io/pak/Writer.cpp

		# Yatf IO
		io/yatf/Reader.cpp
		)

target_include_directories(europa PUBLIC ${PROJECT_SOURCE_DIR}/include)

europa_target(europa)