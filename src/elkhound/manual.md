Elkhound Manual
===============

This page describes the input format for grammars for the Elkhound parser generator, and the features of that generator.

Related pages:

* [Elkhound overview](index.md)
* [Elkhound tutorial](tutorial.md)

If you'd like to look at a simple grammar while reading this description, see [examples/arith/arith.gr](examples/arith/arith.gr), a parser for simple arithmetic expressions.

Contents
--------

* [1\. Lexical structure](#1\.-lexical-structure)
* [2\. Context Class](#2\.-context-class)
* [3\. Terminals](#3\.-terminals)
    * [3.1 Token Types](#3.1-token-types)
    * [3.2 dup/del](#3.2-dup/del)
    * [3.3 classify](#3.3-classify)
    * [3.4 Precedence/associativity specifications](#3.4-precedence/associativity-specifications)
    * [3.5 Lexer Interface](#3.5-lexer-interface)
* [4\. Nonterminals](#4\.-nonterminals)
    * [4.1 dup](#4.1-dup)
    * [4.2 del](#4.2-del)
    * [4.3 merge](#4.3-merge)
    * [4.4 keep](#4.4-keep)
    * [4.5 precedence](#4.5-precedence)
    * [4.6 forbid_next](#4.6-forbid_next)
* [5\. Options](#5\.-options)
    * [5.1 useGCDefaults](#5.1-useGCDefaults)
    * [5.2 defaultMergeAborts](#5.2-defaultMergeAborts)
    * [5.3 Expected conflicts, unreachable symbols](#5.3-expected-conflicts,-unreachable-symbols)
    * [5.4 allow\_continued\_nonterminals](#5.4-allow_continued_nonterminals)
* [6\. OCaml](#6\.-ocaml)
* [7\. Precedence and Associativity](#7\.-precedence-and-associativity)
    * [7.1 Meaning of prec/assoc specifications](#7.1-meaning-of-prec/assoc-specifications)
    * [7.2 Attaching prec/assoc to tokens and productions](#7.2-attaching-prec/assoc-to-tokens-and-productions)
    * [7.3 Conflict resolution with prec/assoc specifications](#7.3-conflict-resolution-with-prec/assoc-specifications)
    * [7.4 Example: Arithmetic grammar](#7.4-example:-arithmetic-grammar)
    * [7.5 Example: Dangling else](#7.5-example:-dangling-else)
    * [7.6 Further directions](#7.6-further-directions)

1\. Lexical structure
---------------------

The grammar file format is free-form, meaning that all whitespace is considered equivalent. In the C tradition, grouping is generally denoted by enclosing things in braces ("{" and "}"). Strings are enclosed in double-quotes ("").

Grammar files may include other grammar files, by writing include("other\_file\_name").

Comments can use the C++ "//" syntax or the C "/*\*/" syntax.

2\. Context Class
-----------------

The parser's action functions are all members of a C++ context class. As the grammar author, you must define the context class. The class is introduced with the "context_class" keyword, followed by ordinary C++ syntax for classes (ending with "};");

3\. Terminals
-------------

The user must declare of all the tokens, also called terminals. A block of terminal declarations looks like:

    terminals {
      0 : TOK_EOF;
      1 : TOK_NUMBER;              // no alias
      2 : TOK_PLUS     "+";        // alias is "+" (including quotes)
      3 : TOK_MINUS    "-";
      4 : TOK_TIMES    "*";
      5 : TOK_DIVIDE   "/";
      6 : TOK_LPAREN   "(";
      7 : TOK_RPAREN   ")";
    }

Each statement gives a unique numeric code (e.g. 3), a canonical name (e.g. TOK\_MINUS), and an optional alias (e.g. "-"). Either the name or the alias may appear in the grammar productions, though the usual style is to use aliases for tokens that always have the same spelling (like "+"), and the name for others (like TOK\_NUMBER).

Normally it's expected the tokens will be described in their own file, and the [make-tok](make-tok) script will create the token list seen above.

### 3.1 Token Types

In addition to declaring the numeric codes and aliases of the tokens, the user must declare types for semantic values of tokens, if those values are used by reduction actions (specifically, if their occurence on a right-hand-side includes a label, denoted with a colon ":").

The syntax for declaring a token type is

> token(_type_) _token_name_;

or, if specifying terminal functions,

> token(_type_) _token_name_ {  
>       _terminal_functions_  
> }

The terminal functions are explained in the next sections.

### 3.2 dup/del

Terminals can have dup and del functions, just like nonterminals. See [below](#dup) for more information.

### 3.3 classify

In some situations, it is convenient to be able to alter the classification of a token after it is yielded by the lexer but before the parser sees it, in particular before it is compared to lookahead sets. For this purpose, each time a token is yielded from the lexer, it is passed to that token's classify() function. classify accepts a single argument, the semantic value associated with that token. It returns the token's new classification, as a token id. (It cannot change the semantic value.)

The main way it differs from simply modifying the lexer is that the classify function has access to the parser context class, whereas the lexer presumably does not. In any case, it's something of a hack, and best used sparingly.

As a representative example, here is the classify function from [c/c.gr](c/c.gr), used to implement the lexer hack for a C parser:

    token(StringRef) L2_NAME {
      fun classify(s) {
        if (isType(s)) {
          return L2_TYPE_NAME;
        }
        else {
          return L2_VARIABLE_NAME;
        }
      }
    }

### 3.4 Precedence/associativity specifications

Inside a block that begins with "precedence {" and ends with "}", one may list directives of the form

> _dir_     _num_     _tok..._     ;

where _dir_ is a precedence direction, _num_ a precedence number, and _tok..._ a sequence of tokens (either token names or aliases). The effect is to associate _dir_/_num_ with each listed token. The meaning of such specifications is explained in Section 7, below.

### 3.5 Lexer Interface

The terminals specification given in the grammar file is a description of the output of the lexer. This section describes the mechanism by which the lexer provides that output.

The parser interacts with the lexer through the API defined in the [lexerint.h](lexerint.h) file. The lexer must implement (inherit from) LexerInterface. When the parser asks the lexer for a token by invoking its NextTokenFunc, the lexer describes the current token by filling in the three data fields of LexerInterface:

* type: The token code as defined in the terminals list (Section 3).
* sval: The semantic value, a value of the type given in the Token Types specification (Section 3.1) for the token whose code is in type.
* loc: The source location of the start of the current token. You only need to fill this in if you plan on using the value of loc from within reduction action functions.

The parser will then do its work by reading these fields, and when it is ready for the next token, will invoke the NextTokenFunc again.

> **Rationale:** The rationale for this very imperative style of interface is performance. It's faster to share storage between the parser and lexer than to have the parser make a separate copy of this information. Since token retrieval is in the inner loop of the parser, saving just a few instructions can be significant.

The actual mechanism for invoking NextTokenFunc is a little convoluted. The parser first calls getTokenFunc(), a virtual method, which returns a function pointer. This pointer is stored in the parser when it first starts up. The parser then invokes this function pointer each time it wants a new token, passing a pointer to the LexerInterface as an argument.

This means that the lexer must define _two_ functions, a static method (or a nonmember function) that does token retrieval, and a virtual method that just returns a pointer to the first function.

> **Rationale**: Again, performance is the main consideration. The cost of a virtual method dispatch is two memory reads plus an indirect function call (with data dependencies separating each step), whereas calling a function pointer requires only the indirect call. Originally, Elsa used a virtual function dispatch, and switching to this approach produced a significant speedup (I no longer remember the exact numbers, I'd guess it was 5-10%).

Finally, there are two functions, tokenDesc and tokenKindDesc, that are used for diagnostic purposes. When there is a parse error, the parser will call these functions to obtain descriptions of tokens for use in the error message.

4\. Nonterminals
----------------

Following the terminals, the bulk of the grammar is one or more nonterminals. Each nonterminal declaration specifies all of the productions for which it is the left-hand-side.

A simple nonterminal might be:

    nonterm(int) Exp {
      -> e1:Exp "+" e2:Exp        { return e1 + e2; }
      -> n:TOK_NUMBER             { return n; }
    }

The type of the semantic value yielded by the productions is given in parentheses, after the keyword "nonterm". In this case, int is the type. The type can be omitted if productions do not yield interesting semantic values.

In the example, Exp has two productions, Exp -> Exp "+" Exp and Exp -> TOK_NUMBER. The "->" keyword introduces a production.

Right-hand-side symbols can be given names, by putting the name before a colon (":") and the symbol. These names can be used in the action functions to refer to the semantic values of the subtrees (like Bison's $1, $2, etc.). Note that action functions return their value, as opposed to (say) assigning to $$.

There are four kinds of nontermial functions, described below.

### 4.1 dup

Because of the way the GLR algorithm operates, a semantic value yielded (returned) by one action may be passed as an argument to _more than one_ action. This is in contrast to Bison, where each semantic value is yielded exactly once.

Depending on what the actions actually do, i.e. what the semantic values actually mean, the user may need to intervene to help manage the sharing of semantic values. For example, if the values form a tree where memory is managed by reference counting, then the reference count of a value would need to be increased each time it is yielded.

The dup() nonterminal function is intended to support the kind of sharing management alluded to above. Each time a semantic value is to be passed to an action, it first is passed to the associated dup() function. The value returned by dup() is stored back in the parser's data structures, for use the next time the value must be passed to an action. Effectively, by calling dup(), the parser is announcing, "I am about to surrender this value to an action; please give me a value to use in its place next time."

Common dup() strategies:

* Return NULL. This is the default. This expresses the intent that the value _not_ be passed more than once (since any subsequent actions will receive NULL arguments).
* Return the argument. This is the default if the useGCDefaults option is specified. This is useful when the semantic values can be shared arbitrarily without special handling.
* Increment a reference count, make a deep copy, etc. Various possibilities exist depending on the particular sharing management in use.

### 4.2 del

A natural counterpart to dup(), del() accepts values that are not going to be passed to any more actions (this happens when, for example, one of the potential parsers fails to make further progress). It does not return anything.

Common del() strategies:

* Print a warning. This is the default, since an unhandled del() may be the cause of a memory leak, depending on the memory management strategy in use.
* Do nothing. This is the default if the useGCDefaults option is specified.
* Decrement a reference count, deallocate, etc. Depending on the programmer's intended memory management scheme, the value passed to del() can be treated appropriately.

### 4.3 merge

An _ambiguity_ is the condition when a single sequence of tokens can be parsed as some nonterminal in more than one way. During parsing, when an ambiguity is encountered, the semantic values from the different parses are passed to the nonterminal's merge() function, two at a time.

Merge accepts two competing semantic value arguments, and returns a semantic value that will stand for the ambiguous region in all future reductions. Both the arguments and the return value have the type of the nonterminal's usual semantic values.

If there are more than two parses, the first two will be merged, the result of which will be merged with the third, and so on until they are all merged. At each step, the first argument is the one that may have resulted from a previous merge(), and the second argument is not (unless it is the result of merging from further down in the parse forest).

Common merge() strategies:

* Print a message to the effect that the ambiguity is unexpected, and return one of the arguments arbitrarily. This is the default behavior.
* Abort the program. This is a more severe response than the first one, and is the default if the defaultMergeAborts option is specified.
* Examine the two semantic values, and apply some disambiguation criteria to choose which to retain. Then return only the retained value. Note that merge() is being given exclusive right to access both values; if it chooses to only return one of them for future reductions, then the other should probably be treated similarly to calling del() (though in fact, literally calling del() is not possible in the current implementation).
* Create some explicit representation of the ambiguity. This is how the Elsa C++ parser handles most of the ambiguities in its C++ grammar, due to the type/variable ambiguity.

**Note that ambiguity is different from a reduce/reduce conflict.** A reduce/reduce conflict happens when the top few symbols of the parse stack can be reduced by two different rules. It is not (necessarily) an ambiguity, as the reduced input portions are different. In merge(), the merging fragments arise from reductions applied to the \*same\* sequence of ground terminals.

For example, the following unambiguous(!) grammar

    S -> B a c | b A a d
    B -> b a a
    A -> a a

has a reduce/reduce conflict because after seeing "baa" with "a" in the lookahead, the parser cannot tell whether to reduce the topmost "aa" to A, or the entire "baa" to B, since it only has one token of lookahead and can't see whether the token after the lookahead is "c" or "d".

On the other hand, the ambiguous grammar

    S -> A b | a b
    A -> a

contains no reduce/reduce conflicts. The only conflict is a shift/reduce, as after seeing "a" with "b" as lookahead the parser cannot tell whether to reduce "a" to A, or shift the "b" and later reduce the combination directly as S.

Conceptually, if you imagine a nondeterministic LALR parsing algorithm, conflicts are split (choice) points and ambiguities are join points. You cannot have a join without a split, but there is no easy way (in fact it's undecidable) to compute a precise relationship between splits and possible future joins. Both shift/reduce and reduce/reduce are split points, whereas merge() is a join.

### 4.4 keep

Sometimes, a potential ambiguity can be prevented if a semantic value can be determined to be invalid in isolation (as opposed to waiting to see a competing alternative in merge()). To support such determination, each nonterminal can have a keep() function, which returns true if its semantic value argument should be retained (as usual) or false if its argument should be suppressed, as if the reduction never happened.

If keep returns false, the parser does _not_ call del() on that value; it is regarded as disposed by keep.

Common keep strategies:

* Always return true. This is the default.
* Look at the argument, and return false if for some reason it should be discarded, particularly if it will otherwise lead to an ambiguity. This is the intended usage.
* As a variation of the above, the reduction action itself can make the determination of suitability, and return a special value like NULL if it wants to cancel the reduction. Then, keep() simply checks for the special value. In fact, since keep() is a somewhat expensive option in terms of performance, I am considering eliminating keep() in favor of a built-in NULL check.

### 4.5 precedence

A rule can be annotated with an explicit precedence specification, for example:

    nonterm N {
      -> A B C     precedence("+")   { /*...*/ }
    }

This specification has the effect of assigning the rule "N -> A B C" the same precedence level as the terminal "+". See [Section 7](#prec_assoc) for more information on what this does.

### 4.6 forbid_next

A rule can also be annotated with one or more _forbidden lookahead_ declarations, for example:

    nonterm N {
      -> A B C     forbid\_next("+") forbid\_next("*")  { /*...*/ }
    }

This specification means that the rule "N -> A B C" can not be used to reduce if the next symbol is either "+" or "*".

5\. Options
-----------

A number of variations in parser generator behavior can be requested through the use of the option syntax:

> option _option_name_;

or, for options that accept an argument:

> option _option_name_ _option_argument_;

The _option_name_ is an identifier from among those listed below, and _option_argument_ is an integer.

Option processing is implemented in [grampar.cc](grampar.cc), function astParseOptions. The various options are described in the following sections.

### 5.1 useGCDefaults

The command

    option useGCDefaults;

instructs the parser generator to make the tacit assumption that sharing management is automatic (e.g. via a garbage collector), and hence set the default terminal and nonterminal functions appropriately.

In fact, most users of Elkhound will probably want to specify this option during initial grammar development, to reduce the amount of specification needed to get started. The rationale for not making useGCDefaults the global default is that users should be aware that the issue of sharing management is being swept under the carpet.

### 5.2 defaultMergeAborts

The command

    option defaultMergeAborts;

instructs the parser generator that if the grammar does not specify a merge() function, the supplied default should print a message and then abort the program. This is a good idea once it is believed that all the ambiguities have been handled by merge() functions.

### 5.3 Expected conflicts, unreachable symbols

Nominally, the parser generator expects there to be no shift/reduce and no reduce/reduce conflicts, and no unreachable (from the start symbol) symbols. Of course, the whole point of using GLR is to allow conflicts, but it is still generally profitable to keep track of how many conflicts are present at a given stage of grammar development, since a sudden explosion of conflicts often indicates a grammar bug.

So, the user can declare how many conflicts of each type are expected. For example,

    option shift\_reduce\_conflicts 40;
    option reduce\_reduce\_conflicts 30;

specifies that 40 S/R conflicts and 30 R/R conflicts are expected. If the parser generator finds matching statistics, it will suppress reporting of such statistics; if there is a difference, it will be reported.

Similarly, one can indicate the expected number of unreachable syhmbols (this usually corresponds to a grammar in development, where part of the grammar has been deliberately disabled by making it inaccessible):

    option unreachable_nonterminals 3;
    option unreachable_terminals 2;

### 5.4 allow\_continued\_nonterminals

By default, Elkhound will complain if there is more than one definition of a given nonterminal. However, if you say

    option allow\_continued\_nonterminals;

then Elkhound will treat input like

    nonterm(type) N {
      -> A ;
    }
    nonterm(type) N {
      -> B ;
    }

as equivalent to

    nonterm(type) N {
      -> A ;
      -> B ;
    }

The nonterminal types, if specified, must be identical.

Since Elkhound just concatenates the bodies, specification functions (like merge()) must be given at most once across all continuations, and they apply to the whole (concatenated) nonterminal.

This feature is mostly useful for automatically-generated grammars, particularly those created by textually combining elements from two or more human-written grammars.

6\. OCaml
---------

By default, Elkhound generates a parser in C++. By specifying "-ocaml" on the command line, the user can request that instead the parser generate OCaml code. Please see [ocaml/](ocaml/), probably starting with the example driver [ocaml/main.ml](ocaml/main.ml).

The lexer interface for OCaml is given in [ocaml/lexerint.ml](ocaml/lexerint.ml). It is similar to the C++ interface except I have not yet (2005-06-13) implemented support for source location propagation, so there is no loc field. Also, since OCaml does not have function pointers, it uses an ordinary virtual dispatch to get the next token.

7\. Precedence and Associativity
--------------------------------

Precedence and associativity ("prec/assoc") declarations are a technique for statically resolving conflicts. They are often convenient, but can also be confusing.

Precisely understanding prec/assoc requires some familiarity of the details of LR parsing, in particular the difference between "shift" and "reduce". The following description assumes the reader is familiar with these concepts. If you are not, Section 2 of the [Elkhound technical report](http://www.cs.berkeley.edu/~smcpeak/elkhound/elkhound.ps) contains a brief description.

### 7.1 Meaning of prec/assoc specifications

A prec/assoc specification is a precedence number, and an associativity direction. Precedence numbers are integers, where larger integers have "higher precedence". An associativity direction is one of the following:

| Name | Mnemonic | Description |
| --- | --- | --- |
| left | left associative | Resolve shift/reduce by reducing. |
| right | right associative | Resolve shift/reduce by shifting. |
| nonassoc | non associative | "Resolve" shift/reduce by deleting _both_ possibilities, consequently making associative uses into parse-time syntax errors. |
| prec | specify only precedence | This directive asserts that a grammar does not have a conflict that would require using the associativity direction; if it does, it is a parser-generation-time error. |
| assoc_split |     | Do not resolve the conflict; the GLR algorithm will split the parse stack as necessary. |

### 7.2 Attaching prec/assoc to tokens and productions

Each token can be associated with a prec/assoc specification. By default, a token has no prec/assoc specification. However, a specification can be attached via the precedence block of the terminals section of the grammar file (see Section 3.4, above).

Each production can be associated with a precedence number. If a production includes any terminals among its RHS elements, then the production inherits the precedence number of the _rightmost_ terminal in the RHS; if the rightmost terminal has no prec/assoc specification, then the rule has no precedence. The precedence number can also be explicitly supplied by writing

> precedence ( _tok_ )

at the end of the RHS (just before the action). This will use the precedence of _tok_ instead of the rightmost terminal.

### 7.3 Conflict resolution with prec/assoc specifications

The resulting token and rule prec/assoc info is used during parser generation (only), to resolve shift/reduce and reduce/reduce conflicts. "Resolving" a conflict means removing one of the possible actions from the parse tables. It is implemented in GrammarAnalysis::resolveConflicts().

When a shift/reduce conflict is encountered between token and a rule, if the token has higher precedence, then a shift is used, otherwise a reduce is used. If they have the same precedence, then the associativity direction of the token is consulted, using the resolution procedures described in the table above.

When a reduce/reduce conflict is encountered, the highest precedence rule is chosen.

In both cases, if either conflicting entity does not have any prec/assoc specification, then no resolution is done, and the GLR algorithm will split the parse stack at parse time as necessary.

### 7.4 Example: Arithmetic grammar

The canonical example of using prec/assoc is to give a grammar for arithmetic expressions, using an ambiguous grammar (for convenience) and prec/assoc to resolve the ambiguities. The grammar is typically (e.g., [arith.gr](examples/arith/arith.gr)) something like

    nonterm Expr {
      -> Expr "+" Expr ;
      -> Expr "-" Expr ;
      -> Expr "*" Expr ;
      -> Expr "/" Expr ;
      -> TOK_NUMBER ;
    }

with a precedence specification like

    precedence {
      left 20 "*" "/";
      left 10 "+" "-";
    }

Consider parsing the input "1 + 2 * 3". This should parse like "1 + (2 * 3)". The crucial moment during the parsing algorithm comes when the parser sees the "*" in its lookahead. At that moment, the parse stack looks like

    Expr(2)                       <-- top of stack (most recently shifted)
    +
    Expr(1)                       <-- bottom of stack

If the parser were to _reduce_, via the rule "Expr -> Expr + Expr", then the stack would become

    Expr(Expr(1)+Expr(2))         <-- top/bottom of stack

which means the final parse would be "(1 + 2) * 3", which is wrong.

Alternatively, if the parser were to _shift_, the stack becomes

    *                             <-- top of stack
    Expr(2)
    +
    Expr(1)                       <-- bottom of stack

Then after one more shift, we have

    Expr(3)                       <-- top of stack
    *
    Expr(2)
    +
    Expr(1)                       <-- bottom of stack

and the only choice is to reduce via "Expr -> Expr * Expr", leading to the configuration

    Expr(Expr(2)*Expr(3))         <-- top of stack
    +
    Expr(1)                       <-- bottom of stack

and one more reduce yields

    Expr( Expr(1) + Expr(Expr(2)*Expr(3)) )    <-- top/bottom

which is the desired "1 + (2 * 3)" interpretation.

The parsing choice above is a _shift/reduce conflict_; the parser must choose between shifting "*" and reducing via "Expr -> Expr + Expr". Now, the prec/assoc spec says that "*" has higher precedence than "+", and the rule is (by default) given the precedence of the latter. Consequently, the shift is chosen, so that the "*" will ultimately be reduced before the "+".

One can do similar reasoning for the case of "1 + 2 + 3", which should parse as "(1 + 2) + 3", and does so because "+" is declared to be left-associative: reduces are preferred to shifts (reduce early instead of reducing late).

### 7.5 Example: Dangling else

As another example of using prec/assoc to resolve grammar ambiguity, we can consider the classic "dangling else" problem. Many languages, such as C, have two rules for the "if" statement:

    nonterm Stmt {
      -> "if" "(" Expr ")" Stmt ;
      -> "if" "(" Expr ")" Stmt "else" Stmt ;
      ...
    }

This grammar is ambiguous; for example, the input

    if (P)   if (Q)   a=b;   else   c=d;

could be parsed as

    if (P) {
      if (Q)
        a=b;
      else        // associated with inner "if"
        c=d;
    }

or as

    if (P) {
      if (Q)
        a=b;
    }
    else          // associated with outer "if"
      c=d;

As with all uses of prec/assoc, it is possible to resolve the ambiguity by modifying the grammar. (Actually, I'm not 100% sure about this fact, especially because of LALR vs LR. But it seems to be true in practice.) However, in this case, doing so means duplicating the entire Stmt nonterm, creating one case for when it is the first thing in the then-block of an "if" statement with an "else" clause (in which case the version missing an "else" is not allowed), and another case for when it is not. Such savagery to the grammar is usually unacceptable.

There is an easy fix with prec/assoc, however: simply declare "else" to be right-associative, and (explicitly) give both "if" rules the precedence of the "else" token:

    terminals {
      precedence {
        right 100 "else";     // precedence number irrelevant
      }
    }
    ...
    nonterm Stmt {
      -> "if" "(" Expr ")" Stmt                   precedence("else");
      -> "if" "(" Expr ")" Stmt "else" Stmt       precedence("else");
      ...
    }

At the crucial moment, the lookahead is "else" and the stack is

    Stmt(a=b;)                          <-- top
    ")"
    Expr(Q)
    "("
    "if"
    ")"
    Expr(P)
    "("
    "if"                                <-- bottom

The conflict is between shifting "else" and reducing via "Stmt -> if ( Expr ) Stmt". Since the precedence of both the token and the rule is the same, the associativity direction of the token is consulted. That direction is "right", which means to reduce late, i.e. shift. After shifting "else", then the rest of the input, and reducing "c=d;", we have

    Stmt(c=d;)                          <-- top
    "else"
    Stmt(a=b;)
    ")"
    Expr(Q)
    "("
    "if"
    ")"
    Expr(P)
    "("
    "if"                                <-- bottom

Finally, the algorithm reduces via "Stmt -> if ( Expr ) Stmt else Stmt", yielding

    If(Q, Stmt(a=b;), Stmt(c=d;))       <-- top
    ")"
    Expr(P)
    "("
    "if"                                <-- bottom

and one more reduce yields

    If(P,   If(Q, Stmt(a=b;), Stmt(c=d;))  )

which corresponds to binding the "else" to the innermost "if".

### 7.6 Further directions

As illustrated in the "dangling else" example, prec/assoc specifications need not involve "operators" with a classic "precedence order". Arguably, such cases constitute a hack, wherein the grammar designer takes advantage of knowledge of the parsing algorithm in use. It would perhaps be better if there were some kind of parsing-algorithm-neutral specification technique that could subsume LR prec/assoc, but be translated into LR prec/assoc if that is the algorithm in use. But I do not know of such a technique. (At one point I played with using attribute grammars and planned some static analysis to figure out when an attribute grammar's inherited attributes could be replaced with an equivalent prec/assoc spec, but it turned out to be hard so I abandoned the attempt.)

Not every use of prec/assoc is to resolve an actual grammatical ambiguity. It is possible for the grammar to be unambiguous, but still have a conflict, in which case a prec/assoc specification may be a good way to resolve it, if performance is a concern.

I think the best advice is: if you are not very familiar with LR parsing, ignore prec/assoc. Either write an unambiguous grammar, or use Elkhound's merge feature to do disambiguation; in either case, the conflicts only affect performance and so can be ignored. If you _are_ familiar with LR parsing, then use your judgment as to whether a given situation is right for prec/assoc. They can seem appealing since they yield fast parsers, but it can be difficult to determine the actual language accepted by an LR parser with prec/assoc.

