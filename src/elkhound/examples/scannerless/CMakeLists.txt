#
# scannerless CMakeLists.txt
#
project(sless)

# generate sless.{cc,h}
add_custom_command(
    OUTPUT sless.cc sless.h
    COMMAND ${ELKHOUND_DIR}/elkhound -o sless ${CMAKE_CURRENT_SOURCE_DIR}/sless.gr
    DEPENDS elkhound
)

# all the files for sless
add_executable(sless
    sless.cc
    main.cc
)

# link against elkhound and smbase
target_link_libraries(sless libelkhound smbase)

find_package(Perl)

if(BUILD_TESTING AND PERL_EXECUTABLE)
    add_test(
        NAME sless
        COMMAND ${PERL_EXECUTABLE} ${SCRIPTS_DIR}/test-pipe "if (a) fi (if aa fi) a aa aaa" ./sless
    )
elseif(NOT PERL_EXECUTABLE)
    message(WARNING " * Skipping the sless tests: Perl not found")
endif(BUILD_TESTING AND PERL_EXECUTABLE)
