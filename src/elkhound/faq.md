Elkhound Frequently Asked Questions
===================================

This page has answers to questions that I've received and think might be useful generally.

Contents
--------

*   [1. Reduction Actions](#1\.-reduction-actions)
    *   [1.1 Does Elkhound ever run action routines on parses that are not part of a complete parse tree?](#1.1-does-elkhound-ever-run-action-routines-on-parses-that-are-not-part-of-a-complete-parse-tree?)
    *   [1.2 What if a reduction has a side effect?](#1\.2-what-if-a-reduction-has-a-side-effect?)
*   [2. Starting the Parser](#2\.-starting-the-parser)
    *   [2.1 How do I start parsing from a symbol other than the start symbol?](#2.1-how-do-i-start-parsing-from-a-symbol-other-than-the-start-symbol?)

1\. Reduction Actions
---------------------

### 1.1 Does Elkhound ever run action routines on parses that are not part of a complete parse tree?

Yes, it does. The question of whether a given reduction can be part of a complete parse tree can, in general, only be answered by seeing all of the input. So the two choices are to reduce as you go, which is what Elkhound does, or to build a parse tree/forest and then run all reductions after parsing is complete. Elkhound does not do the latter because building a parse tree slows parsing by a large (~10) constant factor.

In certain circumstances, Elkhound can determine that an action corresponds to a parse that is going to fail before processing of the current token is finished (see [glr.cc](glr.cc), GLR::canMakeProgress()). In such cases, the action is preempted, but this is just a quick hack that can slightly improve performance.

### 1.2 What if a reduction has a side effect?

Original question:

> We have a scenario where an action routine is run on part of an expression - this part is legal on its own, but the subdivision is illegal. The problem is that the action routine raises an exception due to context-sensitive invariants.

This is an instance of a more general problem: actions that have side effects. Elkhound will call the del function corresponding to reductions that are performed and then discarded, but if the action had a side effect then undoing it in del can be very difficult. Therefore you have to be careful about using side effects. Throwing an exception is essentially an extreme form of side effect from Elkhound's point of view.

Probably the easiest solution is to delay the activity that leads to throwing the exception. The reduction action can build a record of the reduction, but then postpone further processing until another pass, after parsing is complete. As long as this doesn't happen too often, you won't incur the cost of a complete parse tree.

Another possibility is to examine the parse tables and see if a grammar modification can eliminate the nondeterminism that leads to the parse split. This of course requires detailed understanding of how the parsing algorithm works; it is probably only worth it if performance is crucial.

2\. Starting the Parser
-----------------------

### 2.1 How do I start parsing from a symbol other than the start symbol?

Sometimes people want to use a single Elkhound specification, but have it start parsing with different symbols in different situations. Eventually I'd like to build in a feature to do this automatically, but in the meantime there is a trick that will work.

Suppose you have an Elkhound grammar with several nonterminals you want to be able to use as start symbols:

    nonterm(Foo*) Foo { -> A B C {/*...*/} /*...*/ }
    nonterm(Bar*) Bar { /*...*/ }
    nonterm(Baz*) Baz { /*...*/ }

You can create a wrapper nonterminal, which (when put first in the grammar file) Elkhound will regard as the "real" start symbol, and give it rules whose first symbol is a special token that selects the start symbol:

    nonterm(void*) WrapperStart {
      -> TOK_SELECT_FOO f:Foo { return f; }
      -> TOK_SELECT_BAR b:Bar { return b; }
      -> TOK_SELECT_BAZ b:Baz { return b; }
    }

Now, these special tokens (e.g. TOK\_SELECT\_FOO) are declared like ordinary tokens but have the property that they are never yielded by the lexer. Instead, you manually provide them as the first token when initializing the parser. This is done instead of the usual "priming step"; the parser engine will read the first real token once it drops into its normal parsing loop:

    Lexer lexer;                   // your implementation of LexerInterface
    lexer.type = TOK_SELECT_FOO;   // you pick what to provide here
    SemanticValue treeTop;
    glr.glrParse(lexer, treeTop);
    Foo *foo = (Foo*)treeTop;      // then interpret the result accordingly

Above, I've used void\* for convenience. If you're bothered by the lack of type safety, or using the OCaml version, you can use some kind of tagged union type. The union only has to carry the final parse result from the WrapperStart symbol out to the caller, so it is short lived and irrelevant to performance.