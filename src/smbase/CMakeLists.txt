#
# ast CMakeLists.txt
#
project(smbase)

# all the files
add_library(smbase STATIC
    autofile.cc
    bflatten.cc
    bit2d.cc
    bitarray.cc
    boxprint.cc
    breaker.cpp
    crc.cpp
    cycles.c
    datablok.cpp
    exc.cpp
    flatten.cc
    gprintf.c
    growbuf.cc
    hashline.cc
    hashtbl.cc
    malloc_stub.c
    missing.cpp
    mypopen.c
    mysig.cc
    nonport.cpp
    point.cc
    smregexp.cc
    srcloc.cc
    str.cpp
    strdict.cc
    strhash.cc
    stringset.cc
    strtokp.cpp
    strutil.cc
    svdict.cc
    syserr.cpp
    trace.cc
    trdelete.cc
    unixutil.c
    vdtllist.cc
    voidlist.cc
    vptrmap.cc
    warn.cpp
)

# extra compile options
target_compile_options(smbase PRIVATE -Wno-maybe-uninitialized)
SET_SOURCE_FILES_PROPERTIES( srcloc.cc PROPERTIES COMPILE_FLAGS -Wno-nonnull-compare )
