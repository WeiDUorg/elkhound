Elkhound
========

Elkhound is a parser generator which emits GLR parsers, either in
OCaml or C++. It was written by Scott McPeak and is licensed under
3-clause BSD terms. See license.txt.

Building
--------

Elkhound is built with CMake and a C/C++ toolchain.

Elkhound has been tested and builds on what can vaguely be described
as contemporary x86_64 and x86 GNU/Linux systems and Cygwin. It should
also build on contemporary macOS. Elkhound itself can be built with
MSVC++, tested with CMake 3.17 and Visual Studio 2017, with options
equivalent to `-DEXTRAS=OFF -DOCAML=OFF` (_vide infra_). Status on
other systems and in other environments is not known.

Dependencies:
- Flex
- Bison

To build, enter from elkhound's root directory:
- `mkdir build && cd build`
- `cmake ../src/ -DCMAKE_BUILD_TYPE=$type` where $type is either
  `Debug`, `Release` or one of the other CMake build types
- `make`
- `make test`

If everything was successful, you'll find the elkhound binary in
`build/elkhound` (this is the same build directory as above).

cmake can take a few arguments when configuring the build system:
- `-DEXTRAS=OFF` for a leaner build that just produces elkhound itself
- `-DOCAML=OFF` or you need to have an OCaml distribution installed

If you are building on Cygwin, you may want to add the following line
to your .bashrc:\
`export CMAKE_LEGACY_CYGWIN_WIN32=0`

Additional information
----------------------

[http://scottmcpeak.com/elkhound/](http://scottmcpeak.com/elkhound/)

There is also a tutorial and other texts among the source files. Have
a look at [src/elkhound/README.md](src/elkhound/README.md) (some parts are out of date).
