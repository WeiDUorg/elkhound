# Elkhound

Elkhound is a parser generator which emits GLR parsers, either in
OCaml or C++. It was written by Scott McPeak and is licensed under
3-clause BSD terms. See license.txt.

## Building

Elkhound is built with CMake and a C/C++ toolchain.

Elkhound has been tested and builds on what can vaguely be described
as contemporary x86_64 and x86 GNU/Linux systems and Cygwin. It should
also build on contemporary macOS. On Windows it can also be built with
MSVC, tested with CMake 3.17+ and Visual Studio 2017, 2019 and 2022.

Status on other systems and in other environments is not known.

If you are building on Cygwin, you may want to add the following line
to your .bashrc:\
`export CMAKE_LEGACY_CYGWIN_WIN32=0`

## Dependencies

On Windows, a build takes no external dependencies. The included copy of `winflexbison` is built and used for convenience.

On Unix, the build requires:

- Flex
- Bison

Optionally, on all platforms:

- Perl is used in building some of the examples and running tests. It's used if found.
- OCaml is used to build and run OCaml-targeting examples. It's used if found.
  On Windows, a Diskuv distrubution of OCaml should be detected automatically.

To build, enter from elkhound's root directory:
- `mkdir build && cd build`
- `cmake ../src/ -DCMAKE_BUILD_TYPE=$type` where $type is either
  `Debug`, `Release` or one of the other CMake build types
- `make`
- `make test`

If everything was successful, you'll find the elkhound binary in
`build/elkhound` (this is the same build directory as above).

On Windows, under Visual Studio 2017 or newer, use *Open Folder* with the root
working copy folder to get a full "IDE experience".

cmake can take a few arguments when configuring the build system:
- `-DEXTRAS=OFF` for a leaner build that just produces elkhound itself
- `-DOCAML=OFF` if OCaml should not be used even if present

## Additional information

[http://scottmcpeak.com/elkhound/](http://scottmcpeak.com/elkhound/)

There is also a tutorial and other texts among the source files. Have
a look at elkhound/index.html (some parts are out of date).
