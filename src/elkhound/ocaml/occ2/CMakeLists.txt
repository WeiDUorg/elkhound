#
# occ2 CMakeLists.txt
#
project(occ2)

find_package(Perl)

if(PERL_EXECUTABLE)
    # generate tokens.tok
    add_custom_command(
        OUTPUT tokens.tok
        COMMAND ${PERL_EXECUTABLE} ${SCRIPTS_DIR}/make-tok Token < ${CMAKE_CURRENT_SOURCE_DIR}/tokens.h > tokens.tok
    )

    # create occ2.ml
    add_custom_target(occ2 ALL
        COMMAND ${ELKHOUND_DIR}/elkhound -o occ2 -ocaml ${CMAKE_CURRENT_SOURCE_DIR}/occ2.gr
        DEPENDS elkhound tokens.tok
    )
else()
    message(WARNING " * Skipping the occ2 example: Perl not found")
endif()
