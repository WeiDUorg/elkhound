cmake_minimum_required(VERSION 3.0)
project(elkhound LANGUAGES C CXX)

if(MSVC)
    # Use local FLEX and BISON for MSVC builds
    set(BISON_EXECUTABLE win_bison)
    set(FLEX_EXECUTABLE win_flex --wincompat)
    set(FLEX_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/winflexbison/flex/src")

    add_subdirectory(thirdparty/winflexbison)
endif()

if(BUILD_TESTING)
    enable_testing()
endif()

add_subdirectory(src)
