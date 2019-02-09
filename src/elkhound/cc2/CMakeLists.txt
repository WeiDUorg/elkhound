#
# cc2 CMakeLists.txt
#
project(cc2)

# generate cc2t.gr.gen.{cc,h}
add_custom_command(
    OUTPUT cc2.gr.gen.cc cc2.gr.gen.h
    COMMAND ${ELKHOUND_DIR}/elkhound -tr treebuild,lrtable -o cc2.gr.gen ${CMAKE_CURRENT_SOURCE_DIR}/cc2.gr
    DEPENDS elkhound cparse
)

# all the files for cc2
add_executable(cc2
    cc2.gr.gen.cc
    cc2main.cc
)

# link options
target_link_libraries(cc2 libcparse)

