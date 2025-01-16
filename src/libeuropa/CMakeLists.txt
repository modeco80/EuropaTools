#
# EuropaTools
#
# (C) 2021-2022 modeco80 <lily.modeco80@protonmail.ch>
#
# SPDX-License-Identifier: MIT
#

add_library(europa
		# Implementation details
		io/StreamUtils.cpp

		# Pak IO
		io/pak/Reader.cpp
		io/pak/Writer.cpp

		# Yatf IO
		io/YatfReader.cpp
		)

target_include_directories(europa PUBLIC ${PROJECT_SOURCE_DIR}/include)

target_compile_features(europa PUBLIC cxx_std_20)

# Projects which libeuropa depends on
target_link_libraries(europa PUBLIC
		pixel::libpixel
		)
