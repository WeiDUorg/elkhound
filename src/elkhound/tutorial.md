Elkhound Tutorial
================

<img src="https://web.archive.org/web/20120426153703im_/http://www.castlebarelkhounds.com/puppy111.jpg" alt="An elkhound puppy" style="zoom:150%;" />

The purpose of this tutorial is to walk through the steps of building a parser using Elkhound. Familiarity with another parser generator (such as [Bison](http://www.gnu.org/manual/bison-1.25/bison.html)) might be helpful but should not be necessary.

The [Elkhound manual](manual.md) may also be of use, but at the moment the manual is far from complete, and this tutorial has much more information.

Contents
--------

* [1. The Language to Parse](#1\.-the-language-to-parse)
* [2. The Lexer](#2\.-the-lexer)
* [3. A grammar for AExp](#3\.-a-grammar-for-aexp)
* [4. The parser driver](#4\.-the-parser-driver)
* [5. Resolving the ambiguity](#5\.-resolving-the-ambiguity)
* [6. Parse actions](#6\.-parse-actions)
* [7. Filling out the language](#7\.-filling-out-the-language)
* [8. Building an AST](#8\.-building-an-ast)
* [9. Late disambiguation](#9\.-late-disambiguation)
* [Conclusion](#conclusion)
* [References](#references)

1\. The Language to Parse
-------------------------

I'll use Dijkstra's guarded command language as the example language to parse 

[^1]: Edsger W. Dijkstra. _A Discipline of Programming._ Prentice-Hall, 1976.

We can describe the syntax with the nonterminals **AExp** (arithmetic expression), **BExp** (boolean expression), **Stmt** (statement) and **GCom** (guarded command):

```
AExp ::= n                  // integer literal
       | x                  // variable name
       | AExp + AExp        // addition
       | AExp - AExp        // subtraction
       | AExp * AExp        // multiplication
       | (AExp)             // grouping parentheses

BExp ::= true
       | false
       | AExp = AExp        // equality test
       | AExp < AExp        // less than
       | !BExp              // boolean negation
       | BExp /\ BExp       // and
       | BExp \/ BExp       // or
       | (BExp)             // grouping

Stmt ::= skip               // do nothing
       | abort              // terminate execution unsuccessfully
       | print x            // print variable value
       | x := AExp          // variable assignment
       | Stmt ; Stmt        // sequential execution
       | if GCom fi         // guarded command
       | do GCom od         // loop

GCom ::= BExp -> Stmt       // run command if expression is true
       | GCom # GCom        // nondeterministic choice (using "#" for "fatbar")
```

My hope is this example language will illustrate the main tasks of parser construction without being yet another "desktop calculator" example. Of course, we'll start with just AExp (since it's the only nonterminal that doesn't depend on the others), so initially it will be the same old example.

2\. The Lexer
-------------

The first step is to write the lexer. While it is possible to put the lexer definition right into the grammar file (using the verbatim and impl_verbatim directives), and doing so might make the example shorter, it would not represent very good design. So the lexer is its own module.

### 2.1   lexer.h

The header [lexer.h](examples/gcom1/lexer.h) starts with an enumeration describing the tokens and their associated codes:

    // token codes (must agree with the parser)
    enum TokenCode {
      TOK_EOF         = 0,     // end of file
      TOK_LITERAL,             // integer literal
      TOK_IDENTIFIER,          // identifier like "x"
      TOK_PLUS,                // "+"
      TOK_MINUS,               // "-"
      TOK_TIMES,               // "*"
      TOK_LPAREN,              // "("
      TOK_RPAREN,              // ")"
    };

Next, a class needs to implement LexerInterface ([lexerint.h](lexerint.h)). The parser's interaction with the lexer is conducted via this interface:

    // read characters from stdin, yield tokens for the parser
    class Lexer : public LexerInterface {
    public:
      // function that retrieves the next token from
      // the input stream
      static void nextToken(LexerInterface *lex);
      virtual NextTokenFunc getTokenFunc() const
      { return &Lexer::nextToken; }
    
      // debugging assistance functions
      string tokenDesc() const;
      string tokenKindDesc(int kind) const;
    };


### 2.2   lexer.cc

Typically, one would use a lexer generator such as [flex](http://www.gnu.org/software/flex/) to help write a fast lexer. However, this tutorial is not about flex, and is not concerned with performance, so we'll use a simple hand-coded lexer. The lexer's primary task is to set the type field equal to the code of the token that is found. For some tokens (TOK_LITERAL and TOK_IDENTIFIER in our example), it also sets the sval (semantic value) field. The meaning of sval is decided by the person who writes the lexer.

Here is the Lexer::nextToken function from [lexer.cc](examples/gcom1/lexer.cc):

    void Lexer::nextToken(LexerInterface *lex)
    {
      int ch = getchar();
    
      // skip whitespace
      while (isspace(ch)) {
        ch = getchar();
      }
    
      // end of file?
      if (ch == EOF) {
        lex->type = TOK_EOF;
        return;
      }
    
      // simple one-character tokens
      switch (ch) {
        case '+': lex->type = TOK_PLUS; return;
        case '-': lex->type = TOK_MINUS; return;
        case '*': lex->type = TOK_TIMES; return;
        case '(': lex->type = TOK_LPAREN; return;
        case ')': lex->type = TOK_RPAREN; return;
      }
    
      // integer literal
      if (isdigit(ch)) {
        int value = 0;
        while (isdigit(ch)) {
          value = value*10 + ch-'0';
          ch = getchar();
        }
        ungetc(ch, stdin);      // put back the nondigit
    
        // semantic value is the integer value of the literal
        lex->sval = (SemanticValue)value;
    
        lex->type = TOK_LITERAL;
        return;
      }
    
      // identifier
      if (isalpha(ch)) {        // must start with letter
        char buf[80];
        int i=0;
        while (isalnum(ch)) {   // but allow digits later on
          buf[i++] = (char)ch;
          if (i==80) {
            fprintf(stderr, "identifier is too long\n");
            abort();
          }
          ch = getchar();
        }
        buf[i]=0;
        ungetc(ch, stdin);
    
        // semantic value is a pointer to an allocated string; it
        // is simply leaked (never deallocated) for this example
        lex->sval = (SemanticValue)strdup(buf);
    
        lex->type = TOK_IDENTIFIER;
        return;
      }
    
      fprintf(stderr, "illegal character: %c\n", ch);
      abort();
    }


The lexer interface includes functions that return information about the tokens, mainly to assist in debugging. For example, when the Elkhound parser is told to print out the actions it is taking, it uses these description functions to make that output more informative.

    string Lexer::tokenDesc() const
    {
      switch (type) {
        // for two kinds of tokens, interpret their semantic value
        case TOK_LITERAL:      return stringf("%d", (int)sval);
        case TOK_IDENTIFIER:   return string((char*)sval);
    
        // otherwise, just return the token kind description
        default:               return tokenKindDesc(type);
      }
    }


    string Lexer::tokenKindDesc(int kind) const
    {
      switch (kind) {
        case TOK_EOF:          return "EOF";
        case TOK_LITERAL:      return "lit";
        case TOK_IDENTIFIER:   return "id";
        default: {
          static char const map[] = "+-*()";
          return substring(&map[kind-TOK_PLUS], 1);
        }
      }
    }


### 2.3   Test the lexer

Finally, it's useful to write a simple driver to test the lexer by itself. It also illustrates how the parser will use the lexer interface. This driver will only be used for the lexer test program; it's not the main() function of the completed parser program.

    #ifdef TEST_LEXER
    int main()
    {
      Lexer lexer;
      for (;;) {
        lexer.getTokenFunc()(&lexer);    // first call yields a function pointer
    
        // print the returned token
        string desc = lexer.tokenDesc();
        printf("%s\n", desc.c_str());
    
        if (lexer.type == TOK_EOF) {
          break;
        }
      }
    
      return 0;
    }
    #endif // TEST_LEXER

We can compile and run this completed lexer test program:

    $ g++ -o lexer -g -Wall -I../.. -I../../../smbase -DTEST_LEXER lexer.cc \
      ../../libelkhound.a ../../../smbase/libsmbase.a
    $ echo "5 + myVar * (6 + 7)" | ./lexer
    5
    +
    myVar
    *
    (
    6
    +
    7
    )
    EOF

The [Makefile](examples/gcom1/Makefile) knows how to do the compilation step; just say make lexer in the examples/gcom1 directory of Elkhound.

3\. A grammar for AExp
----------------------

### 3.1   Context class

All of the parsing actions become methods of a class, called the parser context class. Fields of that class do the job that would be done with global variables in other parser generators. Since we don't need any such context yet, we use an empty class. It must implement the UserActions ([useract.h](useract.h)) interface.

    context_class GCom : public UserActions {
    public:
      // empty for now
    };

### 3.2   Terminals

Next, the tokens or terminal symbols of the grammar must be declared, along with their numeric codes. Tokens can be given optional aliases (in quotes) to make the grammar that follows more readable.

    terminals {
      0 : TOK_EOF                        ;
      1 : TOK_LITERAL                    ;
      2 : TOK_IDENTIFIER                 "x";
      3 : TOK_PLUS                       "+";
      4 : TOK_MINUS                      "-";
      5 : TOK_TIMES                      "*";
      6 : TOK_LPAREN                     "(";
      7 : TOK_RPAREN                     ")";
    }

Since this information repeats information already in lexer.h, there is a script, [make-tok](make-tok), that can create it automatically. Run the script like this:

    $ perl ../../make-tok TokenCode <lexer.h >;tokens.tok

and then the terminals section of the grammar becomes simply

    terminals {
      include("tokens.tok")
    }

### 3.3   The grammar

Finally, we specify the grammar. Nonterminals are introduced with the nonterm keyword, then the name of the nonterminal and an open-brace ("{"). Inside the braces are a sequence of right-hand sides: begin with "->" (pronounced "rewrites as"), then a sequence of terminals or nonterminals, then a semicolon (";").

    nonterm AExp {
      -> TOK_LITERAL;
      -> TOK_IDENTIFIER;
      -> AExp "+" AExp;
      -> AExp "-" AExp;
      -> AExp "*" AExp;
      -> "(" AExp ")";
    }

The syntax is free-form; all whitespace is equivalent. The example could have been written all on one line, or spread out onto even more lines (with blank lines wherever). You can put comments, either C++-style "//" or C-style "/*...*/", anywhere you can put whitespace.

Some optional components have been left out of this example: semantic value types, right-hand side (RHS) labels, and parse actions. They will be addressed in subsequent sections.

### 3.4   Running elkhound

The next step is to run elkhound, the parser generator program. (Run it without arguments to see a short usage description.)

    $ ../../elkhound gcom.gr
    9 shift/reduce conflicts

It has some shift/reduce conflicts, because the grammar is ambiguous, but we'll deal with them later.

elkhound wrote output to two files, gcom.h and gcom.cc. gcom.h contains the definition of the parser context class, GCom. It consists of whatever appeared in the context_class declaration in the grammar file, plus declarations used during parsing.

    // gcom.h
    // *** DO NOT EDIT BY HAND ***
    // automatically generated by elkhound, from gcom.gr
    
    #ifndef GCOM_H
    #define GCOM_H
    
    #include "useract.h"     // UserActions
    
    // parser context class
    class
    #line 6 "gcom.gr"
     GCom : public UserActions {
    public:
      // empty for now
    
    #line 19 "gcom.h"
    
    private:
      USER_ACTION_FUNCTIONS      // see useract.h
    
      // declare the actual action function
      static SemanticValue doReductionAction(
        GCom *ths,
        int productionId, SemanticValue const *semanticValues,
      SourceLoc loc);
    
      // declare the classifier function
      static int reclassifyToken(
        GCom *ths,
        int oldTokenType, SemanticValue sval);
    
      void action0___EarlyStartSymbol(SourceLoc loc, SemanticValue top);
      void action1_AExp(SourceLoc loc);
      void action2_AExp(SourceLoc loc);
      void action3_AExp(SourceLoc loc);
      void action4_AExp(SourceLoc loc);
      void action5_AExp(SourceLoc loc);
      void action6_AExp(SourceLoc loc);
    
    // the function which makes the parse tables
    public:
      virtual ParseTables *makeTables();
    };
    
    #endif // GCOM_H

gcom.cc contains implementations of those functions, plus the parse tables themselves as static data. I don't include example output here because the details aren't very important right now.

4\. The parser driver
---------------------

Finally, we're ready to write a main() function to tie it all together. Again, this could have been stuffed into gcom.gr, but it's better to separate it into another file ([parser.cc](examples/gcom1/parser.cc)) for maintainability.

    #include "lexer.h"     // Lexer
    #include "gcom.h"      // GCom
    #include "glr.h"       // GLR
    
    int main()
    {
      // create and initialize the lexer
      Lexer lexer;
      lexer.nextToken(&lexer);
    
      // create the parser context object
      GCom gcom;
    
      // initialize the parser
      GLR glr(&gcom, gcom.makeTables());
    
      // parse the input
      SemanticValue result;
      if (!glr.glrParse(lexer, result)) {
        printf("parse error\n");
        return 2;
      }
    
      // print result
      printf("result: %d\n", (int)result);
    
      return 0;
    }

Compile and link this program (can also use the [Makefile](examples/gcom1/Makefile): "make parser"):

    $ g++ -c -o lexer.o -g -Wall -I../.. -I../../../smbase lexer.cc
    $ g++ -c -o parser.o -g -Wall -I../.. -I../../../smbase parser.cc
    $ g++ -c -o gcom.o -g -Wall -I../.. -I../../../smbase gcom.cc
    $ g++ -o parser lexer.o parser.o gcom.o -g -Wall ../../libelkhound.a ../../../smbase/libsmbase.a

Right now, it doesn't do much more than recognize the language:

    $ echo "2" | ./parser
    result: 0
    
    $ echo "2 + 3" | ./parser
    result: 0
    
    $ echo "2 + 3 +" | ./parser
    WARNING: there is no action to deallocate terminal TOK_PLUS
    In state 4, I expected one of these tokens:
      [1] lit
      [2] id
      [6] (
    <noloc>:1:1: Parse error (state 4) at EOF
    parse error
    
    $ echo "2 + 3 + 5" | ./parser
    <noloc>:1:1: WARNING: there is no action to merge nonterm AExp
    result: 0
    
    $ echo "2 + 3 + 5 +" | ./parser
    <noloc>:1:1: WARNING: there is no action to merge nonterm AExp
    WARNING: there is no action to deallocate terminal TOK_PLUS
    In state 4, I expected one of these tokens:
      [1] lit
      [2] id
      [6] (
    <noloc>:1:1: Parse error (state 4) at EOF
    
    parse error

The parse error message explains which tokens would have allowed the parser to make progress for at least one more token of input. Elkhound's error diagnosis and recovery is unfortunately still quite primitive; among the TODOs is to improve it. Anyway, the output can be suppressed if desired by adding to main():

    glr.noisyFailedParse = false;

The complaints about not being able to deallocate terminals mean the parser is dropping semantic values on the floor. Elkhound offers a way to specify what should happen in that case, but since we did not, the parser prints a warning. The warning can be suppressed by adding at the top of gcom.gr:

    option useGCDefaults;

2005-03-03: I just made it so that terminals with no declared type are silently dropped on the floor, so you won't see the above warnings about deallocating terminals.

Finally, the warning about merging the nonterminal AExp means that the parser discovered an ambiguity, but the grammar did not specify how to handle it, so (at least) one of the ambiguous alternatives was arbitrarily dropped. We could suppress that by specifying an empty ambiguity resolution procedure, but let's leave it alone for now.

The source files for this point in the tutorial are in the [examples/gcom1](examples/gcom1) directory:

* [lexer.h](examples/gcom1/lexer.h)
* [lexer.cc](examples/gcom1/lexer.cc)
* [gcom.gr](examples/gcom1/gcom.gr)
* [parser.cc](examples/gcom1/parser.cc)
* [Makefile](examples/gcom1/Makefile)

5\. Resolving the ambiguity
---------------------------

### 5.1   Look at the conflicts

Often, you can tell what the problem is by looking at the parser's conflict report. To do that, run elkhound with "-tr conflict":

    $ ../../elkhound -tr conflict gcom.gr
    %%% conflict: --------- state 11 ----------
    left context: AExp + AExp
    sample input: TOK_LITERAL + TOK_LITERAL
    %%% conflict: conflict for symbol TOK_PLUS
    %%% conflict:   shift, and move to state 4
    %%% conflict:   reduce by rule [3] AExp[void] -> AExp + AExp
    %%% conflict: conflict for symbol TOK_MINUS
    %%% conflict:   shift, and move to state 5
    %%% conflict:   reduce by rule [3] AExp[void] -> AExp + AExp
    %%% conflict: conflict for symbol TOK_TIMES
    %%% conflict:   shift, and move to state 6
    %%% conflict:   reduce by rule [3] AExp[void] -> AExp + AExp
    (etc.)

What this is saying is that after seeing "AExp + AExp", if it sees a "+", "-" or "*" then it has two possible actions:

* reduce using "AExp -> AExp + AExp", or
* shift the symbol, in hopes of reducing that symbol before reducing via the "+" rule

This information may be enough for you to understand what the problem is and how to solve it. However, it's rather low-level information, and sometimes it isn't clear where the problem actually lies. Also, not every conflict leads to an ambiguity.

For those familiar with other parser generators, you may find yourself wanting to see the entire LR table. For that, add "-tr lrtable" to the elkhound command line. It will write another file (gcom.out).

### 5.2   Print the tree

If you have an input that demonstrates a geniune ambiguity, you can have Elkhound print the parse tree, including ambiguities. The parse tree can then be compared with the input and grammar to decide how to proceed.

To print the parse tree, we just need to change the driver a little. Specifically, we wrap the lexer with a version that just yields the nonterminal name, and substitute the given actions with actions that build a parse tree. We need two more headers:

    #include "ptreenode.h" // PTreeNode
    #include "ptreeact.h"  // ParseTreeLexer, ParseTreeActions

Then, run the parser like this:

    // wrap the lexer and actions with versions that make a parse tree
    ParseTreeLexer ptlexer(&lexer, &gcom);
    ParseTreeActions ptact(&gcom, gcom.makeTables());
    
    // initialize the parser
    GLR glr(&ptact, ptact.getTables());
    
    // parse the input
    SemanticValue result;
    if (!glr.glrParse(ptlexer, result)) {
      printf("parse error\n");
      return 2;
    }
    
    // print the tree
    PTreeNode *ptn = (PTreeNode*)result;
    ptn->printTree(cout, PTreeNode::PF_EXPAND);

The file [parser.cc](examples/gcom2/parser.cc) decides whether to print the tree based on a command-line argument "-tree". The new parser gives some interesting output:

    $ echo "2" | ./parser -tree
    __EarlyStartSymbol -> AExp TOK_EOF
      AExp -> TOK_LITERAL
        TOK_LITERAL
      TOK_EOF


    $ echo "2 + 3" | ./parser -tree
    __EarlyStartSymbol -> AExp TOK_EOF
      AExp -> AExp TOK_PLUS AExp
        AExp -> TOK_LITERAL
          TOK_LITERAL
        TOK_PLUS
        AExp -> TOK_LITERAL
          TOK_LITERAL
      TOK_EOF


    $ echo "2 + 3 + 4" | ./parser -tree
    __EarlyStartSymbol -> AExp TOK_EOF
      --------- ambiguous AExp: 1 of 2 ---------
        AExp -> AExp TOK_PLUS AExp
          AExp -> AExp TOK_PLUS AExp
            AExp -> TOK_LITERAL
              TOK_LITERAL
            TOK_PLUS
            AExp -> TOK_LITERAL
              TOK_LITERAL
          TOK_PLUS
          AExp -> TOK_LITERAL
            TOK_LITERAL
      --------- ambiguous AExp: 2 of 2 ---------
        AExp -> AExp TOK_PLUS AExp
          AExp -> TOK_LITERAL
            TOK_LITERAL
        TOK_PLUS
        AExp -> AExp TOK_PLUS AExp
          AExp -> TOK_LITERAL
            TOK_LITERAL
          TOK_PLUS
          AExp -> TOK_LITERAL
            TOK_LITERAL
    --------- end of ambiguous AExp ---------
    TOK_EOF


As one can tell by inspecting the grammar, an ambiguity arises because the production

    AExp -> AExp "+" AExp

does not specify an associativity for "+". Moreover, we can see that "-" and "*" have the same problem, and that there is no specified precedence among these operators.

### 5.3   Precedence and associativity

These problems are easy to fix. We can just tell Elkhound the precedence and associativity of these operators, in the terminals section of [gcom.gr](examples/gcom3/gcom.gr):

    terminals {
      include("tokens.tok")
    
      precedence {
        left  20 "*";        // high precedence, left associative
        left  10 "+" "-";    // low precedence, left associative
      }
    }

Higher precedence numbers mean higher precedence, i.e. that those operators bind more tightly. The order in which the declarations appear is irrelevant; only the numbers matter. Operators with the same precedence are resolved using associativity, typically either "left" or "right". See the [manual](manual.html) (Section 7) for more information.

Now, elkhound reports no LR conflicts, which implies the grammar is unambiguous (though the converse is not true: the presence of conflicts does not guarantee ambiguity). We can see that the tree for "2 + 3 + 4" no longer contains ambiguities:

    $ echo "2 + 3 + 4" | ./parser -tree
    __EarlyStartSymbol -> AExp TOK_EOF
      AExp -> AExp TOK_PLUS AExp
        AExp -> AExp TOK_PLUS AExp
          AExp -> TOK_LITERAL
            TOK_LITERAL
          TOK_PLUS
          AExp -> TOK_LITERAL
            TOK_LITERAL
        TOK_PLUS
        AExp -> TOK_LITERAL
          TOK_LITERAL
      TOK_EOF

Precedence and associativity can only be used when the disambiguation criteria is entirely syntactic, and fairly simple at that. One of Elkhound's special features is the ability to tolerate ambiguity during parsing, letting the user defer disambiguation until it is convenient. Later on I'll demonstrate how to do this.

6\. Parse actions
-----------------

To make the program do anything besides simply recognize a language, we need to add actions to the grammar productions. Actions are written in C++, inside braces ("{" and "}") after the right-hand side (RHS). Like the rest of the grammar, actions are free-form, and can span many lines if needed.

An action can yield a _semantic value_, or sval. Svals can have whatever meaning you want. However, since they are stored internally in a single word (typically 32 bits), data that does not fit into one word must be referred to with a pointer or some other indirection mechanism. Svals are yielded by simply returning them.

Semantic values have a declared type. When the generated parser is compiled by the C++ compiler, it will check that the type matches what is actually yielded. The sval type for a nonterminal is specified by enclosing it in parentheses ("(" and ")") right after the nonterm keyword at the beginning of the nonterminal declaration. The type for a terminal is declared in the terminals section with the syntax:

> token( _type_ ) _tokenName_ ;

Actions accept svals from the actions that run for productions lower in the parse tree. Specifically, each RHS element of a rule will yield some semantic value, and that value can be used by referring to the RHS label. Labels are attached to RHS elements by preceding their symbol (terminal or nonterminal) name with an identifier and a colon (":"). Within the action body, these labels are ordinary C++ variable names, with their proper declared type.

Here is a complete example for AExp which simply evaluates the expression in the usual way (from [gcom.gr](examples/gcom4/gcom.gr)):

    nonterm(int) AExp {
      -> n:TOK_LITERAL         { return n; }
      -> a1:AExp "+" a2:AExp   { return a1 + a2; }
      -> a1:AExp "-" a2:AExp   { return a1 - a2; }
      -> a1:AExp "*" a2:AExp   { return a1 * a2; }
      -> "(" a:AExp ")"        { return a; }
    
      // interpret identifiers using environment variables; it's
      // a bit of a hack, and we'll do something better later
      -> x:TOK_IDENTIFIER {
        char const *envp = getenv(x);
        if (envp) {
          return atoi(envp);
        }
        else {
          return 0;      // not defined, call it 0
        }
      }
    }

I added a verbatim section at the top, so the identifier action could call two library functions. This gets emitted directly into the generated gcom.h file. In this case it could also have been impl_verbatim, which is only inserted into gcom.cc.

    verbatim {
      #include <stdlib.h>     // getenv, atoi
    }

Now the parser finally does some useful computation. (The last two examples assume you're using the [bash](http://www.gnu.org/software/bash/bash.html) shell. If not, adjust them to use your shell's syntax for setting environment variables.)

    $ echo "2 + 3 * 4" | ./parser
    result: 14
    
    $ echo "(2 + 3) * 4" | ./parser
    result: 20
    
    $ echo "(2 + 3) * 4 - 72" | ./parser
    result: -52
    
    $ echo "(2 + 3) * z - 72" | ./parser
    result: -72
    
    $ export a=4; echo "2 + a" | ./parser
    result: 6
    
    $ export a=4 b=6; echo "2 + a * b" | ./parser
    result: 26

The source files for this point in the tutorial are in the [examples/gcom4](examples/gcom4) directory:

* [lexer.h](examples/gcom4/lexer.h)
* [lexer.cc](examples/gcom4/lexer.cc)
* [gcom.gr](examples/gcom4/gcom.gr)
* [parser.cc](examples/gcom4/parser.cc)
* [Makefile](examples/gcom4/Makefile)

7\. Filling out the language
----------------------------

At this point, let's fill out the lexer and grammar to parse the whole GCom language. The lexer header gets some new tokens, [lexer.h](examples/gcom5/lexer.h):

    // for BExp
    TOK_TRUE,                // "true"
    TOK_FALSE,               // "false"
    TOK_EQUAL,               // "="
    TOK_LESS,                // "<"
    (etc.)

The lexer implementation gets more complicated (just an excerpt), [lexer.cc](examples/gcom5/lexer.cc):

    case '-':
      // TOK_MINUS or TOK_ARROW?
      ch = getchar();
      if (ch == '>') {
        lex->type = TOK_ARROW;
      }
      else {
        lex->type = TOK_MINUS;
        ungetc(ch, stdin);
      }
      return;

Finally, three new nonterminals are added to the grammar file, [gcom.gr](examples/gcom5/gcom.gr):

    nonterm(bool) BExp {
      -> "true"                     { return true; }
      -> "false"                    { return false; }
      -> a1:AExp "=" a2:AExp        { return a1 == a2; }
      -> a1:AExp "<" a2:AExp        { return a1 < a2; }
      -> "!" b:BExp                 { return !b; }
      -> b1:BExp TOK_AND b2:BExp    { return b1 && b2; }
      -> b1:BExp TOK_OR  b2:BExp    { return b1 || b2; }
      -> "(" b:BExp ")"             { return b; }
    }


    nonterm Stmt {
      -> "skip" {
           // no-op
         }

      -> "abort" {
           printf("abort command executed\n");
           exit(0);
         }
    
      -> "print" x:TOK_IDENTIFIER {
           char const *envp = getenv(x);
           printf("%s is %d\n", x, envp? atoi(envp) : 0);
         }
    
      -> x:TOK_IDENTIFIER ":=" a:AExp {
           // like above, use the environment variables
           putenv(strdup(stringf("%s=%d", x, a).c_str()));
         }
    
      -> Stmt ";" Stmt {
           // sequencing is automatic
         }
    
      -> "if" g:GCom "fi" {
           if (!g) {
             printf("'if' command had no enabled alternatives; aborting\n");
             exit(0);
           }
         }
    
      -> "do" GCom "od" {
           // There's no way to get the parser to loop; that's not its job.
           // For now, we'll just treat it like an 'if' that doesn't mind
           // when no alternative is enabled.  Later, we'll build a tree
           // and do this right.
         }
    }


    // a guarded command returns true if it found an enabled guard, and
    // false otherwise
    nonterm(bool) GCom {
      -> b:BExp "->" Stmt {
           // Like for 'do', there is no way to tell the parser not to
           // parse part of its input, so the statement will be executed
           // regardless of the value of 'b'.  Again, this will be fixed
           // in a later version of this example.  For now, we can at
           // least indicate whether the guard succeeded.
           return b;
         }

      -> g1:GCom "#" g2:GCom {
           return g1 || g2;
         }
    }

It behaves reasonably well, except that the control flow doesn't quite work, because the parse actions are not capable of causing the parser to skip sections of input, nor loop over the input.

    $ echo "x := 2 + 3; print x" | ./parser
    x is 5
    result: 0

    $ echo "abort" | ./parser
    abort command executed

    $ echo "skip" | ./parser
    result: 0

    $ echo "if true -> skip fi" | ./parser
    result: 0

    $ echo "if false -> skip fi" | ./parser
    'if' command had no enabled alternatives; aborting

    $ echo "if false -> skip # true -> skip fi" | ./parser
    result: 0

    $ echo "do false -> skip od" | ./parser
    result: 0

The source files for this point in the tutorial are in the [examples/gcom5](examples/gcom5) directory:

* [lexer.h](examples/gcom5/lexer.h)
* [lexer.cc](examples/gcom5/lexer.cc)
* [gcom.gr](examples/gcom5/gcom.gr)
* [parser.cc](examples/gcom5/parser.cc)
* [Makefile](examples/gcom5/Makefile)

8\. Building an AST
-------------------

To demonstrate some of the more sophisticated features of disambiguation and semantic-value handling, we need to make the example more realistic. We'll modify the parser to build an abstract syntax tree (AST), instead of evaluating the program directly. This will involve the use of another tool, [astgen](../ast/index.html), which is useful in its own right.

### 8.1   AST definition

The input to astgen consists of a sequence of class declarations. Inside each class is one or more subclass declarations, that will become nodes in the AST. The file [gcom.ast](examples/gcom/gcom.ast) begins with a few enumerations, and then has the classes:

    // arithmetic expressions
    class AExp {
      pure_virtual int eval(Env &env);
    
      -> A_lit(int n);
      -> A_var(string x);
      -> A_bin(AExp a1, AOp op, AExp a2);
    }
    
    // boolean expressions
    class BExp {
      pure_virtual bool eval(Env &env);
    
      -> B_lit(bool b);
      -> B_pred(AExp a1, BPred op, AExp a2);
      -> B_not(BExp b);
      -> B_bin(BExp b1, BOp op, BExp b2);
    }
    
    // statements
    class Stmt {
      pure_virtual void eval(Env &env);
    
      -> S_skip();
      -> S_abort();
      -> S_print(string x);
      -> S_assign(string x, AExp a);
      -> S_seq(Stmt s1, Stmt s2);
      -> S_if(GCom g);
      -> S_do(GCom g);
    }
    
    // guarded commands
    class GCom {
      // returns true if it finds an enabled alternative, false o.w.
      pure_virtual bool eval(Env &env);
    
      -> G_stmt(BExp b, Stmt s);
      -> G_seq(GCom g1, GCom g2);
    }

Then, astgen will produce two files that contain full C++ class definitions for the AST nodes. See [the ast page](../ast/index.html) for more information.

    $ ../../../ast/astgen -o ast gcom.ast
    writing ast.h...
    writing ast.cc...

### 8.2   AST-building grammar actions

Next, we modify the grammar to build these AST components, instead of evaluating the program directly; the result is [gcom.gr](examples/gcom/gcom.gr). The only slightly tricky part is that we deallocate the strings allocated by the lexer when they are consumed. The rules are otherwise straightforward:

    nonterm(AExp*) AExp {
      -> n:TOK_LITERAL                { return new A_lit(n); }
      -> x:TOK_IDENTIFIER             { return new A_var(copyAndFree(x)); }
      -> a1:AExp "+" a2:AExp          { return new A_bin(a1, AO\_PLUS, a2); }
      -> a1:AExp "-" a2:AExp          { return new A_bin(a1, AO\_MINUS, a2); }
      -> a1:AExp "*" a2:AExp          { return new A_bin(a1, AO\_TIMES, a2); }
      -> "(" a:AExp ")"               { return a; }
    }
    
    nonterm(BExp*) BExp {
      -> "true"                       { return new B_lit(true); }
      -> "false"                      { return new B_lit(false); }
      -> a1:AExp "=" a2:AExp          { return new B_pred(a1, BP_EQUAL, a2); }
      -> a1:AExp "<" a2:AExp          { return new B_pred(a1, BP_LESS, a2); }
      -> "!" b:BExp                   { return new B_not(b); }
      -> b1:BExp TOK_AND b2:BExp      { return new B_bin(b1, BO_AND, b2); }
      -> b1:BExp TOK_OR  b2:BExp      { return new B_bin(b1, BO_OR, b2); }
      -> "(" b:BExp ")"               { return b; }
    }
    
    nonterm(Stmt*) Stmt {
      -> "skip"                       { return new S_skip; }
      -> "abort"                      { return new S_abort; }
      -> "print" x:TOK_IDENTIFIER     { return new S_print(copyAndFree(x)); }
      -> x:TOK_IDENTIFIER ":=" a:AExp { return new S_assign(copyAndFree(x), a); }
      -> s1:Stmt ";" s2:Stmt          { return new S_seq(s1, s2); }
      -> "if" g:GCom "fi"             { return new S_if(g); }
      -> "do" g:GCom "od"             { return new S_do(g); }
    }
    
    nonterm(GCom*) GCom {
      -> b:BExp "->" s:Stmt           { return new G_stmt(b, s); }
      -> g1:GCom "#" g2:GCom          { return new G_seq(g1, g2); }
    }

Do not be confused by the double meaning of names like "AExp". Inside the parentheses, they refer to a C++ class name; outside, they refer to a grammar nonterminal. Since grammar nonterminal names do not automatically become type names anywhere, there is no conflict.

Digression: Notice the difference between a parse tree and an AST. In the AST, we do not need nodes for the grouping parentheses (they are only an aid to the parser), and we are free to consolidate similar nodes like the binary arithmetic expressions. In general, the AST should reflect semantics more than syntax, whereas the parse tree necessarily reflects the syntax directly. A good AST design is a key ingredient in a good language analysis program, since it serves as the communication medium between the parser and every other analysis that follows.

### 8.3   Evaluation

We're ready to write the evaluation rules as methods of the AST nodes. Those methods are declared in [gcom.ast](examples/gcom/gcom.ast) like:

    pure_virtual int eval(Env &env);

This declaration means that in class AExp, eval is pure (no definition). Further, astgen will insert declarations for eval into each of the subclasses A_lit, A_var and A_bin. Thus, we only need to implement them.

[eval.h](examples/gcom/eval.h) declares class Env, which contains variable bindings; we're not using the program's environment variables anymore.

    class Env {
    private:
      // map: name -> value
      TStringHash<Binding> map;
    
    public:
      Env();
      ~Env();
    
      int get(char const *x);
      void set(char const *x, int val);
    };

Then, [eval.cc](examples/gcom/eval.cc) has the implementations of the eval routines. Here are the evaluation rules for AExp; the others are straightforward as well:

    int A_lit::eval(Env &env)
    {
      return n;
    }
    
    int A_var::eval(Env &env)
    {
      return env.get(x.c_str());
    }
    
    int A_bin::eval(Env &env)
    {
      switch (op) {
        default:       assert(!"bad code");
        case AO_PLUS:  return a1->eval(env) + a2->eval(env);
        case AO_MINUS: return a1->eval(env) - a2->eval(env);
        case AO_TIMES: return a1->eval(env) * a2->eval(env);
      }
    }

### 8.4   Modifications to the driver

We need a few modifications to the driver, [parser.cc](examples/gcom/parser.cc). We need two more headers:

    #include "ast.h"       // Stmt, etc.
    #include "eval.h"      // Env

I've added a command-line option to print the AST (in addition to the one already there to print the parse tree):

    bool printAST  = argc==2 && 0==strcmp(argv[1], "-ast");

I changed the context class name to avoid a clash with an AST node name:

    GComContext gcom;

And finally we have the code to print and evaluate the tree. One of astgen's features is that it supplies the debugPrint method automatically.

    // result is an AST node
    Stmt *top = (Stmt*)result;
    
    if (printAST) {
      top->debugPrint(cout, 0);
    }
    
    // evaluate
    printf("evaluating...\n");
    Env env;
    top->eval(env);
    printf("program terminated normally\n");
    
    // recursively deallocate the tree
    delete top;

### 8.5   A few examples

Let's try it on a few examples!

[in1](examples/gcom/in1) just prints 5 (here I also told it to print the AST):

    $ cat in1
    x := 5;
    print x


    $ ./parser -ast <in1
    S_seq:
      S_assign:
        x = "x"
        A_lit:
          n = 5
      S_print:
        x = "x"
    evaluating...
    x is 5
    program terminated normally


[in2](examples/gcom/in2) counts to 10:

    $ cat in2
    x := 0;
    do x < 10 ->;
      print x;
      x := x + 1
    od;
    print x


    $ ./parser <in2
    evaluating...
    x is 0
    x is 1
    x is 2
    x is 3
    x is 4
    x is 5
    x is 6
    x is 7
    x is 8
    x is 9
    x is 10
    program terminated normally

[in3](examples/gcom/in3) computes the greatest common divisor of two numbers:

    $ cat in3
    x := 152;
    y := 104;
    do !(x = y) ->
      print x;
      print y;
      if x < y -> y := y - x #
         y < x -> x := x - y fi
    od;
    print x


    $ ./parser <in3
    evaluating...
    x is 152
    y is 104
    x is 48
    y is 104
    x is 48
    y is 56
    x is 48
    y is 8
    x is 40
    y is 8
    x is 32
    y is 8
    x is 24
    y is 8
    x is 16
    y is 8
    x is 8
    program terminated normally

The source files for this point in the tutorial are in the [examples/gcom](examples/gcom) directory:

* [lexer.h](examples/gcom/lexer.h)
* [lexer.cc](examples/gcom/lexer.cc)
* [gcom.gr](examples/gcom/gcom.gr)
* [parser.cc](examples/gcom/parser.cc)
* [gcom.ast](examples/gcom/gcom.ast)
* [eval.h](examples/gcom/eval.h)
* [eval.cc](examples/gcom/eval.cc)
* [Makefile](examples/gcom/Makefile)

9\. Late disambiguation
-----------------------

Now that we've got a realistic infrustructure, I can demonstrate some of the options for late disambiguation. Let's imagine that we wanted to implement precedence and associativity for AExp sometime later than parser generation time. After removing the precedence and associativity declarations for "+", "-" and "*" from [gcom.gr](examples/gcom7/gcom.gr), there are two main tasks: specify how to deallocate unused semantic values, and specify how to merge ambiguous alternatives.

### 9.1   dup and del

Anytime the grammar contains LR conflicts, the parser will pursue all possible alternatives in parallel. When an alternative fails, it tries to "undo" the actions that were executed on the failed parse branch by calling the del() function associated with the each symbol that produced a value that will now be ignored. Furthermore, when a value produced by one action is consumed by more than one action, it may be necessary to take additional action (like incrementing a reference count). The dup() function makes a copy of an sval for later use.

For terminals, dup and del are specified in the terminals section:

    token(int) TOK_LITERAL {
      fun dup(n) { return n; }     // nothing special to do for ints
      fun del(n) {}
    }
    token(char*) TOK_IDENTIFIER {
      fun dup(x) { return strdup(x); }     // make a copy
      fun del(x) { free(x); }              // delete a copy
    }

For nonterminals, these functions are included with the rest of the nonterm info:

    nonterm(AExp*) AExp {
      fun dup(a) { return a->clone(); }     // deep copy
      fun del(a) { delete a; }              // deep delete
      // ... rest of AExp ...
    }

During development, you may want to postpone dealing with dup and del. Or, you might be using a garbage collector, and all of your dup and del functions would be trivial, like for TOK_LITERAL, above. In either case, you can say

    option useGCDefaults;

at the top of the grammar file. Then, Elkhound will automatically insert trivial dup/del everywhere, to suppress the warning that is otherwise printed when such a function is called but not implemented.

### 9.2   merge

An ambiguity is a situation where some sequence of terminals can be rewritten in more than one way to yield some nonterminal. For example, in the ambiguous AExp grammar, we have

    2+3+4 -> (AExp+AExp)+AExp -> AExp+AExp -> AExp
    2+3+4 -> AExp+(AExp+AExp) -> AExp+AExp -> AExp

When this happens, the parser executes both reduction actions for AExp, and then hands the results to the merge() function associated with AExp. The merge function must return a semantic value to be used in place of the ambiguous alternatives. Furthermore, it assumes "ownership" of the original values; if one or both are not used in the result, it should deallocate them.

For AExp, we'll look inside the two alternatives, pick the one that respects the usual precedence and associativity rules, and deallocate the other one:

    fun merge(a1,a2) {
      if (precedence(a1) < precedence(a2))
        { delete a2; return a1; }         // lowest precedence goes on top
      if (precedence(a1) > precedence(a2))
        { delete a1; return a2; }
    
      // equal precedence, must be binary operators
      A_bin *ab1 = a1->asA_bin();         // cast to A_bin*
      A_bin *ab2 = a2->asA_bin();
    
      // same precedence; associates to the left; that means
      // that the RHS must use a higher-precedence operator
      if (precedence(ab1) < precedence(ab1->a2))
        { delete ab2; return ab1; }       // high-prec exp on right
      if (precedence(ab2) < precedence(ab2->a2))
        { delete ab1; return ab2; }       // high-prec exp on right
    
      printf("failed to disambiguate!\n");
      delete ab2; return ab1;             // pick arbitrarily
    }

This depends on the precedence function:

    static int precedence(AExp *a)
    {
      if (!a->isA_bin())       { return 20; }      // unary
    
      A_bin *ab = a->asA_bin();
      if (ab->op == AO_TIMES)  { return 10; }      // *
      else                     { return 0;  }      // +,-
    }

It also requires that we add an AST node to represent parenthetical grouping, since they now must be retained to participate in run-time disambiguation; see [gcom.ast](examples/gcom7/gcom.ast), [gcom.gr](examples/gcom7/gcom.gr) and [eval.cc](examples/gcom7/eval.cc).

One way to see how this is different from the way it was with precedence and associativity implemented at parser generation time is to compare the parse tree to the AST: the parse tree has the ambiguities, whereas the AST is disambiguated during construction by merge():

    $ echo "x := 2+3*4; print x" | ./parser -tree
    __EarlyStartSymbol -> Start TOK_EOF
      Start -> Stmt
        Stmt -> Stmt TOK_SEMI Stmt
          Stmt -> TOK_IDENTIFIER TOK_ASSIGN AExp
            TOK_IDENTIFIER
            TOK_ASSIGN
            --------- ambiguous AExp: 1 of 2 ---------
              AExp -> AExp TOK_TIMES AExp
                AExp -> AExp TOK_PLUS AExp
                  AExp -> TOK_LITERAL
                    TOK_LITERAL
                  TOK_PLUS
                  AExp -> TOK_LITERAL
                    TOK_LITERAL
                TOK_TIMES
                AExp -> TOK_LITERAL
                  TOK_LITERAL
            --------- ambiguous AExp: 2 of 2 ---------
              AExp -> AExp TOK_PLUS AExp
                AExp -> TOK_LITERAL
                  TOK_LITERAL
                TOK_PLUS
                AExp -> AExp TOK_TIMES AExp
                  AExp -> TOK_LITERAL
                    TOK_LITERAL
                  TOK_TIMES
                  AExp -> TOK_LITERAL
                    TOK_LITERAL
            --------- end of ambiguous AExp ---------
          TOK_SEMI
          Stmt -> TOK_PRINT TOK_IDENTIFIER
            TOK_PRINT
            TOK_IDENTIFIER
      TOK_EOF


    $ echo "x := 2+3*4; print x" | ./parser -ast
    S_seq:
      S_assign:
        x = "x"
        A_bin:
          A_lit:
            n = 2
          op = 0
          A_bin:
            A_lit:
              n = 3
            op = 2
            A_lit:
              n = 4
      S_print:
        x = "x"
    evaluating...
    x is 14
    program terminated normally

In this example, the merge() function is able to decide which alternative to keep based on local (syntactic) information. In some situations, this might not be possible. Another approach is to write merge() so that it retains _both_ alternatives, and then eval() could do disambiguation. While this tutorial does not have an example of this approach, it is used extensively in the [Elsa C++ parser](../elsa/index.md), so you can look there for further guidance.

The source files for this point in the tutorial are in the [examples/gcom7](examples/gcom7) directory:

* [lexer.h](examples/gcom7/lexer.h)
* [lexer.cc](examples/gcom7/lexer.cc)
* [gcom.gr](examples/gcom7/gcom.gr)
* [parser.cc](examples/gcom7/parser.cc)
* [gcom.ast](examples/gcom7/gcom.ast)
* [eval.h](examples/gcom7/eval.h)
* [eval.cc](examples/gcom7/eval.cc)
* [Makefile](examples/gcom7/Makefile)

Conclusion
----------

By now you know how to:

* Implement the LexerInterface that Elkhound uses
* Read and write the grammar syntax
* Write a parser driver
* Resolve ambiguities at parser-generation time with precedence and associativity
* Write grammar actions for use at parse time
* Use astgen to build an AST
* Manage copying and deallocation of semantic values during nondeterministic parsing with dup() and del()
* Resolve ambiguities at parse time with merge()

It's my hope that, with these skills and the Elkhound tool, you'll find it's easy and fun to write language analysis and translation programs. If not, you can always email me (smcpeak at cs dot berkeley dot edu) and complain!   :)
