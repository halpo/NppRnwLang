/*  Copyright (C)2012 Andrew Redd
 *
 *  This file is part of the RnwLang Rnw Lexer for Notepad++.
 *
 *  RnwLang is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  RnwLang is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with RnwLang.  If not, see <http: *www.gnu.org/licenses/>.
 *
 */
/*! @file LexRnw.cpp
 *  Holds lexer functions for the Rnw R/Sweave Language
 */
#include "RnwLang.h"
using namespace std;
using namespace RnwLang;
using namespace RnwLang::Lexers;

namespace RnwLang { namespace Lexers{ namespace Rnw{
typedef unsigned int uint;
LexerRnw Rnw;

LexerRnw::LexerRnw(){
  dbg << rnwmsg << "in " << thisfunc << endl;
}
LexerRnw::~LexerRnw(){
  dbg << rnwmsg << "in " << thisfunc << "LexerRnw DESTRUCTED!!"  << endl;
}
ILexer* LexerRnw::LexerFactory() {
  dbg << rnwmsg << "in " << thisfunc << endl;
  try {
    LexerRnw* lex = new LexerRnw;
    return dynamic_cast<ILexer*>(lex);//new LexerRnw;
  } catch (...) {
    dbg << rnwerr << thisfunc << (" Unhandled Exception Caught") << endl;
    // Should not throw into caller as may be compiled with different compiler or options
    return 0;
  }
}
void SCI_METHOD LexerRnw::Lex(unsigned int startPos, int length, int initStyle, IDocument *pAccess) {
  dbg << rnwmsg << "in " << thisfunc << endl;
	try {
		Accessor astyler(pAccess, &props);
		R::ColouriseDoc(startPos, length, initStyle, keyWordLists, astyler);
		astyler.Flush();
	} catch (...) {
		// Should not throw into caller as may be compiled with different compiler or options
		pAccess->SetErrorStatus(SC_STATUS_FAILURE);
	}
}
void SCI_METHOD LexerRnw::Fold(unsigned int startPos, int length, int initStyle, IDocument *pAccess) {
  dbg << rnwmsg << "in " << thisfunc << endl;
	try {
		Accessor astyler(pAccess, &props);
		R::FoldDoc(startPos, length, initStyle, keyWordLists, astyler);
		astyler.Flush();
	} catch (...) {
		//! Should not throw into caller as may be compiled with different compiler or options
		pAccess->SetErrorStatus(SC_STATUS_FAILURE);
	}
}

namespace {
bool ScanTo(uint pos, uint max,  char * s, Accessor &styler){
  #ifdef DEBUG
  dbg << rnwmsg <<  thisfunc << endl;
  #endif
  uint i=0;
  for(; i < max; i++){
    if(styler.Match(i, s)) return i;
  }
  return i;
}
bool isInTexState(uint j, Accessor &styler){
  char state = styler.StyleAt(j);
  return TEX_DEFAULT <= state && state < TEX_END;
}
bool isInRState(uint j, Accessor &styler){
  char state = styler.StyleAt(j);
  return R_DEFAULT <= state && state < R_END;
}
bool isInCodeBlockState(uint j, Accessor &styler){
  char state = styler.StyleAt(j);
  return RNW_DEFAULT <= state && state < RNW_END;
}
uint findNextCodeBlock(
    uint i             /// Assumed to be start of a line
  , uint max           /// The maximum to look through, should be the length from caller.
  , Accessor &styler   /// passed on from Colourise and Fold
){
  /*! Finds the next block of code denoted by "<<.*>>="
   *
   */
  #ifdef DEBUG
  dbg << rnwmsg <<  thisfunc
      << "(" << i << ',' << max << ")" << endl;
  #endif
  do{ /// shortcuts search by looking though line
    int line = styler.GetLine(i);
    uint j = styler.LineStart(line + 1);
         i = styler.LineStart(i);
    if( styler.Match(i, "<<") ){
      i = ScanTo(i, j, ">>=", styler);
      #ifdef DEBUG
      dbg << rnwmsg <<  thisfunc << "(" << i << ',' << j << ")" << endl;
      #endif
      if(i == j){
        i = j;
        break;
      } else {
        return i;  /// @returns the position.
      }
    } else {
      #ifdef DEBUG
      dbg << rnwmsg <<  "No Match" << "(" << i << ',' << j << ',' << max << ")" << endl;
      #endif
      i=j;
    }
  } while(i < max);
  return i;
}

}

void ColouriseDoc(
    unsigned int startPos
  , int length
  , int initStyle
  , WordList *keywordlists[]
  , Accessor &styler)
{
  #ifdef DEBUG
  dbg << rnwmsg << "in " << thisfunc
      << "(" << startPos << ", " << length
      << deparseStyle(initStyle) << ")"
      << endl;
  #endif // DEBUG

  unsigned int lengthDoc = startPos+length ;
  for ( unsigned int i = startPos; i < lengthDoc; ) {
    char style = styler.StyleAt(i);
    uint nextchange = length;
    if ( i >= lengthDoc ) break ;
    if ( isInTexState(i, styler) ) {
      // nextchange = findNextCodeBlock(i, lengthDoc, styler);
      #ifdef DEBUG
      dbg << rnwmsg << "Tex_state, lengthDoc=" << lengthDoc
                    << ", nextchange="         << nextchange << endl;
      #endif
      TeX::ColouriseDoc(startPos, nextchange, style, keywordlists, styler);
      i=nextchange;
    } else
    if ( isInRState(i, styler) ) {

    } else
    if ( isInCodeBlockState(i, styler) ) {

    }
  }
}

void FoldDoc(
    unsigned int startPos       /// Starting Position, assumed to be start of a line
  , int length                  /// length to be analyzed
  , int initStyle               /// initial style
  , WordList *keywordlists[]  /// Word Lists
  , Accessor &styler            /// Accessor object
){
  #ifdef DEBUG
  dbg << rnwmsg << "in " << thisfunc
      << "(" << startPos
      << ", " << length
      << ", " << deparseStyle(initStyle)
      << ")" << endl;
  #endif //DEBUG
	unsigned int lengthDoc = startPos + length;
	int lineCurrent = styler.GetLine(startPos);
	int levelCurrent = SC_FOLDLEVELBASE;
  // initialization
	if (lineCurrent > 1)
		levelCurrent = styler.LevelAt(lineCurrent) & SC_FOLDLEVELNUMBERMASK;
	else
		styler.SetLevel(lineCurrent, SC_FOLDLEVELBASE);
	int levelNext = levelCurrent;
	char chNext = styler[startPos];
	int styleNext = styler.StyleAt(startPos);
	int style = initStyle;
	char s[10];
}

}}} // end namespace RnwLang::Lexers::Rnw

