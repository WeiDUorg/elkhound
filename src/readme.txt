Changed by Valerio Bigiani on 2009-01-26 to fix an issue in run-flex.pl and
remove Elsa.

Changed by Fredrik Lindgren on 2013-05-31 and onward. See git log for changes.
Prior to that (circa 2011) there were other changes of an unspecified nature,
made by a benevolent passer-by.

readme.txt for Elkhound/Elsa distribution
-----------------------------------------

This release is provided under the BSD license.  See license.txt.

Elkhound is a parser generator.
Elsa is a C/C++ parser that uses Elkhound.

See additional documentation in index.html in the various
subdirectories.

Alternatively, see the Documentation section of
http://www.cs.berkeley.edu/~smcpeak/elkhound/ .


Build instructions:

  $ mkdir build && cd build
  $ cmake ../src/ -DEXTRAS=ON -DCMAKE_BUILD_TYPE=Release
  $ make
  $ make test     # (optional but a good idea)

This simply does each of these activities in each of the directories:
smbase, ast, elkhound and elsa.  If a command fails you can restart it
in a particular directory just by going into the failing directory and
issuing it there.

After building, the interesting binary is elsa/ccparse.  See
elsa/index.html for more info on what to do with it.


If you run into problems you can email me: smcpeak@cs.berkeley.edu
But be aware I'm usually pretty busy so responses may take a couple
of days.
