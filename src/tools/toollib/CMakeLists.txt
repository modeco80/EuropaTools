
add_library(toollib
    ToolCommand.cpp
    ToolMain.cpp
)

target_link_libraries(toollib PUBLIC
    argparse::argparse
)

target_include_directories(toollib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

mco_target(toollib)
