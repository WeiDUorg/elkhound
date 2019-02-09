#
# gcom5 CMakeLists.txt
#
project(lexer5)
project(parser5)

# generate gcom.{cc,h}
add_custom_command(
    OUTPUT gcom.cc gcom.h
    COMMAND ${ELKHOUND_DIR}/elkhound -o gcom ${CMAKE_CURRENT_SOURCE_DIR}/gcom.gr
    DEPENDS elkhound tokens.tok
)

# generate tokens.tok
add_custom_command(
    OUTPUT tokens.tok
    COMMAND ${SCRIPTS_DIR}/make-tok TokenCode < ${CMAKE_CURRENT_SOURCE_DIR}/lexer.h > tokens.tok
)

# all the files for lexer5
add_executable(lexer5
    lexer.cc
)

# all the files for parser5
add_executable(parser5
    lexer.cc
    parser.cc
    gcom.cc
)

# extra compile options
target_compile_options(lexer5 PRIVATE -DTEST_LEXER)

# link against elkhound, smbase, libcparse and ast
target_link_libraries(lexer5 libelkhound smbase ast)
target_link_libraries(parser5 libelkhound smbase ast)

if(BUILD_TESTING)
    add_test(
        NAME gcom5-lexer
        COMMAND ${SCRIPTS_DIR}/test-pipe "x := 2 + 3 + 4; print x" ./lexer5
    )
    add_test(
        NAME gcom5-parser1
        COMMAND ${SCRIPTS_DIR}/test-pipe "x := 2 + 3 + 4; print x" ./parser5
    )
    add_test(
        NAME gcom5-parser2
        COMMAND ${SCRIPTS_DIR}/test-pipe "x := 2 + 3 + 4; print x" ./parser5 -tree
    )
endif(BUILD_TESTING)
