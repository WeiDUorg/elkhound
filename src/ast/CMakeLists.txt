#
# ast CMakeLists.txt
#
# libast.a
project(ast)

# astgen
project(astgen)

# all the files for libast.a
add_library(ast STATIC
    asthelp.cc
    ccsstr.cc
    embedded.cc
    gramlex.cc
    locstr.cc
    reporterr.cc
    strtable.cc
    xmlhelp.cc
)

# generate agrampar.tab.{cpp,h}
add_custom_command(
    OUTPUT agrampar.tab.cpp agrampar.tab.h
    COMMAND bison -d ${CMAKE_CURRENT_SOURCE_DIR}/agrampar.y && mv agrampar.tab.c agrampar.tab.cpp
)

# generate agramlex.cc
add_custom_command(
    OUTPUT agramlex.cc
    COMMAND flex -oagramlex.cc ${CMAKE_CURRENT_SOURCE_DIR}/agramlex.lex
)

# all the files for astgen
add_executable(astgen
    agramlex.cc
    agrampar.cc
    agrampar.tab.cpp
    ast.ast.cc
    astgen.cc
    asthelp.cc
    ccsstr.cc
    embedded.cc
    gramlex.cc
    reporterr.cc
    strtable.cc
    xmlhelp.cc
)

# link astgen against libast.a amd libsmbase.a
target_link_libraries(astgen ast smbase)
