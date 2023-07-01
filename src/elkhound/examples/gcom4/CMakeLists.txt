#
# gcom4 CMakeLists.txt
#
project(lexer4)
project(parser4)

find_package(Perl)

if(PERL_EXECUTABLE)
    # generate gcom.{cc,h}
    add_custom_command(
        OUTPUT gcom.cc gcom.h
        COMMAND ${ELKHOUND_DIR}/elkhound -o gcom ${CMAKE_CURRENT_SOURCE_DIR}/gcom.gr
        DEPENDS elkhound tokens.tok
    )

    # generate tokens.tok
    add_custom_command(
        OUTPUT tokens.tok
        COMMAND ${PERL_EXECUTABLE} ${SCRIPTS_DIR}/make-tok TokenCode < ${CMAKE_CURRENT_SOURCE_DIR}/lexer.h > tokens.tok
    )

    # all the files for lexer4
    add_executable(lexer4
        lexer.cc
    )

    # all the files for parser4
    add_executable(parser4
        lexer.cc
        parser.cc
        gcom.cc
    )

    # extra compile options
    target_compile_options(lexer4 PRIVATE -DTEST_LEXER)

    # link against elkhound, smbase, libcparse and ast
    target_link_libraries(lexer4 libelkhound smbase ast)
    target_link_libraries(parser4 libelkhound smbase ast)

    if(BUILD_TESTING)
        add_test(
            NAME gcom4-lexer
            COMMAND ${PERL_EXECUTABLE} ${SCRIPTS_DIR}/test-pipe "2 + 3" ./lexer4
        )
        add_test(
            NAME gcom4-parser
            COMMAND ${PERL_EXECUTABLE} ${SCRIPTS_DIR}/test-pipe "2 + 3" ./parser4
        )
    endif(BUILD_TESTING)
else()
    message(WARNING " * Skipping the gcom4 example: Perl not found")
endif()
