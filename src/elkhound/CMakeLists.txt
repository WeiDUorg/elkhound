#
# elkhound CMakeLists.txt
#
# elkhound library
project(libelkhound)
# elkhound executable
project(elkhound)

# generate grampar.tab.{cpp,h}
add_custom_command(
    OUTPUT grampar.tab.cpp grampar.tab.h
    COMMAND ${BISON_EXECUTABLE} -d ${CMAKE_CURRENT_SOURCE_DIR}/grampar.y
    COMMAND ${CMAKE_COMMAND} -E copy grampar.tab.c grampar.tab.cpp
    COMMAND ${CMAKE_COMMAND} -E remove agrampar.tab.c
)

# generate gramlex.yy.cc
add_custom_command(
    OUTPUT gramlex.cc
    COMMAND ${FLEX_EXECUTABLE} -ogramlex.cc ${CMAKE_CURRENT_SOURCE_DIR}/gramlex.lex
)

# generate gramast.ast.gen.{cc,h}
add_custom_command(
    OUTPUT gramast.ast.gen.cc gramast.ast.gen.h
    COMMAND astgen -ogramast.ast.gen ${CMAKE_CURRENT_SOURCE_DIR}/gramast.ast
    DEPENDS astgen
)

# all the files for libelkhound
add_library(libelkhound STATIC
    cyctimer.cc
    glr.cc
    parsetables.cc
    useract.cc
    ptreenode.cc
    ptreeact.cc
)

# all the files for elkhound
add_executable(elkhound
    grampar.tab.cpp
    gramanl.cc
    asockind.cc
    grammar.cc
    emitcode.cc
    mlsstr.cc
    genml.cc
    gramast.ast.gen.cc
    gramlex.cc
    grampar.cc
    gramexpl.cc
    parsetables.cc
)

# extra compile options
target_compile_options(elkhound PRIVATE -DGRAMANL_MAIN)

# link against ast and smbase
target_link_libraries(elkhound smbase ast)
target_link_libraries(libelkhound smbase ast)

# disable lib prefix for libelkhound
SET_TARGET_PROPERTIES(libelkhound PROPERTIES PREFIX "")

include_directories(${FLEX_INCLUDE_DIR})
