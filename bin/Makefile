# Makefile for Notepad++ plugins
.SUFFIXES: .cpp .cxx

# use specifically TDM version of minGW
CC = C:/MinGW64/bin/g++
RC = windres --target pe-i386  
# the target MUST be pe-i386 for Notepad++ plugins
DEL = rm -f
all: plugin
.PHONY: all unicode plugin clean echo debug objects

SRC = ../src
DEBUG_SRC = ../debug
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
PLUGIN_OBJECTS = \
	RnwLang.o \
	RnwRes.o \
	$(LEXERS) \
	LexRnw.o
$(PLUGIN_OBJECTS): $(SRC)/RnwLang.h
OBJECTS = \
	$(LOBJS) \
	$(PLUGIN_OBJECTS)

vpath %.h   $(SRC) $(SRC)/headers\
            $(SCINTILLA_SRC)/src \
            $(SCINTILLA_SRC)/include \
            $(SCINTILLA_SRC)/lexlib
vpath %.cpp $(SRC)
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
		    -I $(SRC)/headers 
		    
# http://gcc.gnu.org/onlinedocs/gcc-4.4.3/gcc/Warning-Options.html#Warning-Options
CHECK_FLAGS  = -Wall -Wextra -Wno-missing-braces -Wno-char-subscripts \
               -Wno-strict-overflow -pedantic -Wno-unused-parameter \
			-Wno-missing-field-initializers -Wno-type-limits
SCI_CXXFLAGS = -DSCI_LEXER
CXXBASEFLAGS = -m32 $(CHECK_FLAGS) $(SCI_CXXFLAGS)\
	$(INCLUDEDIRS) -fno-rtti  \
	-march=i686 -mthreads

# DEBUG
DEBUG ?= 0
ifeq ($(DEBUG),1)
CXXFLAGS=-DDEBUG -D_DEBUG -g -O0 $(CXXBASEFLAGS)
PLUGIN_OBJECTS += \
	deparse_wm_msg.o \
	dbgstream.o
vpath %.h   $(DEBUG_SRC) $(DEBUG_SRC)/dbgstream
vpath %.cpp $(DEBUG_SRC) $(DEBUG_SRC)/dbgstream
INCLUDEDIRS += -I $(DEBUG_SRC) -I $(DEBUG_SRC)/dbgstream

else
	CXXFLAGS=-DNDEBUG -Os $(CXXBASEFLAGS)
	STRIPFLAG=-s
endif



#UNICODE
UNICODE ?= 1
ifeq ($(UNICODE), 1)
CXXFLAGS += -DUNICODE -D_UNICODE
endif

# exported variables
export DEBUG
export _DEBUG
export UNICODE
export _UNICODE

##  RULES  ####################################################################
#goals
.PHONY:plugin objects
plugin: $(PLUGIN)
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
	cd ..; $(MAKE) clean
	