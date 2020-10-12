CMAKE_MINIMUM_REQUIRED(VERSION 3.10 FATAL_ERROR)

SET(CMAKE_C_STANDARD 11)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

SET(CMAKE_INSTALL_MESSAGE NEVER)

# Help define kern_ver.h
SET(FERAL_VERSION_MAJOR 0)
SET(FERAL_VERSION_MINOR 0)
SET(FERAL_VERSION_PATCH 1)

CONFIGURE_FILE(
	"${CMAKE_SOURCE_DIR}/kern/inc/kern_ver.h.in"
	"${CMAKE_BINARY_DIR}/kern_ver.h"
	@ONLY)


# Assume x86_64 and not EFI for now.
SET(FERAL_ARCH "x86_64")
SET(CMAKE_LINKER "ld.lld")
SET(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/arch/x86_64/linkin.lld)
SET(CMAKE_ASM_NASM_OBJECT_FORMAT "elf64")

# We use clang to be easy to get started with development.
# Gcc has a somewhat lengthy cross-compile process.
IF (CMAKE_C_COMPILER STREQUAL "clang")
# Well aware this is frowned upon.
# But this is the easy way to force it to work right...
SET(BASE_COMPILER_FLAGS "-ffreestanding -nostdlib -mno-red-zone -c -mno-mmx -mno-sse -msoft-float -fno-vectorize -fstack-protector-all -fPIE -fPIC")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${BASE_COMPILER_FLAGS} -Wall -Wextra -O2 -Wstrict-prototypes -Wvla -Wmissing-prototypes -mcmodel=kernel")
SET(CMAKE_C_KERN_FLAGS "${CMAKE_C_FLAGS} ${BASE_COMPILER_FLAGS} -Wall -Wextra -O2 -Wstrict-prototypes -Wvla -Wmissing-prototypes -mcmodel=kernel")
SET(CMAKE_C_USER_FLAGS "-Wall -Wextra -O2 -Wstrict-prototypes -Wvla -Wmissing-prototypes -g3 -mno-red-zone -c -mno-mmx -mno-sse -msoft-float -fno-vectorize -fstack-protector-all -fPIE")
SET(CMAKE_C_LINK_EXECUTABLE "<CMAKE_LINKER> -T ${LINKER_SCRIPT} <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")
ELSE()
MESSAGE(FATAL_ERROR "Unsupported compiler. Use clang. Got ${CMAKE_C_COMPILER}")
ENDIF()

PROJECT(FERALKER C ASM_NASM)
INCLUDE_DIRECTORIES(inc)
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR})


# Maybe this will help us organize private headers better...
SET(CMAKE_INCLUDE_CURRENT_DIR ON)

# Don't even try with host stuff.
SET(CMAKE_SHARED_LIBRARY_PREFIX "")
SET(CMAKE_SHARED_MODULE_PREFIX "")

# Let's wait a while for fixing EFI support...
OPTION(FERAL_BUILD_STANDALONE_UEFI_ "Enable UEFI build" OFF)

ADD_SUBDIRECTORY(drivers)
ADD_SUBDIRECTORY(kern)
ADD_SUBDIRECTORY(sec)
ADD_SUBDIRECTORY(tests)

ADD_EXECUTABLE(FERALKER
	$<TARGET_OBJECTS:Security>)
	
TARGET_LINK_LIBRARIES(FERALKER Drivers)
TARGET_LINK_LIBRARIES(FERALKER Kernel)
	
SET_TARGET_PROPERTIES(FERALKER PROPERTIES LINK_DEPENDS ${LINKER_SCRIPT})


# We should possibly be more careful what we strip instead of just
# stripping everything.
IF (CMAKE_BUILD_TYPE STREQUAL Release)
    ADD_CUSTOM_COMMAND(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_STRIP} ${CMAKE_PROJECT_NAME})
ENDIF()
