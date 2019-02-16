#
# Elkhound CMakeLists.txt
#
# Minimum CMAKE version
cmake_minimum_required(VERSION 3.0)

# Global C flags
set(CMAKE_C_FLAGS "-g3 -Wall -D__UNIX__ -fno-strict-aliasing")
# Global C++ flags
set(CMAKE_CXX_FLAGS "-g3 -Wall -fno-strict-aliasing")

# Use debugging optimisation for Debug builds
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Og")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Og")

# Strip & disable debug for Release builds
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -s -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s -DNDEBUG")

# Global include directories
INCLUDE_DIRECTORIES(ast smbase elkhound elkhound/c)
# add current directory & binary directory
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Option(s)
option(EXTRAS "Compile extra stuff(examples, cparse, etc)" ON)
option(OCAML "Run OCaml tests" ON)

# set variables
set(AST_DIR         ${CMAKE_CURRENT_BINARY_DIR}/ast)
set(SMBASE_DIR      ${CMAKE_CURRENT_BINARY_DIR}/smbase)
set(ELKHOUND_DIR    ${CMAKE_CURRENT_BINARY_DIR}/elkhound)
set(CPARSE_DIR      ${CMAKE_CURRENT_BINARY_DIR}/elkhound/c)
set(CC2_DIR         ${CMAKE_CURRENT_BINARY_DIR}/elkhound/cc2)
set(SCRIPTS_DIR     ${CMAKE_CURRENT_SOURCE_DIR}/scripts)

# ast
add_subdirectory(ast)

# smbase
add_subdirectory(smbase)

# elkhound
add_subdirectory(elkhound)

# include testing stuff
include(CTest)

if(EXTRAS)
    # cparse
    add_subdirectory(elkhound/c)

    # C++ parser based on Standard grammar
    add_subdirectory(elkhound/cc2)

    # C++ parser (unfinished)
    add_subdirectory(elkhound/ocaml/occ2)

    # elkhound examples
    add_subdirectory(elkhound/examples)

    if(BUILD_TESTING)
        # cparse tests
        add_subdirectory(elkhound/c.in)

        # elkhound triv tests, requires cparse
        add_subdirectory(elkhound/triv)
    endif(BUILD_TESTING)
endif(EXTRAS)

if(BUILD_TESTING)
    # enable testing
    enable_testing()

    # ast tests
    add_subdirectory(ast/test)

    # smbase tests
    add_subdirectory(smbase/test)

    # elkhound tests
    add_subdirectory(elkhound/test)

    if(OCAML)
        # elkhound ocaml tests
        add_subdirectory(elkhound/ocaml)
    endif(OCAML)
endif(BUILD_TESTING)
