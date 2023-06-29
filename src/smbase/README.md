smbase: A Utility Library
=========================

Introduction
============

"smbase" stands for Scott McPeak's Base Library (sorry, naming things is not my specialty). It's a bunch of utility modules I use in virtually all of my projects. The entire library is in the public domain.

There is some overlap in functionality between smbase and the C++ Standard Library. Partly this is because smbase predates the standard library, and partly this is because that library has aspects to its design that I disagree with (for example, I think it is excessively templatized, given flaws in the C++ template mechanism). However, the intent is that client code can use smbase and the standard library at the same time.

smbase has developed organically, in response to specific needs. While each module individually has been reasonably carefully designed, the library as a whole has not. Consequently, the modules to not always orthogonally cover a given design space, and some of the modules are now considered obsolete (marked below as such).

Some of the links below refer to generated documentation files. If you are reading this from your local filesystem, you may have to say "make gendoc" (after "./configure") to get them.

Build Instructions
==================

    $ ./configure
    $ make
    $ make check

[./configure](configure.pl) understands [these options](gendoc/configure.txt). You can also look at the [Makefile](Makefile.in).

Modules
=======

The following sections list all the smbase modules, grouped by functionality.

Linked Lists
------------

Linked lists are sequences of objects with O(1) insertion at the front and iterators for traversal. Most also have _mutators_ for traversing and modifying.

The two main lists classes are ObjList and SObjList. Both are lists of pointers to objects; the former _owns_ the objects, and will delete them when it goes away, while the latter does not.

* [objlist.h](objlist.h): ObjList, a general linked list of objects. ObjList considers itself to "own" (be responsible for deallocating) the things on its list. See also [sobjlist.h](sobjlist.h).

* [sobjlist.h](sobjlist.h): SObjList, a general linked list of objects. SObjList does _not_ consider itself the owner of the list elements. The "s" in the name stands for "serf", which I use to mean the opposite of "owner". See also [objlist.h](objlist.h).

* [xobjlist.h](xobjlist.h): This file is processed by [M4](http://www.gnu.org/software/m4/) to make [objlist.h](objlist.h) and [sobjlist.h](sobjlist.h).

* [voidlist.h](voidlist.h), [voidlist.cc](voidlist.cc): The core of the linked list implementation used by [objlist.h](objlist.h) and [sobjlist.h](sobjlist.h).

There are a couple of variants that support O(1) appending.

* [vdtllist.h](vdtllist.h), [vdtllist.cc](vdtllist.cc): VoidTailList, the core of a linked list implementation which maintains a pointer to the last node for O(1) appends. Used by [astlist.h](astlist.h) and [taillist.h](taillist.h).

* [taillist.h](taillist.h): Template class built on top of VoidTailList ([vdtllist.h](vdtllist.h)).

* [astlist.h](astlist.h): ASTList, a list class for use in abstract syntax trees.

Finally, two stacks implemented with lists. Recently, I've been preferring to use array-based stacks ([array.h](array.h)), so these are somewhat obsolete.

* [objstack.h](objstack.h): ObjStack, a stack of owned objects. Built with a linked list.

* [sobjstack.h](sobjstack.h): SObjStack, a stack of non-owned objects. Built with a linked list.

Arrays
------

Arrays are sequences of objects with O(1) random access and replacement.

The main array header, [array.h](array.h), contains several array classes. GrowArray supports bounds checking and a method to grow the array. ArrayStack supports a distinction between the _length_ of the sequence and the _size_ of the array allocated to store it, and grows the latter automatically.

* [array.h](array.h): Several array-like template classes, including growable arrays.

The other array modules are less-used.

* [arrayqueue.h](arrayqueue.h): ArrayQueue, a template class implementing a queue with an array. Supports O(1) enqueue and dequeue.

* [datablok.h](datablok.h), [datablok.cpp](datablok.cpp): DataBlock, an array of characters of a given length. Useful when the data may contain NUL ('\\0') bytes.

* [growbuf.h](growbuf.h), [growbuf.cc](growbuf.cc): Extension of DataBlock ([datablok.h](datablok.h)) that provides an append() function.

This is obsolete.

* [arraymap.h](arraymap.h): ArrayMap, a map from integers to object pointers. Obsolete.

Arrays of Bits
--------------

Arrays of bits are handled specially, because they are implemented by storing multiple bits per byte.

* [bit2d.h](bit2d.h), [bit2d.cc](bit2d.cc): Two-dimensional array of bits.

* [bitarray.h](bitarray.h), [bitarray.cc](bitarray.cc): One-dimensional array of bits.

Hash Tables and Maps
--------------------

Maps support mapping from arbitrary domains to arbitrary ranges. Mappings can be added and queried in amortized O(1) time, but the constant factor is considerably higher than for arrays and lists.

Probably the most common map is the PtrMap template, which will map from pointers to pointers, for arbitrary pointed-to types.

* [ptrmap.h](ptrmap.h): Template class built on top of VoidPtrMap ([vptrmap.h](vptrmap.h)).

* [objmap.h](objmap.h): Variant of PtrMap ([ptrmap.h](ptrmap.h)) that owns the values.

* [vptrmap.h](vptrmap.h), [vptrmap.cc](vptrmap.cc): Hashtable-based map from void* to void*. Used by [ptrmap.h](ptrmap.h) and [objmap.h](objmap.h).

If the key can always be derived from the data (for example, the key is stored in the data object), then it is inefficient to store both in the table. The following variants require a function pointer to map from data to keys.

* [hashtbl.h](hashtbl.h), [hashtbl.cc](hashtbl.cc): HashTable, a hash table. Maps void* to void*.

* [thashtbl.h](thashtbl.h): Template class built on top of HashTable. Maps KEY* to VALUE*.

* [ohashtbl.h](ohashtbl.h): OwnerHashTable, a hash table that owns the values, built on top of HashTable. Maps void* to T*.

The above can be used to efficiently implement a set of T*.

* [sobjset.h](sobjset.h): SObjSet, a non-owning set of objects implemented on top of HashTable.

There are two specialized versions that combine O(1) insertion and query of a hash table with O(1) enqueue and dequeue of an array.

* [okhasharr.h](okhasharr.h): OwnerKHashArray, a combination of an owner hash table and an array/stack.

* [okhashtbl.h](okhashtbl.h): OwnerKHashTable, a version of [okhasharr.h](okhasharr.h) with type-safe keys ("k" for keys).

Maps with Strings as Keys
-------------------------

Mapping from strings is a nontrivial extension of the above maps because comparison is more than a pointer equality test. So there are some specialized maps from strings.

If you have a function that can map from data to (string) key, then StringHash and TStringHash (the template version) are the most efficient:

* [strhash.h](strhash.h), [strhash.cc](strhash.cc): StringHash, a case-sensitive map from strings to void* pointers. Built on top of HashTable.

The StringVoidDict and templates wrapped around it are more general. They do not require a function to map from data to key, support query-then-modify-result, and alphabetic iteration.

* [strobjdict.h](strobjdict.h): StringObjDict, a case-sensitive map from strings to object pointers. The dictionary owns the referred-to objects.

* [strsobjdict.h](strsobjdict.h): StringSObjDict, a case-sensitive map from strings to object pointers. The dictionary does _not_ own the referred-to objects.

* [svdict.h](svdict.h), [svdict.cc](svdict.cc): StringVoidDict, a case-sensitive map from strings to void* pointers. Built on top of StringHash.

Finally, there is a module to map from strings to strings.

* [strdict.h](strdict.h), [strdict.cc](strdict.cc): StringDict, a case-sensitive map from strings to strings. Currently, this is implemented with a linked list and consequently not efficient. But it will work when efficiency does not matter, and could be reimplemented (preserving the interface) with something better.

Strings
-------

Strings are sequences of characters.

* [str.h](str.h), [str.cpp](str.cpp): The string class itself. Using the string class instead of char* makes handling strings as convenent as manipulating fundamental types like int or float. See also [string.txt](string.txt).

* [stringset.h](stringset.h), [stringset.cc](stringset.cc): StringSet, a set of strings.

* [strtokp.h](strtokp.h), [strtokp.cpp](strtokp.cpp): StrtokParse, a class that parses a string similar to how strtok() works, but provides a more convenient (and thread-safe) interface. Similar to Java's StringTokenizer.

* [strutil.h](strutil.h), [strutil.cc](strutil.cc): A set of generic string utilities, including replace(), translate(), trimWhitespace(), encodeWithEscapes(), etc.

* [smregexp.h](smregexp.h), [smregexp.cc](smregexp.cc): Regular expression matching.

System Utilities
----------------

The following modules provide access to or wrappers around various low-level system services.

* [autofile.h](autofile.h), [autofile.cc](autofile.cc): AutoFILE, a simple wrapper around FILE* to open it or throw an exception, and automatically close it.

* [cycles.h](cycles.h) [cycles.c](cycles.c): Report total number of processor cycles since the machine was turned on. Uses the RDTSC instruction on x86.

* [missing.h](missing.h), [missing.cpp](missing.cpp): Implementations of a few C library functions that are not present on all platforms.

* [mypopen.h](mypopen.h), [mypopen.c](mypopen.c): Open a process, yielding two pipes: one for writing, one for reading.

* [mysig.h](mysig.h), [mysig.cc](mysig.cc): Some signal-related utilities.

* [syserr.h](syserr.h), [syserr.cpp](syserr.cpp): Intended to be a portable encapsulation of system-dependent error facilities like UNIX's errno and Win32's GetLastError(). It's not very complete right now.

* [unixutil.h](unixutil.h), [unixutil.c](unixutil.c): Some utilities on top of unix functions: writeAll(), readString().

Portability
-----------

These modules help insulate client code from the details of the system it is running on.

* [nonport.h](nonport.h), [nonport.cpp](nonport.cpp): A library of utility functions whose implementation is system-specific. Generally, I try to encapsulate all system depenencies as functions defined in nonport.

* [macros.h](macros.h): A bunch of useful macros.

* [typ.h](typ.h): Some type definitions like byte and bool, plus a few utility macros. Not clearly distinguished from [macros.h](macros.h) in purpose.

Allocation
----------

These modules provide additional control over the allocator.

* [ckheap.h](ckheap.h): Interface to check heap integrity. The underlying malloc implementation must support these entry points for it to work. I've extended Doug Lea's malloc ([malloc.c](malloc.c)) to do so.

* [malloc.c](malloc.c): Version 2.7.0 of [Doug Lea's malloc](http://gee.cs.oswego.edu/dl/html/malloc.html). I've made some modifications to help with debugging of memory errors in client code.

* [objpool.h](objpool.h): ObjPool, a custom allocator for fixed-size objects with embedded 'next' links.

Exceptions
----------

These modules define or throw exceptions.

* [exc.h](exc.h), [exc.cpp](exc.cpp): Various exception classes. The intent is derive everything from xBase, so a program can catch this one exception type in main() and be assured no exception will propagate out of the program (or any other unit of granularity you want).

* [xassert.h](xassert.h): xassert is an assert()-like macro that throws an exception when it fails, instead of calling abort().

Serialization
-------------

The "flatten" serialization scheme is intended to allow sets of objects to read and write themselves to files.

* [bflatten.h](bflatten.h), [bflatten.cc](bflatten.cc): Implementation of the Flatten interface ([flatten.h](flatten.h)) for reading/writing binary files.

* [flatten.h](flatten.h), [flatten.cc](flatten.cc): Generic interface for serializing in-memory data structures to files. Similar to Java's Serializable, but independently conceived, and has superior version control facilities.

Compiler/Translator Support
---------------------------

smbase has a number of modules that are of use to programs that read and/or write source code.

* [hashline.h](hashline.h), [hashline.cc](hashline.cc): HashLineMap, a mechanism for keeping track of #line directives in C source files. Provides efficient queries with respect to a set of such directives.

* [srcloc.h](srcloc.h), [srcloc.cc](srcloc.cc): This module maintains a one-word data type called SourceLoc. SourceLoc is a location within some file, e.g. line/col or character offset information. SourceLoc also encodes _which_ file it refers to. This type is very useful for language processors (like compilers) because it efficiently encodes location formation. Decoding this into human-readable form is slower than incrementally updating it, but decoding is made somewhat efficient with some appropriate index structures.

* [boxprint.h](boxprint.h) [boxprint.cc](boxprint.cc): BoxPrint functions as an output stream (sort of like cout) with operations to indicate structure within the emitted text, so that it can break lines intelligently. It's used as part of a source-code pretty-printer.

* [warn.h](warn.h), [warn.cpp](warn.cpp): Intended to provide a general interface for user-level warnings; the design never really worked well.

Testing and Debugging
---------------------

* [breaker.h](breaker.h) [breaker.cpp](breaker.cpp): Function for putting a breakpoint in, to get debugger control just before an exception is thrown.

* [test.h](test.h): A few test-harness macros.

* [trace.h](trace.h), [trace.cc](trace.cc): Module for recording and querying a set of debug tracing flags. It is documented in [trace.md](trace.md).

* [trdelete.h](trdelete.h), [trdelete.cc](trdelete.cc): An operator delete which overwrites the deallocated memory with 0xAA before deallocating it.

Miscellaneous
-------------

* [crc.h](crc.h) [crc.cpp](crc.cpp): 32-bit cyclic redundancy check.

* [gprintf.h](gprintf.h), [gprintf.c](gprintf.c): General printf; calls a function to emit each piece.

* [owner.h](owner.h): Owner, a pointer that deallocates its referrent in its destructor. Similar to auto_ptr in the C++ Standard.

* [point.h](point.h), [point.cc](point.cc): Point, a pair of integers.

Test Drivers
------------

Test drivers. Below are the modules that are purely test drivers for other modules. They're separated out from the list above to avoid the clutter.

* [testmalloc.cc](testmalloc.cc): A program to test the interface exposed by [ckheap.h](ckheap.h).

* [tmalloc.c](tmalloc.c): Test my debugging enhancements to [malloc.c](malloc.c).

* [tarrayqueue.cc](tarrayqueue.cc): Test driver for [arrayqueue.h](arrayqueue.h).

* [testarray.cc](testarray.cc): Test driver for [array.h](array.h).

* [testcout.cc](testcout.cc): This is a little test program for use by [configure.pl](configure.pl).

* [tobjlist.cc](tobjlist.cc): Test driver for [objlist.h](objlist.h).

* [tobjpool.cc](tobjpool.cc) Test driver for [objpool.h](objpool.h).

* [tsobjlist.cc](tsobjlist.cc): Test driver for [sobjlist.h](sobjlist.h).

Utility Scripts
---------------

* [run-flex.pl](run-flex.pl): Perl script to run flex and massage its output for portability.

* [sm_config.pm](sm_config.pm): This is a Perl module, intended to be used by configure scripts. It is mostly a library of useful routines, but also reads and writes some of the main script's global variables.

Module Dependencies
===================

The [scan-depends.pl](scan-depends.pl) script is capable of generating a [module dependency description](gendoc/dependencies.dot) in the [Dot](http://www.research.att.com/sw/tools/graphviz/) format. Not all the modules appear; I try to show the most important modules, and try to avoid making Dot do weird things. Below is its output.

![Module Dependencies](gendoc/dependencies.png)  
There's also a [Postscript version](gendoc/dependencies.ps).

