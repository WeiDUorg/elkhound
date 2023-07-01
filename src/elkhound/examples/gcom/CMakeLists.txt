#
# gcom CMakeLists.txt
#
project(lexer)
project(parser)

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

    # all the files for lexer
    add_executable(lexer
        lexer.cc
    )

    # all the files for parser
    add_executable(parser
        lexer.cc
        parser.cc
        gcom.cc
        ast.cc
        eval.cc
    )

    # extra compile options
    target_compile_options(lexer PRIVATE -DTEST_LEXER)

    # link against elkhound, smbase, libcparse and ast
    target_link_libraries(lexer libelkhound smbase ast)
    target_link_libraries(parser libelkhound smbase ast)

    if(BUILD_TESTING)
        add_test(
            NAME gcom-lexer
            COMMAND ${PERL_EXECUTABLE} ${SCRIPTS_DIR}/test-pipe "x := 2 + 3 + 4; print x" ./lexer
        )
        add_test(
            NAME gcom-parser1
            COMMAND ${PERL_EXECUTABLE} ${SCRIPTS_DIR}/test-pipe "x := 2 + 3 + 4; print x" ./parser -ast
        )
        add_test(
            NAME gcom-parser2
            COMMAND ${PERL_EXECUTABLE} ${SCRIPTS_DIR}/test-pipe "x := 2 + 3 + 4; print x" ./parser -tree
        )
    endif(BUILD_TESTING)
else()
    message(WARNING " * Skipping the gcom example: Perl not found")
endif()
