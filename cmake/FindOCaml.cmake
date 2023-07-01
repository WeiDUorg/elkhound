# CMake find_package() module for the OCaml language.
# Assumes ocamlfind will be used for compilation.
# http://ocaml.org/
#
# Example usage:
#
# find_package(OCaml)
#
# If successful, the following variables will be defined:
# OCAMLFIND
# OCAML_BINDIR
# OCAML_VERSION
# OCAML_STDLIB_PATH
# HAVE_OCAMLOPT
#

include(FindPackageHandleStandardArgs)

if(MSVC)
    list(APPEND OCAML_HINTS "$ENV{LOCALAPPDATA}/Programs/DiskuvOCaml/usr/bin/")
endif()

find_program(OCAMLFIND
             NAMES ocamlfind
             HINTS ${OCAML_HINTS})

if(OCAMLFIND)
  get_filename_component(OCAML_BINPATH ${OCAMLFIND} DIRECTORY)

  execute_process(
    COMMAND ${OCAMLFIND} ocamlc -version
    OUTPUT_VARIABLE OCAML_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE)

  execute_process(
    COMMAND ${OCAMLFIND} ocamlc -where
    OUTPUT_VARIABLE OCAML_STDLIB_PATH
    OUTPUT_STRIP_TRAILING_WHITESPACE)

  execute_process(
    COMMAND ${OCAMLFIND} ocamlc -version
    OUTPUT_QUIET
    RESULT_VARIABLE find_ocaml_result)
  
  
  if(find_ocaml_result EQUAL 0)
    set(HAVE_OCAMLOPT TRUE)
  else()
    set(HAVE_OCAMLOPT FALSE)
  endif()
endif()

find_package_handle_standard_args( OCaml DEFAULT_MSG
  OCAMLFIND
  OCAML_BINPATH
  OCAML_VERSION
  OCAML_STDLIB_PATH)

mark_as_advanced(
  OCAMLFIND
  OCAML_HINTS)
