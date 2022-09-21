#
# EuropaTools
#
# (C) 2021-2022 modeco80 <lily.modeco80@protonmail.ch>
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

add_library(libeuropa
		# Implementation details
		io/StreamUtils.cpp

		# Pak IO
		io/PakFile.cpp
		io/PakReader.cpp
		io/PakWriter.cpp

		# Yatf IO
		io/YatfReader.cpp
		)

target_include_directories(libeuropa PUBLIC ${PROJECT_SOURCE_DIR}/include)

target_compile_features(libeuropa PUBLIC cxx_std_20)

# Projects which libeuropa depends on
target_link_libraries(libeuropa PUBLIC
		pixel::libpixel
		)
