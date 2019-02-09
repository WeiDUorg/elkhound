#
# cdecl CMakeLists.txt
#
project(cdecl)

# generate cdecl.gr.gen.{cc,h}
add_custom_command(
    OUTPUT cdecl.gr.gen.cc cdecl.gr.gen.h
    COMMAND ${ELKHOUND_DIR}/elkhound -o cdecl.gr.gen ${CMAKE_CURRENT_SOURCE_DIR}/cdecl.gr
    DEPENDS elkhound
)

# all the files for cdecl
add_executable(cdecl
   cdecl.gr.gen.cc
   ../../glrmain.cc
)

# link against elkhound, smbase, libcparse and ast
target_link_libraries(cdecl libelkhound smbase libcparse ast)

