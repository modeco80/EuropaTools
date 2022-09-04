 

add_library(libeuropa
		io/PakReader.cpp
		)

target_include_directories(libeuropa PUBLIC ${PROJECT_SOURCE_DIR}/include)

set_target_properties(libeuropa PROPERTIES
		CXX_STANDARD 20
		CXX_STANDARD_REQUIRED ON
		)
