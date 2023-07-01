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
    COMMAND ${BISON_EXECUTABLE} -d ${CMAKE_CURRENT_SOURCE_DIR}/agrampar.y
    COMMAND ${CMAKE_COMMAND} -E copy agrampar.tab.c agrampar.tab.cpp
    COMMAND ${CMAKE_COMMAND} -E remove agrampar.tab.c
)

# generate agramlex.cc
add_custom_command(
    OUTPUT agramlex.cc
    COMMAND ${FLEX_EXECUTABLE} -oagramlex.cc ${CMAKE_CURRENT_SOURCE_DIR}/agramlex.lex
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

include_directories(${FLEX_INCLUDE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
