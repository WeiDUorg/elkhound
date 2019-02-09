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

if(BUILD_TESTING)
    add_test(
        NAME sless
        COMMAND ${SCRIPTS_DIR}/test-pipe "if (a) fi (if aa fi) a aa aaa" ./sless
    )
endif(BUILD_TESTING)
