AST Manual
==========

astgen is a simple tool for creating C++ data type descriptions of heterogenous tree structures. "ast" comes from the common use of heterogeneous trees to make abstract syntax trees for compilers. This page is its documentation. [Another page](index.html) describes the module structure of its implementation.

Fair warning: The astgen input language was developed "bottom up", adding features and wrinkles as needs arose. Consequently, it is fairly concise in practice, but sort of awkard to describe. There are several instances of incomplete orthogonalization ("irregularities"). It's probably time for a redesign; but for now it is what it is.

1\. Input Description
---------------------

astgen input files are free-form (all whitespace is treated the same), like C++ itself, and the syntax is inspired by C++ in other ways as well. In particular, Emacs' c++-mode works fine for highlighting.

For an example ast description, see [ast.ast](ast.ast).

The input file is a sequence of three kinds of things:

*   verbatim code to be copied to the output
*   options
*   tree class definitions

### 1.1 Verbatim

Verbatim code is exactly what it sounds like: a string that gets copied to either the generated header file (for "verbatim") or the generated C++ implementation file (for "impl\_verbatim"). The verbatim code is delimited by braces ("{}").

### 1.2 Options

There are currently three options:

*   visitor: Emit code for traversal using a visitor. See [Section 3](#3\.-Visitor-Interface).
*   gdb: To make it easier to call debugPrint() from with a debugger (such as gdb), emit methods called gdb() that just call debugPrint(cout,0).
*   xmlPrint: Emit xmlPrint methods, similar to the debugPrint methods. The current xmlPrint is just a prototype and isn't used; improving it is still on the todo list.

### 1.3 Tree Class Definitions

A tree class definition begins with the keyword "class", then the class name, then an optional constructor (hereafter: "ctor") argument list, then a brace-delimited body:

    class MyClass (int arg1, AnotherClass arg2) {
      // ... the body ...
    }

If there are no ctor arguments, either use "()" or leave out the parentheses entirely.

If there is nothing to put in the class body, you can abbreviate "{}" as ";". Note that if you put the braces, there is *not* a semicolon following the "}".

#### 1.3.1 Constructor ("ctor") Arguments

Ctor arguments play two roles. First, they become parameters to the generated class' constructor function. Thus, (above) any time a MyClass is constructed, the caller has to supply two arguments of the given types. Ctor arguments may be given default values using the usual C++ syntax, in which case those become default values for the associated constructor parameters.

Second, ctor arguments become fields in the generated class, and those fields are initialized by the constructor call. So a MyClass has (at least) two fields, an int and an AnotherClass.

Actually, the above is a bit of a lie, if AnotherClass is another one of the tree classes defined in the same astgen input file. astgen recognizes several special forms of ctor argument types, and each has slightly different semantics. In each case below, "A" is the name of a class defined elsewhere in the astgen input file (or one of the extensions it has been combined with, see [Section 2](#2\.-Extension-Modules)).

**Tree:** If the type is "A", then the constructor argument and class field are both of type "A\*" (pointer to A). Further, the class is regarded as the owner of this pointer, and thus will deallocate it in its destructor.

**Tree pointer:** If the type is "A\*", then the argument and field are both "A\*", and the pointer is non-owning. However, astgen recognizes that it knows how to traverse into such a field, which comes into play during visiting.

**ASTList:** If the type is "ASTList\<A>" (see [smbase/astlist.h](../smbase/astlist.h)), then the constructor argument becomes "ASTList\<A>\*" and the field is "ASTList\<A>". ASTList has a constructor which accepts a pointer to another ASTList, and _deallocates_ the argument list, taking ownership of the argument list's elements. This makes it possible to create an ASTList on the heap, pass it around as a simple pointer, and then consume it by passing to a class with an ASTList-typed ctor arg.

**FakeList:** If the type is "FakeList\<A>" (see smbase/fakelist.h), then the constructor argument and class field are both "FakeList\<A>\*". This is really just a pointer to an A, but the class is considered to own the whole list, not just the first element.

**Anything else:** For any other type T, the argument is type T and the field is type T. astgen doesn't do anything special since it assumes it doesn't know how to interact with the type.

#### 1.3.2 Fields

Classes can be given fields (and in fact methods) that astgen doesn't interpret. These don't become part of the constructor parameter list, so they should either have or be given default values. Fields are introduced with one of the keywords "public", "private", or "protected", and the end with a semicolon. Semicolons can appear in the field text, as long as they're bracketed by braces, parentheses, or brackets (the lexer counts nested delimiters when looking for the final ";").

The keywords "public", "private", and "protected" are all treated the same way: the output class will contain the field text, prepended with "public:" (or whatever the keyword was). Syntactically they work similarly to Java class fields.

Optionally, the introducer keyword can be immediately followed (no whitespace) by a comma-separated list of field modifiers, in parentheses. The field modifiers are:

*   field: Signals that the member is a data field, which currently only means that it is printed during debugPrint along with the constructor arguments.
*   func: Indicates that the declaration is a function; this is rarely needed.
*   owner: For pointers, this means that the destructor should delete the object pointed to.
*   virtual: First, adds the "virtual" keyword to the emitted declaration. Then, it adds declarations for overriding implementations of the function to all subclasses.

For example:

    public(field) int w;             // 'w' gets printed by debugPrint
    protected(virtual) int foo();    // 'foo' declared in subclasses too
    private(owner) int *p;           // 'p' deleted by destructor

Also optionally, before the final semicolon, an initializing expression can be provided. This expression will be used to initialize the data member in the constructor. Also, if the "virtual" modifier is used, this will be applied to the declaration of a function, to make it a "pure virtual" function.

    public int x = 3;                // 'x' initialized to 3 in constructor
    public(virtual) int bar() = 0;   // 'bar' pure in superclass

The declaration of bar above could also be written

    pure_virtual int bar();

as pure\_virtual is syntactic sugar for a public, virtual (automatic declarations in subclasses) field that is pure in the superclass.

#### 1.3.3 Custom Code

astgen can insert user-specified code at key points in the code it emits. This is useful for doing some processing with the otherwise uninterpreted fields (the fields astgen doesn't know how to process). The user specifies such code by saying

    custom <kind> { /* ...code... */ }

The \<kind>, lexically just an identifier, controls where this code gets inserted. The current kinds recognized are:

*   debugPrint: Inserted into the 'debugPrint' method after the header is printed.
*   preemptDebugPrint: Inserted into 'debugPrint' before anything else is printed.
*   clone: Inserted into the 'clone' method.
*   traverse: Inserted into the 'traverse' method.

If you specify a \<kind> which is not among these, you'll get a warning when 'astgen' runs.

#### 1.3.4 Subclasses

The classes form a two-level hierarchy. Subclasses are introduced with "\->" inside the superclass body. The syntax following "\->" is identical to what follows "class".

If a class has subclasses, then the superclass is abstract (you can't instantiate it). Further, most of the generated methods are virtual, so subclass implementations will be used.

Superclasses with subclasses get some additional methods, useful for interrogating the type at run-time (this is essentially an alternative to the C++ language's RTTI mechanism).

First, the superclass declares an enum called "Kind", with one value for each subclass, where the name is obtained by capitalizing all the subclass name's letters. Then a pure virtual method "kind()" is declared, and the subclass implementations return their Kind.

Then, for each subclass "Foo" you get:

    bool isFoo() const;
    Foo *asFoo();                // checked downcast
    Foo const asFooC() const;    // checked downcast, const version

The generated header file obtains these functions through the DECL\_AST\_DOWNCASTS macro, defined in [asthelp.h](asthelp.h).

#### 1.3.5 "Last" Ctor Parameters

Occasionally, it is convenient for a ctor parameter to be supplied with the superclass (so that all subclasses have it) but desirable to pass and print the value _after_ the subclass parameters. For example, a superclass Foo might want to have a "Foo \*next" to form a linked list, but we would want "next" to be at the end of all subclass argument lists, printed last (so the list is printed in the "logical" order), etc.

The syntax for this is a second parameter list in the superclass:

    class Foo (/*first*/ int x, int y) (/*last*/ Foo next) {
      -> F_one(int z);
      ...
    }

In this example, "x" and "y" are ordinary ("first") ctor parameters, and "next" is a "last" parameter. For example, to construct an instance of "F\_one", write

    new F_one(x, y, z, next)

### 1.4 Miscellaneous

Comments are either C++-style "//" or C-style "/\*\*/" form.

Some things are keywords, used to aid parsing. Keywords currently include:

    class
    public
    private
    protected
    verbatim
    impl_verbatim
    ctor
    dtor
    pure_virtual
    custom
    option

You cannot use a keyword as the name of a class, or a data member, or in any other way besides as that keyword.

2\. Extension Modules
---------------------

Tree structures often consist of a base definition and then one or more annotation systems on top of the base. Rather than clutter the base with the annotations (making it hard to re-use the base for other projects), annotations should be collected into extension modules.

The extension system is very simple. You supply additional astgen input files on the command line, and the extensions are simply unioned with the base in the obvious way:

*   ctor args are appended
*   fields are added
*   class names that don't exist in the base define new classes

There can be multiple extension modules, and they are added in the order specified on the command line.

3\. Visitor Interface
---------------------

If the input file includes an option of the form

    option visitor <name>;

Then a visitor implementation will be generated, and \<name> used as the name of the interface class.

### 3.1 The Interface Class

The visitor interface class declares two virtual functions for each superclass "Foo" in the astgen input:

    bool visitFoo(Foo *obj);
    void postvisitFoo(Foo *obj);

visitFoo is called in pre-order (before any children are visited) and postvisitFoo is called in post-order. If visitFoo returns false, then its children are not visited and postvisitFoo is not called.

### 3.2 The Traversal Functions

Each tree class is given a method:

    void traverse(<name> &vis);

where \<name> is the visitor interface class name. You can start a visiting traversal by saying "node->traverse(vis)" where "vis" is an object that implements the visitor interface. This function is virtual if the class in question has children (subclasses).
