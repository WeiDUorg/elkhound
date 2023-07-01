#
# gcom1 CMakeLists.txt
#
project(lexer1)
project(parser1)

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

    # all the files for lexer1
    add_executable(lexer1
        lexer.cc
    )

    # all the files for parser1
    add_executable(parser1
        lexer.cc
        parser.cc
        gcom.cc
    )

    # extra compile options
    target_compile_options(lexer1 PRIVATE -DTEST_LEXER)

    # link against elkhound, smbase, libcparse and ast
    target_link_libraries(lexer1 libelkhound smbase ast)
    target_link_libraries(parser1 libelkhound smbase ast)

    if(BUILD_TESTING)
        add_test(
            NAME gcom1-lexer
            COMMAND ${PERL_EXECUTABLE} ${SCRIPTS_DIR}/test-pipe "2 + 3" ./lexer1
        )
        add_test(
            NAME gcom1-parser
            COMMAND ${PERL_EXECUTABLE} ${SCRIPTS_DIR}/test-pipe "2 + 3" ./parser1
        )
    endif(BUILD_TESTING)
else()
    message(WARNING " * Skipping the gcom1 example: Perl not found")
endif()
