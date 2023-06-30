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
    mysig.cc
    nonport.cpp
    point.cc
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
    vdtllist.cc
    voidlist.cc
    vptrmap.cc
)
