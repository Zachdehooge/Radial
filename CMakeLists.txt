# =============================================================================
# CMakeLists.txt  --  reusable project template (C/C++)
# Rough CMake equivalent of the xmake.lua template you're used to.
# =============================================================================
# Assumed layout (matches the xmake.lua template; change the globs below if
# yours differs):
#
#   myproject/
#   ├── CMakeLists.txt
#   ├── CMakePresets.json           build-mode presets (debug/release/asan/...)
#   ├── include/myproject/*.hpp     public headers of the "core" library
#   ├── src/core/*.cpp              "core" library sources  (OPTIONAL: skipped if empty)
#   ├── src/app/main.cpp            executable entry point  (or src/main.cpp, or ./main.cpp)
#   └── tests/test_*.cpp            one test executable per file
#
# NEW PROJECT QUICK START
#   1. Make a folder and go into it:        mkdir myproject && cd myproject
#   2. Drop this file (+ CMakePresets.json) in, plus a starter main.cpp.
#   3. Write your code in src/app/main.cpp (or src/main.cpp, or main.cpp in root).
#   4. List any dependencies you need in section 3 below (see DEPENDENCIES).
#      Unlike xmake, CMake can't auto-detect them from your #includes -- you
#      declare them once, by name + git tag/version, and FetchContent handles
#      the download/build/link. (Nothing to run twice -- one configure does it.)
#   5. Configure + build:
#        cmake --preset debug
#        cmake --build --preset debug
#      (or just: cmake -B build && cmake --build build)
#   6. Run:              ./build/Radial        (Windows: build\Radial.exe)
#   7. Tests (optional): put tests/test_*.cpp files in the folder, then
#        ctest --preset debug
#      (Catch2 is set up for you; use #include <catch2/catch_test_macros.hpp>)
#
# QUICK COMMAND REFERENCE                              xmake equivalent
#   cmake --preset debug                                xmake f -m debug
#   cmake --preset release                               xmake f -m release
#   cmake --build --preset debug                         xmake
#   cmake --build --preset debug --target Radial      xmake build Radial
#   ./build/debug/Radial                               xmake run
#   ctest --preset debug                                 xmake test
#   ctest --preset debug -V                               xmake test -v
#   cmake --build --preset debug --target clean          xmake clean
#   rm -rf build                                          xmake f -c
#   cmake -B build -DCMAKE_TOOLCHAIN_FILE=...             xmake f --toolchain=...
#   cmake -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON     xmake project -k compile_commands
#     (this template turns that ON by default and copies it to the project
#      root automatically, same as xmake's plugin.compile_commands.autoupdate)
# =============================================================================

cmake_minimum_required(VERSION 3.21)

# -----------------------------------------------------------------------------
# 1. PROJECT SETTINGS
# -----------------------------------------------------------------------------
project(Radial VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Default to Debug if nothing was specified (single-config generators: Make, Ninja).
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release RelWithDebInfo MinSizeRel)
endif()

# compile_commands.json for clangd / IDEs, auto-copied to the project root
# (equivalent of xmake's plugin.compile_commands.autoupdate).
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
if(CMAKE_EXPORT_COMPILE_COMMANDS)
    add_custom_target(copy-compile-commands ALL
            ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_BINARY_DIR}/compile_commands.json
            ${CMAKE_SOURCE_DIR}/compile_commands.json
    )
endif()

# -----------------------------------------------------------------------------
# 2. BUILD OPTIONS  (user-facing switches, like xmake's option())
# -----------------------------------------------------------------------------
option(BUILD_TESTS "Build unit tests" ON)
option(WARNINGS_AS_ERRORS "Treat warnings as errors" OFF)
option(ENABLE_ASAN "Build with AddressSanitizer" OFF)
option(ENABLE_LTO "Enable link-time optimization in Release" OFF)

# -----------------------------------------------------------------------------
# 3. DEPENDENCIES  (FetchContent -- closest CMake equivalent to xmake-repo)
# -----------------------------------------------------------------------------
# CMake has no built-in package repo and can't scan your #includes the way
# xmake does. Instead: most libraries' README/docs give you a ready-made
# "Integration via FetchContent" snippet -- just paste it straight in below,
# unmodified. It'll look like:
#
#   FetchContent_Declare(
#       fmt
#       GIT_REPOSITORY https://github.com/fmtlib/fmt.git
#       GIT_TAG        10.2.1)
#   FetchContent_MakeAvailable(fmt)
#
# `include(FetchContent)` below only needs to happen once (already done for
# you). After pasting a block, link it to whatever target needs it in
# section 5, e.g.:  target_link_libraries(Radial PRIVATE fmt::fmt)
# (the exact linked name depends on the library -- its README will say).
#
# Alternative for larger dependency sets: pair CMake with vcpkg or Conan
# instead of FetchContent -- both give you an xmake-repo-like registry:
#   vcpkg:  cmake -B build -DCMAKE_TOOLCHAIN_FILE=$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake
#   conan:  conan install . --output-folder=build --build=missing
include(FetchContent)
set(FETCHCONTENT_QUIET OFF)

# ---- paste dependency snippets here ----
set(RAYLIB_VERSION 6.0)   # check github.com/raysan5/raylib/releases for newer
FetchContent_Declare(
        raylib
        URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
        DOWNLOAD_EXTRACT_TIMESTAMP OFF
        SYSTEM)   # treat raylib headers as system headers, so your -Wconversion etc. don't flag them
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)  # don't build raylib's examples
set(BUILD_GAMES    OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(raylib)

set(RAYGUI_VERSION 5.0)
FetchContent_Declare(
        raygui
        URL https://github.com/raysan5/raygui/archive/refs/tags/${RAYGUI_VERSION}.tar.gz
        DOWNLOAD_EXTRACT_TIMESTAMP OFF
        SYSTEM)
# raygui is a single header with no top-level CMakeLists.txt, so MakeAvailable
# only downloads it -- we have to define the target ourselves.
FetchContent_MakeAvailable(raygui)
add_library(raygui INTERFACE)
target_include_directories(raygui SYSTEM INTERFACE ${raygui_SOURCE_DIR}/src)
target_link_libraries(raygui INTERFACE raylib)

# Test framework (Catch2 v3), only pulled in when tests are enabled.
# (This one's already wired up for you -- leave as-is.)
if(BUILD_TESTS)
    FetchContent_Declare(
            Catch2
            GIT_REPOSITORY https://github.com/catchorg/Catch2.git
            GIT_TAG        v3.6.0)
    FetchContent_MakeAvailable(Catch2)
    list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
endif()

# -----------------------------------------------------------------------------
# 4. GLOBAL FLAGS  (apply to every target defined below)
# -----------------------------------------------------------------------------
add_library(project_warnings INTERFACE)

if(MSVC)
    target_compile_options(project_warnings INTERFACE /W4 /utf-8 /permissive-)
    if(WARNINGS_AS_ERRORS)
        target_compile_options(project_warnings INTERFACE /WX)
    endif()
else()
    target_compile_options(project_warnings INTERFACE -Wall -Wextra -Wshadow -Wconversion)
    if(WARNINGS_AS_ERRORS)
        target_compile_options(project_warnings INTERFACE -Werror)
    endif()
endif()

add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_20)

if(WIN32)
    target_compile_definitions(project_options INTERFACE
            NOMINMAX WIN32_LEAN_AND_MEAN _CRT_SECURE_NO_WARNINGS)
endif()

if(ENABLE_ASAN)
    target_compile_options(project_options INTERFACE -fsanitize=address -fno-omit-frame-pointer)
    target_link_options(project_options INTERFACE -fsanitize=address)
endif()

if(ENABLE_LTO AND CMAKE_BUILD_TYPE STREQUAL "Release")
    include(CheckIPOSupported)
    check_ipo_supported(RESULT ipo_ok)
    if(ipo_ok)
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)
    endif()
endif()

# -----------------------------------------------------------------------------
# 5. TARGETS
# -----------------------------------------------------------------------------
# ---- 5a. Library: every source/header anywhere under src/, except src/app/ --
# Add any new folder (src/core/..., src/whatever/...) and its files are picked
# up automatically -- just re-run the build. (Skipped when there's nothing.)
file(GLOB_RECURSE CORE_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_SOURCE_DIR}/src/*.cpp"
        "${CMAKE_SOURCE_DIR}/src/*.c")
list(FILTER CORE_SOURCES EXCLUDE REGEX "^${CMAKE_SOURCE_DIR}/src/app/")

file(GLOB_RECURSE CORE_HEADERS CONFIGURE_DEPENDS
        "${CMAKE_SOURCE_DIR}/src/*.h"
        "${CMAKE_SOURCE_DIR}/src/*.hpp")
list(FILTER CORE_HEADERS EXCLUDE REGEX "^${CMAKE_SOURCE_DIR}/src/app/")

if(CORE_SOURCES)
    add_library(core STATIC ${CORE_SOURCES} ${CORE_HEADERS})
    target_link_libraries(core PUBLIC project_options PRIVATE project_warnings)
    target_link_libraries(core PUBLIC raylib raygui)

    # PUBLIC so the app can #include "core/gui/headers/window.h"
    # (or the shorter "gui/headers/window.h")
    target_include_directories(core PUBLIC
            ${CMAKE_SOURCE_DIR}/src
            ${CMAKE_SOURCE_DIR}/src/core)

    if(EXISTS "${CMAKE_SOURCE_DIR}/include")
        target_include_directories(core PUBLIC
                $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
                $<INSTALL_INTERFACE:include>)
    endif()

    install(TARGETS core)
endif()

# ---- 5b. Executable ---------------------------------------------------------
# Source lookup order (first one that has files wins), same as the xmake template:
#   1. src/app/**.cpp   2. src/*.cpp   3. *.cpp in the project root
file(GLOB_RECURSE APP_SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/app/*.cpp")
if(NOT APP_SOURCES)
    file(GLOB APP_SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/*.cpp")
endif()
if(NOT APP_SOURCES)
    file(GLOB APP_SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/*.cpp")
endif()
# (C project? swap "*.cpp" for "*.c" in the three globs above.)

# Named after project() in section 1 -- rename the project, the exe follows.
add_executable(${PROJECT_NAME} ${APP_SOURCES})
target_link_libraries(${PROJECT_NAME} PRIVATE project_options project_warnings)

if(CORE_SOURCES)
    target_link_libraries(${PROJECT_NAME} PRIVATE core)
elseif(EXISTS "${CMAKE_SOURCE_DIR}/include")
    target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)
endif()

# link dependencies declared in section 3 that only the app needs, e.g.:
# target_link_libraries(${PROJECT_NAME} PRIVATE fmt::fmt)

install(TARGETS ${PROJECT_NAME})

# ---- 5c. Extra executables (tools, examples, benchmarks) -------------------
# Same pattern; EXCLUDE_FROM_ALL means they only build when asked for by name:
#   cmake --build build --target example_hello
# file(GLOB EXAMPLE_SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/examples/*.cpp")
# foreach(example_src ${EXAMPLE_SOURCES})
#     get_filename_component(example_name ${example_src} NAME_WE)
#     add_executable(example_${example_name} EXCLUDE_FROM_ALL ${example_src})
#     target_link_libraries(example_${example_name} PRIVATE core project_options)
# endforeach()

# -----------------------------------------------------------------------------
# 6. TESTS  (run with `ctest`)
# -----------------------------------------------------------------------------
# Each tests/test_*.cpp becomes its own executable registered as a CTest test.
# Catch2 v3 test file example (no main() needed, Catch2::Catch2WithMain provides it):
#     #include <catch2/catch_test_macros.hpp>
#     #include <myproject/foo.hpp>
#
#     TEST_CASE("foo adds numbers", "[foo]") {
#         REQUIRE(foo::add(1, 2) == 3);
#     }
if(BUILD_TESTS)
    enable_testing()
    include(CTest)
    include(Catch)   # provides catch_discover_tests(); comes from Catch2's extras/ dir

    file(GLOB TEST_SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/tests/test_*.cpp")
    foreach(test_src ${TEST_SOURCES})
        get_filename_component(test_name ${test_src} NAME_WE)
        add_executable(${test_name} ${test_src})
        target_link_libraries(${test_name} PRIVATE
                Catch2::Catch2WithMain project_options project_warnings)
        if(CORE_SOURCES)
            target_link_libraries(${test_name} PRIVATE core)
        endif()
        catch_discover_tests(${test_name})
    endforeach()
endif()

# -----------------------------------------------------------------------------
# 7. SUBDIRECTORIES / SPLITTING THE BUILD
# -----------------------------------------------------------------------------
# Pull in other CMakeLists.txt files (each may define its own targets):
#   add_subdirectory(libs/somelib)
#
# Rules of thumb:
#   * Options/settings above (project(), standards, FetchContent deps) are
#     visible to everything added below/after via add_subdirectory().
#   * Paths inside an added subdirectory are relative to that subdirectory.

# -----------------------------------------------------------------------------
# 8. CONFIG HEADERS  (optional)
# -----------------------------------------------------------------------------
# Generate a header from a template (config.h.in) with version/defines substituted.
# Template syntax inside config.h.in:  #cmakedefine FEATURE_X   @PROJECT_VERSION@
#
# configure_file(config.h.in ${CMAKE_BINARY_DIR}/generated/config.h)
# target_include_directories(core PUBLIC ${CMAKE_BINARY_DIR}/generated)

# -----------------------------------------------------------------------------
# 9. MISC TIPS
# -----------------------------------------------------------------------------
# * Generate other project files:
#     cmake -G "Visual Studio 17 2022" -B build-vs      Visual Studio solution
#     cmake -G Xcode -B build-xcode                     Xcode project
#     cmake -G Ninja -B build-ninja                     Ninja build files
# * Package a release:    cpack --config build/CPackConfig.cmake  (needs `include(CPack)`)
# * Cross compiling:      cmake -B build --toolchain path/to/toolchain.cmake
# * Verbose build:        cmake --build build --verbose
# * Docs: https://cmake.org/cmake/help/latest/