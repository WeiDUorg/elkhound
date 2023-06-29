Elkhound Algorithm
==================

This page describes some of the details of the variant of GLR that Elkhound uses, placing it in context with some of the other GLR variants that have been proposed.

Goals
-----

First, the primary goals of Elkhound:

1.  It must have the capability to execute arbitrary user-provided reduction actions. Building a parse tree isn't good enough, it takes too much time and space.
2.  Reductions and merges should be performed bottom-up (unless the grammar is cyclic).
3.  It should be competitive with Bison for LALR (fragements of) grammars, and degrade gracefully from there. On the scale of grammar nondeterminism, from none (LALR) to some to lots, "some" is the niche Elkhound is going after.

These goals are driven by Elkhound's primary application, the Elsa C++ Parser. In essence, Elkhound came about because I wanted to apply automatic parsing technology to parsing C++, but found exiting systems inadequate.

History
-------

The approximate algorithm descendency sequence:

* Bernard Lang is typically credited with the original GLR idea:
  
    > Deterministic Techniques for Efficient Non-deterministic Parsers. 
    > Bernard Lang. 
    > Automata, Languages and Programming, Springer, 1974.
    
* Later, Tomita published the algorithm with the intent of using it for natural language processing. He popularized the term "Generalized LR Parsing", or GLR.
  
    > Efficient Parsing for Natural Language. 
    > Masaru Tomita. 
    > Int. Series in Engineering and Computer Science, Kluwer, 1985.
    
* Tomita's algorithm fails for some grammars with epsilon rules. Farshi proposed a fix involving doing a GSS search after some reductions.
  
    > GLR Parsing for epsilon-grammars. 
    > Rahman Nozohoor-Farshi.
    > In Generalized LR Parsing, Kluwer, 1991.
    
* Rekers adapted Farshi's solution for use in [ASF+SDF](http://www.cwi.nl/htbin/sen1/twiki/bin/view/SEN1/MetaEnvironment). Rekers added parse table construction to what was otherwise a recognizer. His algorithm was what I based the original Elkhound implementation on.
  
    > Parser Generation for Interactive Environments. 
    > Jan Rekers. 
    > PhD thesis, University of Amsterdam, 1992.
    
* When straightforwardly modified to execute user actions, the Rekers algorithm does not always do merges and reductions bottom-up, which makes using it in a parser much harder. Further, it is slower than Bison (by about a factor of 10) even on LALR grammars. George Necula and I remedied these deficiencies while building the Elkhound GLR parser generator.
  
    > [Elkhound: A Fast, Practical GLR Parser Generator.](http://www.cs.berkeley.edu/~smcpeak/papers/elkhound_cc04.ps) 
    > Scott McPeak and George C. Necula.
    > In Proceedings of Conference on Compiler Constructor (CC04), 2004.
    

Right Nulled GLR
----------------

At the CC04 conference I became acquainted with [Adrian Johnstone](http://www.cs.rhul.ac.uk/people/staff/johnstone.html) and his work. He and his coathors have not only more thoroughly documented the history of GLR, but also proposed a novel alternative solution to the problem Farshi originally addressed, which they call "Right Nulled GLR Parsing".

The basic idea of Right Nulled GLR is, rather than re-examining work already done to check for newly exposed reduction opportunities (as Farshi does), do those reductions that would be found by the search earlier in the parse, namely as soon as the rule in question only has nullable components remaining to recognize.

However, while this approach is certainly appealing, I still have questions about exactly how to adapt it to use user-defined reduction actions. At some point I want to implement the right-nulled variant in Elkhound to experiment more with it, but haven't gotten around to it.

Algorithm Features
------------------

The [Technical Report](http://www.cs.berkeley.edu/~smcpeak/papers/elkhound_tr.ps) has most of the details, so I'll just point out some key distinguishing characteristics here.

* **Hybrid LR/GLR.** The Elkhound Graph-Structured Stack carries sufficient information (in the form of the "deterministic depth") to tell when a given parse action (shift or reduce) can be performed using the ordinary LR algorithm. Essentially, if there isn't any nondeterminism near the stack top, LR can be used. LR actions are much faster to execute, so this leads to a big performance win (a factor of 5 to 10) when the input and grammar are mostly deterministic.

* **User-defined Actions.** In addition to the usual reduction actions (such as with Bison), Elkhound exposes actions to create and destroy semantic values (dup and del), and an action to merge ambiguous regions (merge). The user can then do whatever is desired in these actions, typically building an abstract syntax tree. In contrast, most other GLR implementations build a parse tree, which is quite expensive. (Parse trees tend to be at least 10 times larger than abstract syntax trees.)

* **Reduction Worklist.** As alluded to above, the Rekers algorithm will sometimes mix up the order of reduces and merges. The problem is with the granularity of the GSS Node worklist; it forces certain actions (all those associated with the node) to happen together, even when something else should happen in between. Elkhound solves this by maintaining a finer-grained worklist of reduction opportunties, and keeps this worklist sorted in a specific order, such that a bottom-up reduction/merge order will always be used when possible. (With a cyclic grammar, there may be a cycle in the reduction order, precluding bottom-up execution. Elkhound reports cyclic grammars to the user, but then goes ahead and parses with them.)

* **Reference-Counted GSS.** This is mostly a minor detail, but using reference counting in the GSS produces much better locality than garbage collection alone (and manual deallocation is impossible due to the nature of the algorithm).

* **C++ and OCaml Interfaces.** Though not a feature of the parsing algorithm per se, a nice feature is the ability to let the user write actions in either C++ or OCaml. There are actually two parser core impementations, a C++ implementation and an OCaml implementation, and the user simply links Elkhound's output with the appropriate parser core.