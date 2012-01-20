# Makefile for Notepad++ plugins
.SUFFIXES: .cpp .cxx
CC = g++
RC = windres --target pe-i386
DEL = rm -f
all: plugin

#CONTROL Variables
# DEBUG=1
# UNICODE=1

SRC = ../src

PLUGIN = LexRnw.dll
SCINTILLA_SRC = ../Scintilla

### SCINTILLA OBJECTS #########
BASEOBJS = \
	AutoComplete.o \
	CallTip.o \
	CellBuffer.o \
	CharacterSet.o \
	CharClassify.o \
	ContractionState.o \
	Decoration.o \
	Document.o \
	Editor.o \
	KeyMap.o \
	Indicator.o \
	LineMarker.o \
	PerLine.o \
	PlatWin.o \
	PositionCache.o \
	PropSetSimple.o \
	RESearch.o \
	RunStyles.o \
	ScintRes.o \
	Selection.o \
	Style.o \
	UniConversion.o \
	ViewStyle.o \
	XPM.o
LOBJS = \
	Accessor.o \
	Catalogue.o \
	ExternalLexer.o \
	LexerBase.o \
	LexerModule.o \
	LexerSimple.o \
	StyleContext.o \
	WordList.o \
	$(BASEOBJS)	
###  End Scintilla objects ###

LEXERS = \
	LexR.o \
	LexTeX.o
$(LEXERS):lexers.h
PLUGIN_DEF = $(SRC)/Plugin.def
PLUGIN_OBJECTS = \
	RnwLang.o \
	RnwRes.o \
	LexRnw.o \
	deparse_wm_msg.o \
	dbgstream.o \
	$(LEXERS)
$(PLUGIN_OBJECTS): $(SRC)/RnwLang.h
OBJECTS = $(PLUGIN_OBJECTS) $(LOBJS)

vpath %.h   $(SRC) $(SRC)/headers\
            $(SCINTILLA_SRC)/src \
            $(SCINTILLA_SRC)/include \
            $(SCINTILLA_SRC)/lexlib \
            $(SRC)/dbgstream
vpath %.cpp $(SRC) \
            $(SRC)/dbgstream
vpath %.cxx $(SCINTILLA_SRC)/src \
            $(SCINTILLA_SRC)/lexlib \
            $(SCINTILLA_SRC)/win32
vpath %.rc $(SRC) $(SCINTILLA_SRC)/win32


LDFLAGS=-shared -Wl,--enable-runtime-pseudo-reloc-v2 -mwindows -Wl,--add-stdcall-alias
LIBS=-lstdc++ -limm32 -lole32 -luuid
# Add -MMD to get dependencies
INCLUDEDIRS = -I $(SCINTILLA_SRC)/include \
              -I $(SCINTILLA_SRC)/src \
		    -I $(SCINTILLA_SRC)/lexlib \
		    -I $(SRC)/headers \
		    -I $(SRC)/dbgstream
		    
# http://gcc.gnu.org/onlinedocs/gcc-4.4.3/gcc/Warning-Options.html#Warning-Options
CHECK_FLAGS  = -Wall -Wextra -Wno-missing-braces -Wno-char-subscripts \
               -Wno-strict-overflow -pedantic -Wno-unused-parameter
SCI_CXXFLAGS = -DSCI_LEXER
CXXBASEFLAGS = -m32 $(CHECK_FLAGS) $(SCI_CXXFLAGS)\
	$(INCLUDEDIRS) -fno-rtti  \
	-march=i686 -mthreads

# DEBUG
ifdef DEBUG
CXXFLAGS=-DDEBUG -g -O0 $(CXXBASEFLAGS)
else
CXXFLAGS=-DNDEBUG -Os $(CXXBASEFLAGS)
STRIPFLAG=-s
endif

#UNICODE
ifdef UNICODE
CXXFLAGS+=-DUNICODE
endif

# exported variables
export DEBUG
export UNICODE

##  RULES  ####################################################################
#goals
.PHONY: all unicode plugin clean echo debug objects
plugin: unicode
unicode: $(PLUGIN)
objects:$(OBJECTS)

.cxx.o:
	$(CC) $(CXXFLAGS) -c $<
.cpp.o:
	$(CC) $(CXXFLAGS) -c $<

$(PLUGIN): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $(STRIPFLAG) $^ $(CXXFLAGS) $(LIBS)

%.o:	%.rc
	$(RC) $< $@

.PHONY:clean sweep clobber
sweep:
	-$(DEL) *.dll $(PLUGIN_OBJECTS) dbgtest.exe
clean: sweep
	-$(DEL) *.exe *.o *.obj *.dll *.res *.map *.d
clobber:clean
	cd ..; $(MAKE) --win32 clean
	
echo:
	@echo SRC_DIR= $(SRC)
	@echo OBJECTS= $(OBJECTS)
	g++ --version
	@echo $(DEBUG)

#Pass-off rules
# ../%:
	# cd .. && $(MAKE) --win32 $*
	
#noted rule using dllwrap for definition file.
RnwLangLib.dll: $(OBJECTS)
	dlltool -l $@ -mi386 $(OBJECTS) --no-export-all-symbols --add-stdcall-underscore 
dbgtest.exe: ../src/dbgstream/dbgstream.cpp
	$(CC) -m32 -mwindows -o $@ $(STRIPFLAG) $< $(CXXFLAGS) -DTEST_MAIN
deparseWM_MSG.exe: deparse_wm_msg.cpp
	$(CC) -m32 -mwindows -o $@ $(STRIPFLAG) $< $(CXXFLAGS) -DTEST_MAIN
	
	
	