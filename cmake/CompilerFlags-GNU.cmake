# TODO: This currently assumes libstdc++, later on we should *probably* set this with some detection
# also TODO: Use a list so that this isn't one giant line (list JOIN should help.)
set(EUROPA_CORE_COMPILE_ARGS "-Wall -Wextra -Wformat=2 -Wimplicit-fallthrough")
set(EUROPA_CORE_LINKER_ARGS "-fuse-ld=${EUROPA_LINKER}")

if("${CMAKE_BUILD_TYPE}" STREQUAL "Release") # OR "${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo"
	# If on Release use link-time optimizations.
	# On clang we use ThinLTO for even better build performance.
	if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
		set(EUROPA_CORE_COMPILE_ARGS "${EUROPA_CORE_COMPILE_ARGS} -flto=thin -ffunction-sections -fdata-sections")
		set(EUROPA_CORE_LINKER_ARGS "${EUROPA_CORE_LINKER_ARGS} -flto=thin -ffunction-sections -fdata-sections")
	else()
		set(EUROPA_CORE_COMPILE_ARGS "${EUROPA_CORE_COMPILE_ARGS} -flto -ffunction-sections -fdata-sections")
		set(EUROPA_CORE_LINKER_ARGS "${EUROPA_CORE_LINKER_ARGS} -flto -ffunction-sections -fdata-sections")
	endif()
endif()

set(_EUROPA_CORE_WANTED_SANITIZERS "")

if("asan" IN_LIST EUROPA_SANITIZERS)
	# Error if someone's trying to mix asan and tsan together since they aren't compatible.
	if("tsan" IN_LIST EUROPA_SANITIZERS)
		message(FATAL_ERROR "ASAN and TSAN cannot be used together.")
	endif()

	message(STATUS "Enabling ASAN because it was in EUROPA_SANITIZERS")
	list(APPEND _EUROPA_CORE_WANTED_SANITIZERS "address")
endif()

if("tsan" IN_LIST EUROPA_SANITIZERS)
	if("asan" IN_LIST EUROPA_SANITIZERS)
		message(FATAL_ERROR "ASAN and TSAN cannot be used together.")
	endif()

	message(STATUS "Enabling TSAN because it was in EUROPA_SANITIZERS")
	list(APPEND _EUROPA_CORE_WANTED_SANITIZERS "thread")
endif()

if("ubsan" IN_LIST EUROPA_SANITIZERS)
	message(STATUS "Enabling UBSAN because it was in EUROPA_SANITIZERS")
	list(APPEND _EUROPA_CORE_WANTED_SANITIZERS "undefined")
endif()

list(LENGTH _EUROPA_CORE_WANTED_SANITIZERS _EUROPA_CORE_WANTED_SANITIZERS_LENGTH)
if(NOT _EUROPA_CORE_WANTED_SANITIZERS_LENGTH  EQUAL 0)
	list(JOIN _EUROPA_CORE_WANTED_SANITIZERS "," _EUROPA_CORE_WANTED_SANITIZERS_ARG)
	message(STATUS "Enabled sanitizers: ${_EUROPA_CORE_WANTED_SANITIZERS_ARG}")
	set(EUROPA_CORE_COMPILE_ARGS "${EUROPA_CORE_COMPILE_ARGS} -fsanitize=${_EUROPA_CORE_WANTED_SANITIZERS_ARG}")
	set(EUROPA_CORE_LINKER_ARGS "${EUROPA_CORE_LINKER_ARGS} -fsanitize=${_EUROPA_CORE_WANTED_SANITIZERS_ARG}")
endif()

# Set core CMake toolchain variables so that they get applied to all projects.
# A bit nasty, but /shrug, this way our third party libraries can be mostly sanitized/etc as well.
# We do NOT do this for CMake compiler features however.

set(CMAKE_C_FLAGS "${EUROPA_CORE_COMPILE_ARGS}")
set(CMAKE_CXX_FLAGS "${EUROPA_CORE_COMPILE_ARGS}")

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS} -O0 -g3")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS} -O3 -g3")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS} -DNDEBUG -O3 -s")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -O0 -g3")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS} -O3 -g3")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -DNDEBUG -O3 -s")

set(CMAKE_EXE_LINKER_FLAGS "${EUROPA_CORE_LINKER_ARGS} -Wl,-z,noexecstack,-z,relro,-z,now,--gc-sections")
