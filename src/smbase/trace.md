The Trace Module
================

Introduction
------------

The trace ([trace.h](trace.h), [trace.cc](trace.cc)) module is designed to support a refinement of printf-style debugging. While I do not use printf-style debugging exclusively, it is nevertheless an important and often effective technique.

The main difficulty with traditional printf is that you can't easily turn the output on and off. Hence, the central feature of the trace module is a set of _tracing flags_, set at run-time, that control tracing output. There is also a compile-time flag, NDEBUG, to disable them completely.

Basic Use
---------

An example tracing directive is the following:

    TRACE("weapons", "about to fire proton torpedo " << torpNum);

If NDEBUG is #defined, this expands to nothing. But if it is not, then this expands to something like:

    if (tracingSys("weapons")) {
      cout << "%%% weapons: " << "about to fire proton torpedo " << torpNum << endl;
    }

Several things are noteworthy about the expansion:

* The output is going to stdout. I prefer it that way, but feel free to add a global variable that sends it to stderr if you really want.

* It is prefixed by "%%% weapons" (or whatever) so you can easily recognize it amongst other output, grep it in/out, etc.

* Since it's using the C++ iostreams to do output, you can add extra information, like torpNum above. If you need printf-style formatting, use the stringf function in the [str](str.h) module.

* Since it ends with endl, it prints the newline automatically, _and_ flushes the output stream. That way you'll see the output even if the program crashes on the next line.

* The tracingSys function can be used to explicitly query whether a given tracing flag is active.

Now, the above is a slight simplification. In fact, the second argument to TRACE is actually evaluated _regardless_ of whether the flag is turned on. This is so that if the evaluation itself has a bug (e.g. it segfaults), it will be found quickly, rather than waiting to bite someone who is trying to debug something else.

Enabling Flags
--------------

There are several ways to turn on a tracing flag.

* The traceAddSys function adds a flag explicitly.

* The TRACE_ARGS macro will search for occurrences of "-tr _flag_" at the beginning of the command line (argc, argv), and add the named flag. You can specify more than one flag if they are separated by commas (e.g "-tr a,b,c") or with multiple -tr options. Note that this is a fairly clunky API, and should only be used in test programs. A real program should do its own command-line argument processing and call traceAddSys directly.

* The traceAddFromEnvVar function will grab a comma-separated list of flags from the TRACE environment variable. Note that TRACE_ARGS calls traceAddFromEnvVar.

Trace Flag Naming Convention
----------------------------

In my projects, most of my tracing flags are given the name of the module they are in (e.g. TRACE("foo", ...) in foo.cc). Module-level tracing is to report events likely to be relevant to users and casual maintainers of the module. For more detail, I typically add another word to name the task at hand, e.g. TRACE("fooInit").

Debugging, Testing and the Trace Module
---------------------------------------

How should this module be used in the larger process context? First, some terminology:

* **Testing** is the process of determining if software has defects, and adequately characterizing those defects so they can be debugged, ideally by producing a reproducible testcase. The input to testing is a program and a specification, and the output is a set of bug reports.

* **Debugging** is the process of finding out the cause of a defect. The input is a bug report, and the output is a detailed explanation of what is wrong with the program, which characterizes the defect in sufficient detail to allow someone to fix the code.

I advocate using the tracing module for _debugging_ only. Since the tracing activity (even when no output is produced) is typically prohibitively expensive, one cannot ship to users an executable that has them in. And, since it is a good idea to "ship what you test", testing should be done with NDEBUG turned on, and hence tracing disabled. (See also the note about this in [xassert.h](xassert.h).)

Of course, there is nothing wrong with doing some testing with tracing enabled. In fact, when I am the one developing the code, I do most of my testing with it enabled. But the program _must_ also be tested with tracing disabled if that's how users will use it, and to the extent there is dedicated QA activities in your project I recommend they be performed on the NDEBUG build.

* * *

Back to [smbase](index.md).
