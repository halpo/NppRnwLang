# The src\ directory
Contains all the source files for the RnwLang plugin.

## src\dbgstream
contains code from Dietmar Kuehl to use a debug stream object (dbg) for debug loggin messages.

## src\headers
contains slightly modified headers from Notepad++ required for building.

## Files
deparse_wm_msg.\*
: files for converting WM_MSG messages to their actual names such as "0x0003"  mapps to WM_MOVE
RnwLang.cpp
: Plugin interface implimented
RnwLang.h
: All plugin header definition and inclusions
RnwLex.cpp
: Lexer highlight and folding implimentation
unidef.h
: unicode definitions and mappings