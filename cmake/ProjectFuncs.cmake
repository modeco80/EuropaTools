#
# EuropaTools
#
# (C) 2021-2025 modeco80 <lily.modeco80@protonmail.ch>
#
# SPDX-License-Identifier: MIT
#

function(europa_target target)
        # Set binary products to output in the build directory for easier access
        set_target_properties(
                ${target} PROPERTIES
                RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
        )

        # Require C++20
        target_compile_features(${target} PUBLIC cxx_std_20)
endfunction()

function(europa_set_alternate_linker)
        find_program(LINKER_EXECUTABLE ld.${EUROPA_LINKER} ${EUROPA_LINKER})
        if(LINKER_EXECUTABLE)
                message(STATUS "Using ${EUROPA_LINKER} as argument to -fuse-ld=")
        else()
                message(FATAL_ERROR "Linker ${EUROPA_LINKER} does not exist on your system. Please specify one which does or omit this option from your configure command.")
        endif()
endfunction()

# FIXME: Better MSVC detection
if(NOT WIN32)
    # set the default linker based on compiler id, if one is not provided
    # This is provided so that it can be overridden
    if(NOT EUROPA_LINKER AND "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
            set(EUROPA_LINKER "lld")
    elseif(NOT EUROPA_LINKER)
            set(EUROPA_LINKER "bfd")
    endif()

    europa_set_alternate_linker()
endif()
