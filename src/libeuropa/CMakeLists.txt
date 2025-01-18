#
# EuropaTools
#
# (C) 2021-2025 modeco80 <lily.modeco80@protonmail.ch>
#
# SPDX-License-Identifier: MIT
#

# NOTICE: libeuropa in shared form does not have a stable ABI yet, 
# and more than likely, until a defined semver release
# never will.
#
# There will probably be an option or additional target always
# to build static libeuropa.

add_library(europa SHARED
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

target_compile_features(europa PUBLIC cxx_std_20)

# SOversion for *ix.
set_target_properties(europa PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION 0)