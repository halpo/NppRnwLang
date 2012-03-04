# Makefile for Notepad++ plugins
.SUFFIXES: .cpp .cxx

all: plugin
.PHONY: all unicode plugin clean echo debug objects

SRC             ?= ../src
LEXERPLUGIN_SRC ?= ../LexerPlugin
DEBUG_SRC       ?= ../debug
PLUGIN          ?= LexRnw.dll
SCINTILLA_BASE  ?= ../Scintilla

$(PLUGIN_OBJECTS): LexerPlugin.h

LEXERPLUGIN_OBJS = LexerPlugin.o LexerPluginRes.o

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


OBJECTS = \
	$(LOBJS) \
	$(PLUGIN_OBJECTS) \
	$(LEXERPLUGIN_OBJS)

vpath %.h   $(SRC) \
		  $(LEXERPLUGIN_SRC) $(LEXERPLUGIN_SRC)/headers\
            $(SCINTILLA_BASE)/src \
            $(SCINTILLA_BASE)/include \
            $(SCINTILLA_BASE)/lexlib
vpath %.cpp $(SRC) $(LEXERPLUGIN_SRC)
vpath %.cxx $(SCINTILLA_BASE)/src \
            $(SCINTILLA_BASE)/lexlib \
            $(SCINTILLA_BASE)/win32
vpath %.rc $(SRC) $(LEXERPLUGIN_SRC) $(SCINTILLA_BASE)/win32


LDFLAGS=-shared -Wl,--enable-runtime-pseudo-reloc-v2 -mwindows -Wl,--add-stdcall-alias
LIBS=-lstdc++ -limm32 -lole32 -luuid
# Add -MMD to get dependencies
INCLUDEDIRS = \
		    -I $(SRC) \
		    -I $(LEXERPLUGIN_SRC) \
		    -I $(LEXERPLUGIN_SRC)/headers \
              -I $(SCINTILLA_BASE)/include  \
              -I $(SCINTILLA_BASE)/src      \
		    -I $(SCINTILLA_BASE)/lexlib
		    
# http://gcc.gnu.org/onlinedocs/gcc-4.4.3/gcc/Warning-Options.html#Warning-Options
CHECK_FLAGS  = -Wall -Wextra -Wno-missing-braces -Wno-char-subscripts \
			-Wno-strict-overflow -pedantic -Wno-unused-parameter   \
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
	-$(DEL) $(PLUGIN) $(PLUGIN_OBJECTS) $(LEXERPLUGIN_OBJS)
clean: sweep
	-$(DEL) *.exe *.o *.obj *.dll *.res *.map *.d
clobber:clean
	cd ..; $(MAKE) clean
	