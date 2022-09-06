add_executable(europa_pack_extractor europa_pack_extractor.cpp)

target_link_libraries(europa_pack_extractor PUBLIC libeuropa)

set_target_properties(europa_pack_extractor PROPERTIES
		CXX_STANDARD 20
		CXX_STANDARD_REQUIRED ON
		)


add_executable(texdump texdump.cpp)

target_link_libraries(texdump PUBLIC libeuropa)

set_target_properties(texdump PROPERTIES
		CXX_STANDARD 20
		CXX_STANDARD_REQUIRED ON
		)

add_executable(paktest paktest.cpp)

target_link_libraries(paktest PUBLIC libeuropa)

set_target_properties(paktest PROPERTIES
		CXX_STANDARD 20
		CXX_STANDARD_REQUIRED ON
		)
