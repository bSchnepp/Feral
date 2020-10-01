CMAKE_MINIMUM_REQUIRED(VERSION 3.10 FATAL_ERROR)

# Drop support for something older than 99, so
# we can use anonymous structs and unions and all.
SET(CMAKE_C_STANDARD 11)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
SET(CMAKE_LINKER "clang")
SET(CMAKE_C_LINK_EXECUTABLE "<CMAKE_LINKER> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")

INCLUDE_DIRECTORIES(inc)


# Maybe this will help us organize private headers better...
SET(CMAKE_INCLUDE_CURRENT_DIR ON)

# Don't even try with host stuff.
SET(CMAKE_SHARED_LIBRARY_PREFIX "")
SET(CMAKE_SHARED_MODULE_PREFIX "")

SET(CMAKE_C_FLAGS "-Wall -Wextra -O2 -Wstrict-prototypes -Wvla -Wmissing-prototypes -g3 -mno-red-zone -c -mno-mmx -mno-sse -msoft-float -fno-vectorize -fstack-protector-all -fPIE")

ADD_EXECUTABLE(FERALTESTS
	test_main.c
	$<TARGET_OBJECTS:Security>)
	
TARGET_LINK_LIBRARIES(FERALTESTS Drivers)
TARGET_LINK_LIBRARIES(FERALTESTS Kernel)
TARGET_LINK_LIBRARIES(FERALTESTS c)
