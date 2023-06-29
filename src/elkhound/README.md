Elkhound Overview
=================

Elkhound is a parser generator, similar to [Bison](http://www.gnu.org/software/bison/bison.html). The parsers it generates use the Generalized LR (GLR) parsing algorithm. GLR works with _any_ context-free grammar, whereas LR parsers (such as Bison) require grammars to be LALR(1).

This page is an overview of the implementation. Additional documentation is available:

*   [Elkhound manual](manual.md)
*   [Elkhound tutorial](tutorial.md)
*   [Frequently Asked Questions](faq.md)
*   [Overview of the algorithm and its history](algorithm.md)

To download Elkhound and Elsa, see the [Elkhound distribution page](http://www.cs.berkeley.edu/~smcpeak/elkhound/).

Elkhound requires the following external software:

*   [ast](../ast/readme.md), a system for making abstract syntax trees.
*   [smbase](../smbase/readme.md), my utility library.
*   [Flex](http://www.gnu.org/software/flex/flex.html), a lexical analyzer generator.
*   [Bison](http://www.gnu.org/software/bison/bison.html), a parser generator. (Elkhound does not need Bison once it has been compiled.)

Build instructions:

    $ ./configure
    $ make
    $ make check

[./configure](configure) understands [these options](gendoc/configure.txt). You can also look at the [Makefile](Makefile.in).

Module List:

*   [asockind.h](asockind.h), [asockind.cc](asockind.cc): AssocKind, an enumeration of the kinds of associativity.
*   [cyctimer.h](cyctimer.h), [cyctimer.cc](cyctimer.cc): CycleTimer, which times a section of code in cycles and milliseconds.
*   [emitcode.h](emitcode.h), [emitcode.cc](emitcode.cc): EmitCode, a class to manage the printing of generated code. Its main contribution is knowledge of how to insert #line directives.
*   [flatutil.h](flatutil.h): A few utilities on top of the Flatten interface ([smbase/flatten.h](../smbase/flatten.h)).
*   [genml.h](genml.h), [genml.cc](genml.cc): This module generates parse tables as ML syntax, for when Elkhound is running in ML mode.
*   [glr.h](glr.h), [glr.cc](glr.cc): Core module for the Elkhound parser (the on-line component, not the offline parser generator). Implements a variant of the GLR parsing algorithm. See [glr.h](glr.h) for more info.
*   [gramanl.h](gramanl.h), [gramanl.cc](gramanl.cc): Grammar analysis module. Includes algorithms for computing parse tables. Also includes the main() for the parser generator program, called 'elkhound'.
*   [gramast.ast](gramast.ast): Grammar AST. The input grammar is initially parsed into an AST, before a corresponding Grammar ([grammar.h](grammar.h)) object is created.
*   [gramexpl.cc](gramexpl.cc): Aborted attempt to make an interactive grammar analyzer/explorer/modifier.
*   [gramlex.lex](gramlex.lex): Lexical analyzer for grammar input files. Uses [ast/gramlex.h](../ast/gramlex.h) and [ast/gramlex.cc](../ast/gramlex.cc).
*   [grammar.h](grammar.h), [grammar.cc](grammar.cc): Analysis-time representation of a grammar. Stores symbols, productions, etc. Note that the grammar is distilled down to parse tables for the parser itself; this representation is not normally available during parsing.
*   [grampar.y](grampar.y), [grampar.h](grampar.h), [grampar.cc](grampar.cc): Grammar parser module.
*   [lexerint.h](lexerint.h): LexerInterface, the interface the parser uses to access the lexical analyzer.
*   [mlsstr.h](mlsstr.h), [mlsstr.cc](mlsstr.cc): Module for parsing embedded fragments of ML in reduction actions.
*   [parsetables.h](parsetables.h), [parsetables.cc](parsetables.cc), [emittables.cc](emittables.cc): ParseTables, a container class for the parse tables of a grammar. The parser generator creates the tables, then [emittables.cc](emittables.cc) renders the tables out as code for use by the parser during parsing.
*   [ptreeact.h](ptreeact.h), [ptreeact.cc](ptreeact.cc): A generic set of user actions that build parse trees for any grammar. By making a ParseTreeLexer and ParseTreeActions, you can have a version of your parser which just makes (and optionally prints) a parse tree. This is very useful for debugging grammars.
*   [ptreenode.h](ptreenode.h), [ptreenode.cc](ptreenode.cc): PTreeNode, a generic parse tree node. Forms the basis for the parse trees constructed by [ptreeact.cc](ptreeact.cc).
*   [rcptr.h](rcptr.h): RCPtr, a reference-counting pointer. Used by the parser core to maintain the stack node reference counts.
*   [trivlex.h](trivlex.h), [trivlex.cc](trivlex.cc), [trivmain.cc](trivmain.cc): Lexer and driver program for experimental grammars.
*   [useract.h](useract.h), [useract.cc](useract.cc): UserActions interface, used by the parser core to invoke the actions associated with reductions (and other events).
*   [util.h](util.h): Some random macros.

I used [smbase/scan-depends.pl](../smbase/scan-depends.pl) to make dependency diagrams of the parser generator, and the run-time parser.

The parser generator:  
![Elkhound dependencies](gendoc/elkhound_dep.png)  
Or as [Postscript](gendoc/elkhound_dep.ps).

The run-time parser:  
![GLR Parser dependencies](gendoc/glr.png)  
Or as [Postscript](gendoc/glr.ps).

Miscellanous files:

*   [find-extra-deps](find-extra-deps): This script finds dependencies among automatically-generated source files. Output is [extradep.mk](extradep.mk). [make-lrtable-graph](make-lrtable-graph): Given the .out file from a run of Elkhound with the \-tr lrtable option, produce a Dot-format graph of the parsing automaton.
*   [make-tok](make-tok): Read a C++ header file containing a token code enumeration declaration, and produce a corresponding .tok file for Elkhound.
*   [make-trivparser.pl](make-trivparser.pl): This script makes an Elkhound .gr file from a more-compact description of an experimental grammar.
*   [perf](perf): Run some performance tests.
*   [regrtest](regrtest): Run the regression tests.

Interesting subdirectories:

*   [asfsdf](asfsdf): Contains a few examples written for the ASF+SDF meta-framework, for performance comparison with Elkhound.
*   [c](c): Contains a C parser written in Elkhound. The grammar is almost free of shift/reduce conflicts. It uses the lexer hack (feedback from the symbol table into the lexer) to distinguish variables from types. The lexer itself is very slow (just bad engineering). This parser could be evolved into something useful in its own right, but at this time it's mostly just a test of Elkhound's deterministic parser.
*   [cc2](cc2): This is a C++ parser that uses the C++ Standard's grammar without modification. Consequently, it has many shift/reduce and reduce/reduce conflicts, and also many true ambiguities. This grammar is not a very good grammar to use for parsing C++ input; rather, it's essentially the skeleton on which the standard's English description hangs. Nevertheless, it's useful because it will output exactly the parse tree (ambiguities and all) that the standard grammar induces, and this can then be compared to similar output from Elsa
*   [examples](examples): Contains some example parsers written with Elkhound:
    *   [examples/arith](examples/arith): Simple parser for arithmetic expressions. This example was intended to introduce a new user to some of Elkhound's syntax and semantics.
    *   [examples/cdecl](examples/cdecl): This contains an abstracted fragment of the C grammar, demonstrating the ambiguity between variable and type names. The accompanying code resolves the ambiguity using reduction cancellation.
    *   [examples/cexp](examples/cexp): This has two examples of parsing C expressions. The examples are old and don't show much; I used them to test Elkhound during early development.
    *   [examples/gcom](examples/gcom): One of several directories of files used in the [tutorial](tutorial.html).
*   [in](in): This has a bunch of inputs for various testing grammars.
*   [triv](triv): This is a collection of micro-grammars for performance and correctness testing. The name "triv" comes from the fact that they all use the trival lexer, i.e. a lexer that yields every character as a separate token.

