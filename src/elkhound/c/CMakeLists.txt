#
# cparse CMakeLists.txt
#
project(libcparse)
project(cparse)
project(lexer2)

# generate lexer1yy.cc
add_custom_command(
    OUTPUT lexer1yy.cc
    COMMAND ${FLEX_EXECUTABLE} -olexer1yy.cc ${CMAKE_CURRENT_SOURCE_DIR}/lexer1.lex
)

# generate c.ast.gen.{cc,h}
add_custom_command(
    OUTPUT c.ast.gen.cc c.ast.gen.h
    COMMAND ${AST_DIR}/astgen -oc.ast.gen ${CMAKE_CURRENT_SOURCE_DIR}/c.ast
    DEPENDS astgen
)

# generate c.gr.{cc,h}
add_custom_command(
    OUTPUT c.gr.gen.cc c.gr.gen.h
    COMMAND ${ELKHOUND_DIR}/elkhound -tr bison -o c.gr.gen ${CMAKE_CURRENT_SOURCE_DIR}/c.gr
    DEPENDS elkhound c.tok
)

# generate c.tok
add_custom_command(
    OUTPUT c.tok
    COMMAND lexer2 -myparser > c.tok
    DEPENDS lexer2
)

# all the files for libcparse
add_library(libcparse STATIC
    c.ast.gen.cc
    c.gr.gen.cc
    c_env.cc
    c_type.cc
    c_variable.cc
    cc_flags.cc
    cc_lang.cc
    cparse.cc
    exprequal.cc
    exprvisit.cc
    lexer1.cc
    lexer1yy.cc
    lexer2.cc
    parssppt.cc
    paths.cc
    postorder.cc
    stubs.cc
    tcheck.cc
    treeout.cc
)
# all the files for cparse
add_executable(cparse
    main.cc
)

# all the files for lexer2
add_executable(lexer2
    lexer2.cc
    lexer1.cc
    lexer1yy.cc
    cc_lang.cc
)

# extra compile options
target_compile_options(lexer2 PRIVATE -DTEST_LEXER2)
# suppress -Wsign-compare for Flex-generated code
SET_SOURCE_FILES_PROPERTIES( lexer1yy.cc PROPERTIES COMPILE_FLAGS -Wno-sign-compare )

# link options
target_link_libraries(libcparse smbase ast libelkhound)
target_link_libraries(cparse libcparse)
target_link_libraries(lexer2 smbase ast libelkhound)

# disable lib prefix for libelkhound
SET_TARGET_PROPERTIES(libcparse PROPERTIES PREFIX "")
