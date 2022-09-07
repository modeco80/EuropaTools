 

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

set_target_properties(libeuropa PROPERTIES
		CXX_STANDARD 20
		CXX_STANDARD_REQUIRED ON
		)

# Projects which libeuropa depends on
target_link_libraries(libeuropa PUBLIC
		pixel::libpixel
		)
