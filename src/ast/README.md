ast: A system for creating Abstract Syntax Trees
================================================

The "ast" system takes as input a description of a heterogenous tree structure, and outputs C++ code to define a set of classes that implement the described tree. Abstract Syntax Trees, used in language processors (like compilers), are the principal intended application, though of course the need for heterogenous trees arises in other situations as well.

As a short example, the following is a sample input to the "astgen" tool. It's also in the file [demo.ast](demo.ast).

```c++
class Root(A a, B b);

class A {
  -> A_one(string name);
  -> A_two(B b);
}

class B(string name) {
  public int x = 0;

  -> B_one(int y);
  -> B_two(char c);
}
```

From this description, astgen produces two files, [demo.h](gendoc/demo.h) and [demo.cc](gendoc/demo.cc). You can then write C++ code that uses these tree classes.

The basic inspiration for the ast system is ML's disjoint union types, but I extended them in several ways:

*   You can have fields that are present in all variants; the 'name' field of class B, above, is an example.
*   You can have fields that are not initialized by arguments to the constructor, but instead have a default value, and behave like mutable annotations. The 'x' field of class B is an example.
*   The fields that _are_ set in the constructor can have default values, using C++'s usual mechanism, so it's possible to add a field without having to rewrite every occurrence of a call to the constructor.
*   ast classes are full C++ classes, and as such can have methods, etc.

For more information about the features of the ast system, consult the [AST Manual](manual.md).

The ast system requires the following external software:

*   [smbase](../smbase/index.html), my utility library.
*   [Flex](http://www.gnu.org/software/flex/flex.html), a lexical analyzer generator.
*   [Bison](http://www.gnu.org/software/bison/bison.html), a parser generator.

Build instructions:

```
$ ./configure
$ make
$ make check
```

[./configure](configure) understands [these options](gendoc/configure.txt). You can also look at the [Makefile](Makefile.in).

## Module List:

*   [agramlex.lex](agramlex.lex): Lexical analyzer for .ast files. See also [gramlex.h](gramlex.h).
*   [agrampar.y](agrampar.y), [agrampar.h](agrampar.h), [agrampar.cc](agrampar.cc): Parser for .ast files.
*   [ast.ast](ast.ast), [ast.ast.h](ast.ast.h), [ast.ast.cc](ast.ast.cc): The AST of an .ast file. The outputs of astgen are included for bootstrapping purposes.
*   [astgen.cc](astgen.cc): The main program. Translates .ast files into .h and .cc files.
*   [asthelp.h](asthelp.h), [asthelp.cc](asthelp.cc): This is a support module for astgen-generated code.
*   [ccsstr.h](ccsstr.h): [ccsstr.cc](ccsstr.cc): Implements the EmbeddedLang interface ([embedded.h](embedded.h)) for C++.
*   [embedded.h](embedded.h), [embedded.cc](embedded.cc): Interface for skipping comments and balanced delimiters in a language embedded into another.
*   [example.ast](example.ast), [exampletest.cc](exampletest.cc): Testbed for new astgen features. Not a particularly good pedagogical example.
*   [ext1.ast](ext1.ast): Example extension module for [example.ast](example.ast).
*   [fakelist.h](fakelist.h): FakeList, a linked-list wrapper module for lists of objects built out of internal 'next' pointers. See [fakelist.h](fakelist.h) for a more complete description and rationale.
*   [gramlex.h](gramlex.h), [gramlex.cc](gramlex.cc): Lexical analyzer support module. Goes with [agramlex.lex](agramlex.lex). This module is also used by [elkhound/gramlex.lex](../elkhound/gramlex.lex) in the [Elkhound](../elkhound/index.html) distribution.
*   [locstr.h](locstr.h), [locstr.cc](locstr.cc): Pair of a SourceLoc ([smbase/srcloc.h](../smbase/srcloc.h)) and a StringRef ([strtable.h](strtable.h)).
*   [reporterr.h](reporterr.h), [reporterr.cc](reporterr.cc): Interface for reporting errors and warnings.
*   [strtable.h](strtable.h), [strtable.cc](strtable.cc): StringTable, a global table of immutable strings. Duplicate strings are mapped to the same entry, allowing pointer comparisons to suffice for string equality tests.
