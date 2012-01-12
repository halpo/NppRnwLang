RnwLang R/Sweave Syntax Highlighting for Notepad++
==================================================
Rational
--------
RnwLang is a project to serve two purposes.  First to create a high quality Sweave syntax highlighter and code folding module, collectively referred to as a *lexer*.  Second, to get a working template for Notepad++ compatible plugins that can be compiled with the GCC toolchain.  I firmly believe in Free and Open Source software and it irks me that my favorite editor Notepad++ requires that to extendit you must use Microsoft's Visual C++ suite.  I beleive that is antithetical to open source, and hope to help aleviate the restriction of using M$ products.

Sweave
------
Sweave is essentially two languages, TeX and R/S, where chunks of R code are embedded into a LaTeX document to provide reproducible report and article generation.  In the current state a User must choose to use either the R lexer or the Latex lexer, which in my opinion could use some improvement.  With RnwLang I hope to produce a Lexer that combines both and highlights appropriatly.

Building
--------
I build with the [TDM](http://tdm-gcc.tdragon.net/) port of GCC which plays slightly nicer with the windows SDK, which is required or some components.

building should be as simple and going to the `bin/` directory and executing `make all` or `make all UNICODE=1` for a unicode version.

~Andrew