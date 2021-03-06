// Scintilla source code edit control
/** @file Lexr.cxx
 ** Lexer for R, S, SPlus Statistics Program
 ** Copied and modified from scintilla base.
 **
 **/
// Copyright 1998-2002 by Neil Hodgson <neilh@scintilla.org>
// Modyfications for RnwLang by Andrew Redd 2012.
// The License.txt file describes the conditions under which this software may be distributed.

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include <ctype.h>

#include "ILexer.h"
#include "Scintilla.h"
#include "SciLexer.h"

#include "WordList.h"
#include "LexAccessor.h"
#include "Accessor.h"
#include "StyleContext.h"
#include "CharacterSet.h"
#include "LexerModule.h"
#ifdef SCI_NAMESPACE
using namespace Scintilla;
#endif

#include "RnwLang.h"
#include "lexers.h"
#include "RnwDebug.h"
#undef rnwmsg
#define rnwmsg "RnwLang:R  :"
using namespace RnwLang::Lexers;

namespace RnwLang{ namespace Lexers{ namespace R {
namespace {  // static functions
inline bool IsAWordChar(const int ch) {
	return (ch < 0x80) && (isalnum(ch) || ch == '.' || ch == '_');
}

inline bool IsAWordStart(const int ch) {
	return (ch < 0x80) && (isalnum(ch) || ch == '_');
}

inline bool IsAnOperator(const int ch) {
	if (isascii(ch) && isalnum(ch))
		return false;
	// '.' left out as it is used to make up numbers
	if (ch == '-' || ch == '+' || ch == '!' || ch == '~' ||
	        ch == '?' || ch == ':' || ch == '*' || ch == '/' ||
	        ch == '^' || ch == '<' || ch == '>' || ch == '=' ||
	        ch == '&' || ch == '|' || ch == '$' || ch == '(' ||
	        ch == ')' || ch == '}' || ch == '{' || ch == '[' ||
		ch == ']')
		return true;
	return false;
}

inline bool isEOL(const char ch, const char chNext) {
  return (ch == '\r' && chNext != '\n') || (ch == '\n');
}

const char * const RWordLists[] = {
            "Language Keywords",
            "Base / Default package function",
            "Other Package Functions",
            "Unused",
            "Unused",
            0,
        };

}  // end unamed namespace

void ColouriseDoc(unsigned int startPos, int length, int initStyle, WordList *keywordlists[],
                   Accessor &styler) {

  #ifdef DEBUG
  dbg << "RnwLang:R  :" << "R::ColourizeDoc" 
      << "("  << startPos
      << ", " << length
      << ", " << (initStyle)
      << ") lengthDoc=" << startPos+length
      << endl;
  dbg << "RnwLang:R  :"      << "Colors:" 
      << "SCE_R_DEFAULT="    << SCE_R_DEFAULT    << ", "
      << "SCE_R_COMMENT="    << SCE_R_COMMENT    << ", "
      << "SCE_R_KWORD="      << SCE_R_KWORD      << ", "
      << "SCE_R_BASEKWORD="  << SCE_R_BASEKWORD  << ", "
      << "SCE_R_OTHERKWORD=" << SCE_R_OTHERKWORD << ", "
      << "SCE_R_NUMBER="     << SCE_R_NUMBER     << ", "
      << "SCE_R_STRING="     << SCE_R_STRING     << ", "
      << "SCE_R_STRING2="    << SCE_R_STRING2    << ", "
      << "SCE_R_OPERATOR="   << SCE_R_OPERATOR   << ", "
      << "SCE_R_IDENTIFIER=" << SCE_R_IDENTIFIER << ", "
      << "SCE_R_INFIX="      << SCE_R_INFIX      << ", "
      << "SCE_R_INFIXEOL="   << SCE_R_INFIXEOL
      << endl;
  #endif
	WordList &keywords  = *keywordlists[0];
	WordList &keywords2 = *keywordlists[1];
	WordList &keywords3 = *keywordlists[2];

  if ((initStyle < R_DEFAULT) && (initStyle >= R_END))
    initStyle = SCE_R_DEFAULT;

	// Do not leak onto next line
	if (initStyle == SCE_R_INFIXEOL)
		initStyle = SCE_R_DEFAULT;


	StyleContext sc(startPos, length, initStyle, styler);

	for (; sc.More(); sc.Forward()) {

		if (sc.atLineStart && (sc.state == SCE_R_STRING)) {
			// Prevent SCE_R_STRINGEOL from leaking back to previous line
			sc.SetState(SCE_R_STRING);
		}

		// Determine if the current state should terminate.
		if (sc.state == SCE_R_OPERATOR) {
			sc.SetState(SCE_R_DEFAULT);
		} else if (sc.state == SCE_R_NUMBER) {
			if (!IsADigit(sc.ch) && !(sc.ch == '.' && IsADigit(sc.chNext))) {
				sc.SetState(SCE_R_DEFAULT);
			}
		} else if (sc.state == SCE_R_IDENTIFIER) {
			if (!IsAWordChar(sc.ch)) {
				char s[100];
				sc.GetCurrent(s, sizeof(s));
				if (keywords.InList(s)) {
					sc.ChangeState(SCE_R_KWORD);
				} else if  (keywords2.InList(s)) {
					sc.ChangeState(SCE_R_BASEKWORD);
				} else if  (keywords3.InList(s)) {
					sc.ChangeState(SCE_R_OTHERKWORD);
				}
				sc.SetState(SCE_R_DEFAULT);
			}
		} else if (sc.state == SCE_R_COMMENT) {
			if (sc.ch == '\r' || sc.ch == '\n') {
				sc.SetState(SCE_R_DEFAULT);
			}
		} else if (sc.state == SCE_R_STRING) {
			if (sc.ch == '\\') {
				if (sc.chNext == '\"' || sc.chNext == '\'' || sc.chNext == '\\') {
					sc.Forward();
				}
			} else if (sc.ch == '\"') {
				sc.ForwardSetState(SCE_R_DEFAULT);
			}
		} else if (sc.state == SCE_R_INFIX) {
			if (sc.ch == '%') {
				sc.ForwardSetState(SCE_R_DEFAULT);
			} else if (sc.atLineEnd) {
				sc.ChangeState(SCE_R_INFIXEOL);
				sc.ForwardSetState(SCE_R_DEFAULT);
			}
		}else if (sc.state == SCE_R_STRING2) {
			if (sc.ch == '\\') {
				if (sc.chNext == '\"' || sc.chNext == '\'' || sc.chNext == '\\') {
					sc.Forward();
				}
			} else if (sc.ch == '\'') {
				sc.ForwardSetState(SCE_R_DEFAULT);
			}
		}

		// Determine if a new state should be entered.
		if (sc.state == SCE_R_DEFAULT) {
			if (IsADigit(sc.ch) || (sc.ch == '.' && IsADigit(sc.chNext))) {
				sc.SetState(SCE_R_NUMBER);
			} else if (IsAWordStart(sc.ch) ) {
				sc.SetState(SCE_R_IDENTIFIER);
			} else if (sc.Match('#')) {
					sc.SetState(SCE_R_COMMENT);
			} else if (sc.ch == '\"') {
				sc.SetState(SCE_R_STRING);
			} else if (sc.ch == '%') {
				sc.SetState(SCE_R_INFIX);
			} else if (sc.ch == '\'') {
				sc.SetState(SCE_R_STRING2);
			} else if (IsAnOperator(sc.ch)) {
				sc.SetState(SCE_R_OPERATOR);
        #ifdef DEBUG
        char _S_[2]={sc.ch,0};
        dbg << "RnwLang:R  :Operator '" << &_S_[0] 
            << "' at:" << sc.currentPos 
            << " style=" << sc.state
            << endl;
        #endif
			}
		}
	}
	sc.Complete();
  _debugleave_;
}


// Store both the current line's fold level and the next lines in the
// level store to make it easy to pick up with each increment
// and to make it possible to fiddle the current level for "} else {".
void FoldDoc(unsigned int startPos, int length, int, WordList *[],
              Accessor &styler) {
  #ifdef DEBUG
  _debugenter
      << "("  << startPos
      << ", " << length
      << ", ...)"
      << ", last level = " << hex << styler.LevelAt(styler.GetLine(startPos-1)) << dec
      << endl;
  #endif
	bool foldCompact = styler.GetPropertyInt("fold.compact", 1) != 0;
	bool foldAtElse = styler.GetPropertyInt("fold.at.else", 0) != 0;
	unsigned int endPos = startPos + length;
	int visibleChars = 0;
	int lineCurrent = styler.GetLine(startPos);
	int levelCurrent = SC_FOLDLEVELBASE;
	if (lineCurrent > 0)
		levelCurrent = styler.LevelAt(lineCurrent-1) >> 16;
	int levelMinCurrent = levelCurrent;
	int levelNext = levelCurrent;
	char chNext = styler[startPos];
	int styleNext = styler.StyleAt(startPos);
  #ifdef DEBUG
  dbg << "RnwLang:R  :" << "R Lexer: Settings:"
      <<   "foldCompact="    << foldCompact
      << ", foldAtElse="     << foldAtElse
      << ", endPos="         << endPos
      << ", visibleChars="   << visibleChars
      << endl;
  dbg << "RnwLang:R  :" << "Fold Variables"
      << ", lineCurrent="    << lineCurrent
      << ", levelCurrent="   << levelCurrent
      << ", levelMinCurrent="<< levelMinCurrent
      << ", levelNext="      << levelNext
      << ", chNext="         << chNext
      << ", styleNext="      << styleNext
      << ", styleNext="      << styleNext
      << endl;
  #endif
	for (unsigned int i = startPos; i < endPos; i++) {
		char ch = chNext;
		chNext  = styler.SafeGetCharAt(i + 1);
		int style = styleNext;
		styleNext = styler.StyleAt(i + 1);
		if (style == SCE_R_OPERATOR) {
			if (ch == '{') {
				// Measure the minimum before a '{' to allow
				// folding on "} else {"
				if (levelMinCurrent > levelNext) {
					levelMinCurrent = levelNext;
				}
				levelNext++;
			} else if (ch == '}') {
				levelNext--;
			}
		}
		if (isEOL(ch, chNext)) {
			int levelUse = levelCurrent;
			if (foldAtElse) {
				levelUse = levelMinCurrent;
			}
			int lev = levelUse | levelNext  << 16;
			if (visibleChars == 0 && foldCompact)
				lev |= SC_FOLDLEVELWHITEFLAG;
			if (levelUse < levelNext)
				lev |= SC_FOLDLEVELHEADERFLAG;
			if (lev != styler.LevelAt(lineCurrent)) {
				styler.SetLevel(lineCurrent, lev);
			}
			lineCurrent++;
			levelCurrent = levelNext;
			levelMinCurrent = levelCurrent;
			visibleChars = 0;
		}
		if (!isspacechar(ch))
			visibleChars++;
	}
#ifdef DEBUG
for(int l=styler.GetLine(startPos); l<styler.GetLine(endPos); l++) {
dbg << "RnwLang:R  :" << __func__
    << ":level(" << l 
    << ")=" << hex << styler.LevelAt(l) << dec
    << endl;
}
_debugleave_;
#endif
}

// LexerModule lmR(SCLEX_R, ColouriseRDoc, "r", FoldRDoc, RWordLists);
//{  Class Functions
LexerR::LexerR(){
  _debugenter_;
  props.Set("fold.compact", "0");
  props.Set("fold.at.else", "1");
  _debugleave_;
}
LexerR::~LexerR(){
  _debugenter_;
}
SCI_METHOD int  LexerR::WordListSet(int n, const char *wl) {
  _debugenter_;
	if (n < numWordLists) {
		WordList wlNew;
		wlNew.Set(wl);
		if (*keyWordLists[n] != wlNew) {
			keyWordLists[n]->Set(wl);
			return 0;
		}
	}
	return -1;
}
SCI_METHOD void LexerR::Lex(unsigned int startPos, int length, int initStyle, IDocument *pAccess) {
  _debugenter_;
	try {
    Accessor styler(pAccess,   &props);
		ColouriseDoc(startPos, length, initStyle, keyWordLists, styler);
	} catch (...) {
    #ifdef DEBUG
    dbg << rnwerr << "Unhandled Exception" << endl;
    #endif
		// Should not throw into caller as may be compiled with different compiler or options
		pAccess->SetErrorStatus(SC_STATUS_FAILURE);
	}
}
SCI_METHOD void LexerR::Fold(unsigned int startPos, int length, int initStyle, IDocument *pAccess) {
  _debugenter_;
	try {
    Accessor styler(pAccess,   &props);
		FoldDoc(startPos, length, initStyle, keyWordLists, styler);
	} catch (...) {
    #ifdef DEBUG
    dbg << rnwerr << "unhandled execption in " << __func__ << endl; 
    #endif
		//! Should not throw into caller as may be compiled with different compiler or options
		pAccess->SetErrorStatus(SC_STATUS_FAILURE);
	}
  #ifdef DEBUG
  dbg << rnwmsg << "leaving " << __func__ << endl;
  #endif
}
ILexer* LexerR::LexerFactory() {
  _debugenter_;
  try {
    LexerR* lex = new LexerR();
    return dynamic_cast<ILexer*>(lex);
  } catch (...) {
    #ifdef DEBUG
    dbg << rnwerr << __func__ << (": Unhandled Exception Caught") << endl;
    #endif
    // Should not throw into caller as may be compiled with different compiler or options
    return 0;
  }
}
//}
}}}  // end RnwLang::Lexers:R


