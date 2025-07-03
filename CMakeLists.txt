cmake_minimum_required(VERSION 4.0)
project(SIFT LANGUAGES C CXX ASM_NASM)

# Build options
option(BUILD_OLD_VERSION "Build the OLD version of the stresser (uses mainOld.cpp)" OFF)
option(BUILD_CLI "Build CLI version without ImGui (uses mainCLI.cpp)" OFF)
option(BUILD_LIBRARY "Build as static library instead of executable" OFF)

# Set C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native -mtune=native -Wall -Wextra -pthread")

#=============================================================================
# Find Dependencies
#=============================================================================

find_package(PkgConfig REQUIRED)
find_package(Threads REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(ZLIB REQUIRED)

# Find LZMA with fallback to pkg-config
find_package(LibLZMA)
if(NOT LibLZMA_FOUND)
    pkg_check_modules(LZMA liblzma)
    if(LZMA_FOUND)
        set(LibLZMA_FOUND TRUE)
        set(LibLZMA_LIBRARIES ${LZMA_LIBRARIES})
        set(LibLZMA_INCLUDE_DIRS ${LZMA_INCLUDE_DIRS})
    endif()
endif()

if(NOT LibLZMA_FOUND)
    message(WARNING "LZMA library not found. Compression module may not work properly.")
endif()

#=============================================================================
# Source Files
#=============================================================================

# Core sources
file(GLOB MAIN_HEADER_FILE "include/core.hpp")
file(GLOB ASM_SRC "asm/*.asm")
file(GLOB EXTRA_CPP_MODULES "src/*.module.cpp")
file(GLOB EXTRA_CPP_MANAGE_MODULES "src/*.manage.cpp")

# ImGui sources
set(IMGUI_SOURCES
        include/imgui/imgui.cpp
        include/imgui/imgui_draw.cpp
        include/imgui/imgui_tables.cpp
        include/imgui/imgui_widgets.cpp
        include/imgui/backends/imgui_impl_glfw.cpp
        include/imgui/backends/imgui_impl_opengl3.cpp
)

# Combine all common sources
set(COMMON_SOURCES
        ${MAIN_HEADER_FILE}
        ${ASM_SRC}
        ${EXTRA_CPP_MODULES}
)

# ImGui sources (separate so CLI can exclude them)
set(COMMON_SOURCES_WITH_IMGUI
        ${COMMON_SOURCES}
        ${IMGUI_SOURCES}
        ${EXTRA_CPP_MANAGE_MODULES}
)

#=============================================================================
# Helper Function for Common Target Setup
#=============================================================================

function(setup_target_common TARGET_NAME INCLUDE_IMGUI)
    # Set source file languages
    set_property(SOURCE ${ASM_SRC} PROPERTY LANGUAGE ASM_NASM)

    # Include directories
    target_include_directories(${TARGET_NAME} PRIVATE include/)

    # Link required libraries
    target_link_libraries(${TARGET_NAME} PUBLIC
            Threads::Threads
            ZLIB::ZLIB
    )

    # Link ImGui dependencies only if ImGui is included
    if(INCLUDE_IMGUI)
        target_link_libraries(${TARGET_NAME} PUBLIC
                OpenGL::GL
                glfw
        )
    endif()

    # Link LZMA if found
    if(LibLZMA_FOUND)
        if(TARGET LibLZMA::LibLZMA)
            target_link_libraries(${TARGET_NAME} PUBLIC LibLZMA::LibLZMA)
        else()
            target_link_libraries(${TARGET_NAME} PUBLIC ${LibLZMA_LIBRARIES})
            target_include_directories(${TARGET_NAME} PRIVATE ${LibLZMA_INCLUDE_DIRS})
        endif()
        target_compile_definitions(${TARGET_NAME} PRIVATE HAVE_LZMA=1)
    endif()
endfunction()

#=============================================================================
# Build Target Creation
#=============================================================================

if(BUILD_LIBRARY)
    #=========================================================================
    # LIBRARY BUILD
    #=========================================================================

    set(LIBRARY_NAME "sift-cpu")
    message(STATUS "Building library (CPU-only)")

    # Create library target
    add_library(${LIBRARY_NAME} STATIC ${COMMON_SOURCES_WITH_IMGUI})

    # Apply common setup (with ImGui for library)
    setup_target_common(${LIBRARY_NAME} TRUE)

    # Library-specific include directories
    target_include_directories(${LIBRARY_NAME} PUBLIC include/)

    # Set output name
    set_target_properties(${LIBRARY_NAME} PROPERTIES OUTPUT_NAME ${LIBRARY_NAME})

else()
    #=========================================================================
    # EXECUTABLE BUILD
    #=========================================================================

    # Determine executable name and main source
    if(BUILD_OLD_VERSION)
        set(EXECUTABLE_NAME "siftOld")
        set(MAIN_SOURCE_FILE "src/mainOld.cpp")
        set(SOURCES_TO_USE ${COMMON_SOURCES_WITH_IMGUI})
        set(USE_IMGUI TRUE)
        message(STATUS "Building OLD version executable with ImGui")
    elseif(BUILD_CLI)
        set(EXECUTABLE_NAME "siftCLI")
        set(MAIN_SOURCE_FILE "src/mainCLI.cpp")
        set(SOURCES_TO_USE ${COMMON_SOURCES})
        set(USE_IMGUI FALSE)
        message(STATUS "Building CLI version executable (no ImGui)")
    else()
        set(EXECUTABLE_NAME "sift")
        set(MAIN_SOURCE_FILE "src/main.cpp")
        set(SOURCES_TO_USE ${COMMON_SOURCES_WITH_IMGUI})
        set(USE_IMGUI TRUE)
        message(STATUS "Building standard executable with ImGui")
    endif()

    # Create executable target
    add_executable(${EXECUTABLE_NAME}
            ${MAIN_SOURCE_FILE}
            ${SOURCES_TO_USE}
    )

    # Apply common setup
    setup_target_common(${EXECUTABLE_NAME} ${USE_IMGUI})

endif()

#=============================================================================
# Build Configuration Summary
#=============================================================================

message(STATUS "")
message(STATUS "=== SIFT Build Configuration ===")
if(BUILD_LIBRARY)
    message(STATUS "Build type: Library (${LIBRARY_NAME})")
else()
    message(STATUS "Build type: Executable (${EXECUTABLE_NAME})")
endif()
message(STATUS "GPU support: OFF")
message(STATUS "ZLIB found: ${ZLIB_FOUND}")
message(STATUS "LZMA found: ${LibLZMA_FOUND}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "==================================")
message(STATUS "")