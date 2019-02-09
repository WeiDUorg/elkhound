#
# elkhound test CMakeLists.txt
#

# tests
project(mlsstr)

# files for mlsstr
add_executable(mlsstr
    ../mlsstr.cc
)

# extra compile options
target_compile_options(mlsstr PRIVATE -DTEST_MLSSTR)

# link options
target_link_libraries(mlsstr smbase ast)

# add the tests
add_test (NAME mlsstr COMMAND ./mlsstr)
