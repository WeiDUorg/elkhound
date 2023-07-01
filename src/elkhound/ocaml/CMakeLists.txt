#
# ocaml CMakeLists.txt
#
project(ocaml_main)

# generate ${CMAKE_CURRENT_BINARY_DIR}/lexer.ml
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/lexer.ml
    COMMAND ocamllex ${CMAKE_CURRENT_SOURCE_DIR}/lexer.mll -o lexer.ml
)

# generate ${CMAKE_CURRENT_SOURCE_DIR}/oc_arith.ml{,i}
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/oc_arith.ml ${CMAKE_CURRENT_SOURCE_DIR}/oc_arith.mli
    COMMAND ocamlyacc ${CMAKE_CURRENT_SOURCE_DIR}/oc_arith.mly
)

# generate ${CMAKE_CURRENT_BINARY_DIR}/arith.ml{,i}
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/arith.ml ${CMAKE_CURRENT_BINARY_DIR}/arith.mli
    COMMAND ${ELKHOUND_DIR}/elkhound -ocaml -o arith ${CMAKE_CURRENT_SOURCE_DIR}/arith.gr
    DEPENDS elkhound
)

# generate ${CMAKE_CURRENT_BINARY_DIR}/een.ml{,i}
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/een.ml ${CMAKE_CURRENT_BINARY_DIR}/een.mli
    COMMAND ${ELKHOUND_DIR}/elkhound -ocaml -o een ${CMAKE_CURRENT_SOURCE_DIR}/een.gr
    DEPENDS elkhound
)

# ocaml_main files
# must be in this order!
set(OCAML_MAIN_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/parsetables.ml
    ${CMAKE_CURRENT_SOURCE_DIR}/useract.ml
    ${CMAKE_CURRENT_SOURCE_DIR}/smutil.ml
    ${CMAKE_CURRENT_SOURCE_DIR}/arraystack.ml
    ${CMAKE_CURRENT_SOURCE_DIR}/ptreenode.ml
    ${CMAKE_CURRENT_SOURCE_DIR}/lexerint.ml
    ${CMAKE_CURRENT_SOURCE_DIR}/ptreeact.ml
    
    # this is generated!
    ${CMAKE_CURRENT_BINARY_DIR}/lexer.ml
    
    ${CMAKE_CURRENT_SOURCE_DIR}/lrparse.ml
    ${CMAKE_CURRENT_SOURCE_DIR}/objpool.ml
    
    # this is generated!
    ${CMAKE_CURRENT_SOURCE_DIR}/oc_arith.mli
    ${CMAKE_CURRENT_SOURCE_DIR}/oc_arith.ml

    # this is generated!
    ${CMAKE_CURRENT_BINARY_DIR}/arith.mli
    ${CMAKE_CURRENT_BINARY_DIR}/arith.ml
    
    # this is generated!
    ${CMAKE_CURRENT_BINARY_DIR}/een.mli
    ${CMAKE_CURRENT_BINARY_DIR}/een.ml

    ${CMAKE_CURRENT_SOURCE_DIR}/glr.ml
    ${CMAKE_CURRENT_SOURCE_DIR}/main.ml
)

# ocamlc flags
set(OCAMLC_FLAGS
    -I ${CMAKE_CURRENT_SOURCE_DIR}
    -I ${CMAKE_CURRENT_BINARY_DIR}
    -noassert
)

# Build ocaml_main
add_custom_target(ocaml_main ALL
    COMMAND ocamlc -o ${CMAKE_CURRENT_BINARY_DIR}/ocaml_main -g ${OCAML_MAIN_FILES} ${OCAMLC_FLAGS}
    DEPENDS ${OCAML_MAIN_FILES}
)

# tests
if(BUILD_TESTING AND PERL_EXECUTABLE)
    add_test(
        NAME ocaml_main1
        COMMAND ${PERL_EXECUTABLE} ${SCRIPTS_DIR}/test-pipe "2+3" ./ocaml_main arith lr
    )
    add_test(
        NAME ocaml_main2
        COMMAND ${PERL_EXECUTABLE} ${SCRIPTS_DIR}/test-pipe "2+3" ./ocaml_main arith glr
    )
    add_test(
        NAME ocaml_main3
        COMMAND ${PERL_EXECUTABLE} ${SCRIPTS_DIR}/test-pipe "2+3" ./ocaml_main een glr
    )
    add_test(
        NAME ocaml_main4
        COMMAND ${PERL_EXECUTABLE} ${SCRIPTS_DIR}/test-pipe "2+3+4" ./ocaml_main een glr ptree
    )
elseif(NOT PERL_EXECUTABLE)
    message(WARNING " * Skipping the ocaml tests: Perl not found")
endif()
