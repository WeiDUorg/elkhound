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
    COMMAND bison -d ${CMAKE_CURRENT_SOURCE_DIR}/grampar.y && mv grampar.tab.c grampar.tab.cpp
)

# generate gramlex.yy.cc
add_custom_command(
    OUTPUT gramlex.cc
    COMMAND flex -ogramlex.cc ${CMAKE_CURRENT_SOURCE_DIR}/gramlex.lex
)

# generate gramast.ast.gen.{cc,h}
add_custom_command(
    OUTPUT gramast.ast.gen.cc gramast.ast.gen.h
    COMMAND ${AST_DIR}/astgen -ogramast.ast.gen ${CMAKE_CURRENT_SOURCE_DIR}/gramast.ast
    DEPENDS astgen
)

# all the files for libelkhound
add_library(libelkhound STATIC
    cyctimer
    glr
    parsetables
    useract
    ptreenode
    ptreeact
)

# all the files for elkhound
add_executable(elkhound
    grampar.tab.cpp
    gramanl.cc
    asockind
    grammar
    emitcode
    mlsstr
    genml
    gramast.ast.gen
    gramlex.cc
    grampar.cc
    gramexpl
    parsetables
)

# extra compile options
target_compile_options(elkhound PRIVATE -DGRAMANL_MAIN)

# link against ast and smbase
target_link_libraries(elkhound smbase ast)
target_link_libraries(libelkhound smbase ast)

# disable lib prefix for libelkhound
SET_TARGET_PROPERTIES(libelkhound PROPERTIES PREFIX "")
