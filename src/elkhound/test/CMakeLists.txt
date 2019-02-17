#
# elkhound test CMakeLists.txt
#

# tests
project(mlsstr)
project(trcptr)

# files for mlsstr
add_executable(mlsstr
    ../mlsstr.cc
)

# files for trcptr
add_executable(trcptr
  ../trcptr.cc
  )

# extra compile options
target_compile_options(mlsstr PRIVATE -DTEST_MLSSTR)

# link options
target_link_libraries(mlsstr smbase ast)
target_link_libraries(trcptr smbase)

# add the tests
add_test (NAME mlsstr COMMAND ./mlsstr)
add_test (NAME trcptr COMMAND ./trcptr)
