#
# gcom7 CMakeLists.txt
#
project(lexer7)
project(parser7)

find_package(Perl)

if(PERL_EXECUTABLE)
    # generate gcom.{cc,h}
    add_custom_command(
        OUTPUT gcom.cc gcom.h
        COMMAND ${ELKHOUND_DIR}/elkhound -o gcom ${CMAKE_CURRENT_SOURCE_DIR}/gcom.gr
        DEPENDS elkhound tokens.tok
    )

    # generate ast.{cc,h}
    add_custom_command(
        OUTPUT ast.cc ast.h
        COMMAND ${AST_DIR}/astgen -o ast ${CMAKE_CURRENT_SOURCE_DIR}/gcom.ast
    )

    # generate tokens.tok
    add_custom_command(
        OUTPUT tokens.tok
        COMMAND ${PERL_EXECUTABLE} ${SCRIPTS_DIR}/make-tok TokenCode < ${CMAKE_CURRENT_SOURCE_DIR}/lexer.h > tokens.tok
    )

    # all the files for lexer7
    add_executable(lexer7
        lexer.cc
    )

    # all the files for parser7
    add_executable(parser7
        lexer.cc
        parser.cc
        gcom.cc
        ast.cc
        eval.cc
    )

    # extra compile options
    target_compile_options(lexer7 PRIVATE -DTEST_LEXER)

    # link against elkhound, smbase, libcparse and ast
    target_link_libraries(lexer7 libelkhound smbase ast)
    target_link_libraries(parser7 libelkhound smbase ast)

    if(BUILD_TESTING)
        add_test(
            NAME gcom7-lexer
            COMMAND ${PERL_EXECUTABLE} ${SCRIPTS_DIR}/test-pipe "x := 2 + 3 + 4; print x" ./lexer7
        )
        add_test(
            NAME gcom7-parser1
            COMMAND ${PERL_EXECUTABLE} ${SCRIPTS_DIR}/test-pipe "x := 2 + 3 + 4; print x" ./parser7 -ast
        )
        add_test(
            NAME gcom7-parser2
            COMMAND ${PERL_EXECUTABLE} ${SCRIPTS_DIR}/test-pipe "x := 2 + 3 + 4; print x" ./parser7 -tree
        )
    endif(BUILD_TESTING)
else()
    message(WARNING " * Skipping the gcom7 example: Perl not found")
endif()
