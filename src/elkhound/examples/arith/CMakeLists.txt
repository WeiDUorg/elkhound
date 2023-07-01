#
# arith CMakeLists.txt
#
project(arith)

# generate arith.gr.gen.{cc,h}
add_custom_command(
    OUTPUT arith.gr.gen.cc arith.gr.gen.h
    COMMAND ${ELKHOUND_DIR}/elkhound -o arith.gr.gen ${CMAKE_CURRENT_SOURCE_DIR}/arith.gr
    DEPENDS elkhound
)

# generate arithyy.cc
add_custom_command(
    OUTPUT arithyy.cc
    COMMAND ${FLEX_EXECUTABLE} -oarithyy.cc ${CMAKE_CURRENT_SOURCE_DIR}/arith.lex
)

# all the files for arith
add_executable(arith
    arith.cc
    arithyy.cc
    arith.gr.gen
)

# suppress -Wsign-compare for Flex-generated code
SET_SOURCE_FILES_PROPERTIES( arithyy.cc PROPERTIES COMPILE_FLAGS -Wno-sign-compare )

# link against elkhound and smbase
target_link_libraries(arith libelkhound smbase)

find_package(Perl)

if(BUILD_TESTING AND PERL_EXECUTABLE)
    add_test(
        NAME arith1
        COMMAND ${PERL_EXECUTABLE} ${SCRIPTS_DIR}/test-pipe "3 + 4 * 5" ./arith
        DEPENDS arith
    )
    add_test(
        NAME arith2
        COMMAND ${PERL_EXECUTABLE} ${SCRIPTS_DIR}/test-pipe "3 + 4 * 5" ./arith printTree
        DEPENDS arith
    )
elseif(NOT PERL_EXECUTABLE)
    message(WARNING " * Skipping the arith1 and arith2 tests: Perl not found")
endif(BUILD_TESTING AND PERL_EXECUTABLE)
