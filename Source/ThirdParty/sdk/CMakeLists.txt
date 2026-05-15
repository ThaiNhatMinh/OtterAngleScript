cmake_minimum_required(VERSION 3.10)

project(angelscript)

option(BUILD_SHARED_LIBS "Build shared library" OFF)
option(AS_NO_EXCEPTIONS "Disable exception handling in script context" OFF)
option(AS_DISABLE_INSTALL "Disable installation of AngelScript" OFF)

if(MSVC)
    set(MSVC_COMPILE_FLAGS "/MP" CACHE STRING "Compiler flags to use with MSVC")
endif()

if(APPLE)
    option(BUILD_FRAMEWORK "Build Framework bundle for OSX" OFF)
endif()

file(READ angelscript/include/angelscript.h ANGELSCRIPT_H)
string(REGEX MATCH "#define ANGELSCRIPT_VERSION_STRING \"([0-9]*).([0-9]*).([0-9]*)" ANGELSCRIPT_VERSION_REGEX ${ANGELSCRIPT_H})
set(ANGELSCRIPT_VERSION_MAJOR ${CMAKE_MATCH_1})
set(ANGELSCRIPT_VERSION_MINOR ${CMAKE_MATCH_2})
set(ANGELSCRIPT_VERSION_PATCH ${CMAKE_MATCH_3})
set(PROJECT_VERSION ${ANGELSCRIPT_VERSION_MAJOR}.${ANGELSCRIPT_VERSION_MINOR}.${ANGELSCRIPT_VERSION_PATCH})

if (MSVC)
    set(OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/angelscript/asm)
    SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${OUTPUT_DIRECTORY}")
    SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${OUTPUT_DIRECTORY}")
    SET( CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${OUTPUT_DIRECTORY}")
    SET( CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${OUTPUT_DIRECTORY}")
    SET( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${OUTPUT_DIRECTORY}")
    SET( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${OUTPUT_DIRECTORY}")
else()
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/angelscript)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/angelscript)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/angelscript)
endif()

set(ARCHFLAGS "${CMAKE_SYSTEM_PROCESSOR}")
if (APPLE AND NOT IOS)
  if (${CMAKE_SYSTEM_PROCESSOR} MATCHES "arm")
    set(ARCHFLAGS "aarch64")
  endif ()
endif ()

message(STATUS "Configuring angelscript ${PROJECT_VERSION}")

find_package(Threads)

set(ANGELSCRIPT_SOURCE
    angelscript/source/as_callfunc_mips.cpp
    angelscript/source/as_callfunc_x86.cpp
    angelscript/source/as_callfunc_x64_gcc.cpp
    angelscript/source/as_callfunc_x64_msvc.cpp
    angelscript/source/as_callfunc_x64_mingw.cpp
)

if(MSVC AND CMAKE_CL_64)
    enable_language(ASM_MASM)
    if(CMAKE_ASM_MASM_COMPILER_WORKS)
        set(ANGELSCRIPT_SOURCE ${ANGELSCRIPT_SOURCE} angelscript/source/as_callfunc_x64_msvc_asm.asm)
    else()
        message(FATAL ERROR "MSVC x86_64 target requires a working assembler")
    endif()
endif()

if(${ARCHFLAGS} MATCHES "^arm")
    enable_language(ASM)
    if(CMAKE_ASM_COMPILER_WORKS)
        set(ANGELSCRIPT_SOURCE ${ANGELSCRIPT_SOURCE} angelscript/source/as_callfunc_arm.cpp angelscript/source/as_callfunc_arm_gcc.S)
        set_property(SOURCE angelscript/source/as_callfunc_arm_gcc.S APPEND PROPERTY COMPILE_FLAGS " -Wa,-mimplicit-it=always")
    else()
        message(FATAL ERROR "ARM target requires a working assembler")
    endif()
endif()

if(${ARCHFLAGS} MATCHES "^aarch64")
    enable_language(ASM)
    if(CMAKE_ASM_COMPILER_WORKS)
        if(NOT APPLE)
            set(ANGELSCRIPT_SOURCE ${ANGELSCRIPT_SOURCE} angelscript/source/as_callfunc_arm64.cpp angelscript/source/as_callfunc_arm64_gcc.S)
        else()
            set(ANGELSCRIPT_SOURCE ${ANGELSCRIPT_SOURCE} angelscript/source/as_callfunc_arm64.cpp angelscript/source/as_callfunc_arm64_xcode.S)
        endif()
    else()
        message(FATAL ERROR "ARM target requires a working assembler")
    endif()
endif()

if(${ARCHFLAGS} MATCHES "riscv64")
    enable_language(ASM)
    if(CMAKE_ASM_COMPILER_WORKS)
        set(ANGELSCRIPT_SOURCE ${ANGELSCRIPT_SOURCE} angelscript/source/as_callfunc_riscv64.cpp angelscript/source/as_callfunc_riscv64_gcc.S)
    else()
        message(FATAL ERROR "RISC-V 64 target requires a working assembler")
    endif()
endif()

if(${ARCHFLAGS} MATCHES "^e2k")
    enable_language(ASM)
    if(CMAKE_ASM_COMPILER_WORKS)
        set(ANGELSCRIPT_SOURCE ${ANGELSCRIPT_SOURCE} angelscript/source/as_callfunc_e2k.cpp angelscript/source/as_callfunc_e2k.S)
    else()
        message(FATAL ERROR "E2K target requires a working assembler")
    endif()
endif()

if(MSVC)
    set(CMAKE_DEBUG_POSTFIX "d")
endif()

if(NOT BUILD_FRAMEWORK)
    set(ANGELSCRIPT_LIBRARY_NAME angelscript)
else()
    set(ANGELSCRIPT_LIBRARY_NAME Angelscript) # OS X frameworks should have capitalized name
    set(BUILD_SHARED_LIBS TRUE)
endif()
message("ANGELSCRIPT_LIBRARY_NAME=${ANGELSCRIPT_LIBRARY_NAME}")
set(ANGELSCRIPT_LIBRARY_NAME ${ANGELSCRIPT_LIBRARY_NAME} CACHE STRING "" FORCE)

add_library(${ANGELSCRIPT_LIBRARY_NAME} ${ANGELSCRIPT_SOURCE})

target_include_directories(${ANGELSCRIPT_LIBRARY_NAME} PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/angelscript/include>")

if(MSVC)
    target_compile_definitions(${ANGELSCRIPT_LIBRARY_NAME} PRIVATE -D_CRT_SECURE_NO_WARNINGS)    
endif()

target_compile_definitions(${ANGELSCRIPT_LIBRARY_NAME} PRIVATE -DANGELSCRIPT_EXPORT -D_LIB)

if(AS_NO_EXCEPTIONS)
	target_compile_definitions(${ANGELSCRIPT_LIBRARY_NAME} PRIVATE AS_NO_EXCEPTIONS)
endif()

# Fix x64 issues on Linux
if("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86_64" AND UNIX AND NOT APPLE)
	target_compile_options(${ANGELSCRIPT_LIBRARY_NAME} PRIVATE -fPIC)
endif()

if(MSVC AND MSVC_COMPILE_FLAGS)
	target_compile_options(${ANGELSCRIPT_LIBRARY_NAME} PRIVATE $<$<COMPILE_LANGUAGE:C,CXX>:${MSVC_COMPILE_FLAGS}>)
endif()

# Don't override the default library output path to avoid conflicts when building for multiple target platforms
#set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/angelscript/lib)
target_link_libraries(${ANGELSCRIPT_LIBRARY_NAME} Threads::Threads)

set_target_properties(${ANGELSCRIPT_LIBRARY_NAME} PROPERTIES VERSION ${PROJECT_VERSION})

if(BUILD_FRAMEWORK)
	set_target_properties(${ANGELSCRIPT_LIBRARY_NAME} PROPERTIES
		FRAMEWORK TRUE
		FRAMEWORK_VERSION ${PROJECT_VERSION}
		MACOSX_FRAMEWORK_IDENTIFIER com.angelcode.Angelscript
		MACOSX_FRAMEWORK_SHORT_VERSION_STRING ${PROJECT_VERSION}
		MACOSX_FRAMEWORK_BUNDLE_VERSION ${PROJECT_VERSION}
		XCODE_ATTRIBUTE_INSTALL_PATH "@rpath"
		PUBLIC_HEADER angelscript/include/angelscript.h
	)
endif()


