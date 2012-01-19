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
namespace { // character class helpers
typedef unsigned int uint;
bool inline isWhite(const char ch){
  return _tisspace(ch);
}
inline bool IsAWordChar(const int ch) {
	return (ch < 0x80) && (isalnum(ch) || ch == '.' || ch == '_');
}
inline bool IsAWordStart(const int ch) {
	return (ch < 0x80) && (isalnum(ch) || ch == '_');
}
}
namespace { // state checkers
bool inline isInTexState(const char state){
  return TEX_DEFAULT <= state && state < TEX_END;
}
bool inline isInRState(const char state){
  return R_DEFAULT <= state && state < R_END;
}
bool inline isInCodeBlockState(const char state){
  return RNW_DEFAULT <= state && state < RNW_END;
}

string classifyLine(int line, LexAccessor& styler){
  if(isInTexState(styler.StyleAt(styler.LineStart(line)))) return "TeX";
  if(isInCodeBlockState(styler.StyleAt(styler.LineStart(line)))) return "Code";
  if(isInRState(styler.StyleAt(styler.LineStart(line)))) return "R";
  return "wft!?";
}
}
namespace { //finder functions
int ScanTo(uint pos, const uint max,  char const * const s, LexAccessor &styler){
  #ifdef DEBUG
  dbg << rnwmsg <<  thisfunc << endl;
  #endif
  uint i=pos;
  for(; i < max; i++){
    if(styler.Match(i, s)) return static_cast<int>(i-pos); 
  }
  /// @return the offset from pos if found, otherwise -1 for not found.
  return -1;
}

bool inline isNewCodeChunkLine(
    uint line             /// number of line to check
  , LexAccessor &styler   /// passed on from Colourise and Fold
){
  /*! Checks a line for a new code block
   *  
   *  New code blocks are defined by the pattern "^<<.*>>=.*"
   */
  static const char * endheader = ">>=";
  uint j = styler.LineStart(line);
  if( styler.Match(j, "<<") ){
    if(ScanTo(j+2, styler.LineStart(line+1)-2, endheader, styler) > -1) {
      dbg << rnwmsg <<  "both << >>= found on line " << line  << endl;
      return true;
      /// @returns if the line is a 
    }
  }
  return false;
}
bool inline isCodeReuseLine(
    int line               /// line number of line to check
  , LexAccessor &styler       /// passed on from Colourise and Fold
) {
  uint j = styler.LineStart(line);
  uint nextlinestart = styler.LineStart(line+1);
  if( styler.Match(j, "<<") ){
    int endtags = ScanTo(j+2, nextlinestart-2, ">>", styler);
    if (endtags>-1) {
      if(styler.SafeGetCharAt(j+endtags+2)=='=') return false;
      else return true;
    } else return false;
  }
  return false;
}
bool inline isEndCodeLine(
    int line               /// line number of line to check
  , LexAccessor &styler       /// passed on from Colourise and Fold
) {
  /*! check for end code and begin documentation
   *  
   *  according to the Sweave manual 
   *  (http://www.stat.uni-muenchen.de/~leisch/Sweave/Sweave-manual.pdf)
   *  The code chucks end with @ followed by whitespace.
   *  Also the remainder of the line is assumed as a comment, although 
   *  knitr does not reflect this by default.
   */
  uint j = styler.LineStart(line);
  if(styler.Match(j,"@")){
    if(isWhite(styler.SafeGetCharAt(j+1))) return true;
    else return false;
  }
  return false;
}
// Find Functions //////
inline int findNextCodeBlockLine(
    uint line
  , uint maxline
  , LexAccessor &styler   /// passed on from Colourise and Fold
) {
  for(; line <= maxline; line++){
    if( isNewCodeChunkLine(line, styler) ){
      return line;
    }
  }
  return -1;
}
uint findNextCodeBlock(
    uint i             /// Assumed to be start of a line
  , uint max           /// The maximum to look through, should be the length from caller.
  , LexAccessor &styler   /// passed on from Colourise and Fold
){
  /*! Finds the next block of code denoted by "<<.*>>="
   *
   */
  #ifdef DEBUG
  dbg << rnwmsg <<  thisfunc
      << "(" << i << ',' << max << ")" << endl;
  #endif
  /// shortcuts search by looking though lines
  int line = findNextCodeBlockLine(styler.GetLine(i), styler.GetLine(max), styler);
  if(line ==-1)
    return max;
  else
    return styler.LineStart(line)-1;
}
inline int findEndBlockLine (
    uint line
  , uint maxline
  , LexAccessor &styler   /// passed on from Colourise and Fold
) {
  for(; line < maxline; line++) {
    if(isEndCodeLine(line, styler))
      return line; 
      /// @return returns the line starting with '\@'
  }
  return -1; /// @return -1 if not found
}
inline int findEndBlock(
    uint i             /// Assumed to be start of a line
  , uint max           /// The maximum to look through, should be the length from caller.
  , LexAccessor &styler   /// passed on from Colourise and Fold
) {
  int eline = findEndBlockLine(styler.GetLine(i), styler.GetLine(max), styler);
  if(eline >-1){
    return styler.LineStart(eline+1)-2; 
    /// @return returns the last position in the line with '@' if found.
  }
  return max; /// @return max if end of block not found.
}
inline int findPrevStartBlockLine(
    int line
  , LexAccessor &styler
) {
  while(isNewCodeChunkLine(line,styler)) --line;
  return line; /// @return will give a -1 if not found; 
}

int findNextCodeReuseLine(
    uint line
  , uint maxline
  , LexAccessor &styler   /// passed on from Colourise and Fold
) {
  for(; line < maxline; line++) {
    if(isCodeReuseLine(line, styler))
      return line; 
      /// @return returns the line starting with '\@'
  }
  return -1; /// @return -1 if not found
}
inline int findNextCodeReuse(
    uint i             /// Assumed to be start of a line
  , uint max           /// The maximum to look through, should be the length from caller.
  , LexAccessor &styler   /// passed on from Colourise and Fold
) {
  int line = findNextCodeReuseLine(styler.GetLine(i), styler.GetLine(max), styler);
  if(line >-1){
    return styler.LineStart(line+1)-2; 
    /// @return returns the last position in the line with '@' if found.
  }
  return max; /// @return max if end of block not found.
}

inline uint findEndRCode(uint i, uint max, LexAccessor &styler){
  int line    = styler.GetLine(i),
      maxline = styler.GetLine(max);
  int lineend   = findEndBlockLine(line, maxline, styler);
  if(lineend==-1)
    lineend = maxline;
  int nexthead  = findNextCodeBlockLine(line, lineend, styler);
  if(nexthead!=-1)
    lineend = nexthead;
  int nextreuse = findNextCodeReuseLine(line, lineend, styler);
  if(nextreuse!=-1)
    lineend = nextreuse;
  if(lineend==maxline)
    return max;
  return styler.LineStart(lineend)-2;
}
}
namespace { // Style Functions //////////////////////
void StyleCodeInHead(
    uint j              /// first char in range
  , uint end            /// last char in range, inclusive
  , char bgstyle        /// default style
  , LexAccessor &styler /// document accessor
  , WordList *keys=NULL ///  word list
  , bool usekeys=false  ///  Should any word= be a keyword?  
){
  #ifdef DEBUG
  dbg << rnwmsg << "in " << thisfunc
      << "("    << j << ", " << end
      << ", "   << deparseStyle(bgstyle) << ")"
      << endl;
  #endif // DEBUG
  StyleContext sc(j, end-j+1, bgstyle, styler);
  for(;sc.More(); sc.Forward()){ // scan through declarations
    dbg << rnwmsg << "StlyeCodeInHead:for loop:" 
        << " position=" << sc.currentPos
        << " ch=" << ctos(sc.ch) << endl;
    if(sc.state == RNW_OPERATOR){
      dbg << rnwmsg << "in RNW_OPERATOR" << endl;
      sc.SetState(bgstyle);
    }
    if(sc.state == bgstyle){
      if(IsAWordStart(sc.ch)){
        dbg << rnwmsg << "in bgstyle=" << deparseStyle(bgstyle) 
            << " usekeys="  << usekeys
            << endl;
        if(usekeys){
          char s[100];
          sc.GetCurrent(s, sizeof(s));
          if(keys->InList(s))
            sc.SetState(RNW_KEYWORD);
        } else {
          sc.SetState(RNW_KEYWORD);
        }
      }
    } else 
    if (sc.ch=='='){
      dbg << rnwmsg << "in found '='" << endl;
      sc.SetState(RNW_OPERATOR);
    }
    if (sc.state == RNW_KEYWORD){
      // continue if still keyword
      dbg << rnwmsg << "in RNW_KEYWORD" << endl;
      if(!IsAWordChar(sc.ch))
        sc.SetState(bgstyle);
    }
  }
  dbg << rnwmsg << "Completing sc" << endl;
  sc.Complete();
  dbg << rnwmsg << "sc COMPLETED!" << endl;
}
void StyleCodeHeader(
    int line            ///  line to style
  , LexAccessor &styler    ///  Style Accessor
  , WordList *keys=NULL ///  word list
  , bool usekeys=false  ///  Should any word= be a keyword?  
){
  /*! Style the header of a code block
   *  
   *  Assumes a correctly formatted line with << and >>=
   */
  #ifdef DEBUG
  dbg << rnwmsg << "in " << thisfunc
      << "(" << line << ")"
      << endl;
  #endif // DEBUG
  uint j   = styler.LineStart(line);
  uint max = styler.LineStart(line+1)-1;
  // styler.ColourTo(j+2, RNW_DEFAULT);
  int endhead = j+ScanTo(j, max, ">>=", styler);
  styler.StartSegment(j);
  styler.ColourTo(endhead+3, RNW_DEFAULT);

  StyleCodeInHead(j+2, endhead-1, RNW_DEFAULT, styler, keys, usekeys);
  dbg << rnwmsg << "styling from " << endhead << " to " << endhead +3 << endl;
  //styler.StartSegment(endhead);
  if(max > static_cast<uint>(endhead+3)){
    styler.StartSegment(endhead+4);
    styler.ColourTo(max, RNW_COMMENT);
  }
}
void StyleCodeEnd(
    int line            ///  line to style
  , LexAccessor &styler    ///  Style Accessor
) {
 /*! Style end line
  *  
  *  Assumes a correctly formatted line with "@\s+comments"
  *  NOTE: Default knitr does not recognize lines with comments
  */
  #ifdef DEBUG
  dbg << rnwmsg << "in " << thisfunc
      << "(" << line << ")"
      << endl;
  #endif // DEBUG
  uint j   = styler.LineStart(line);
  uint max = styler.LineStart(line+1);
  
  StyleContext sc(j, max-j, RNW_DEFAULT, styler);
  if(sc.ch=='@'){
    dbg << rnwmsg << "style @:"<< sc.ch << "/" << RNW_OPERATOR<< endl;
    sc.SetState(RNW_OPERATOR);
    sc.Forward();
  }
  if(isWhite(sc.ch)) {
    dbg << rnwmsg << " whitespace found. " << sc.atLineEnd << endl;
    for(; sc.More() && (!sc.atLineEnd); sc.Forward()){
      dbg << rnwmsg << "StyleCodeEnd:for loop:" 
          << sc.currentPos << "/" << max
          << "(" << sc.ch << ")"
          << endl;
      if(sc.state == RNW_COMMENT) continue;
      if(isWhite(sc.ch)){
        dbg << rnwmsg << "in whitespace" << endl;
        sc.SetState(RNW_DEFAULT); //causing problems
        dbg << rnwmsg << "leaving whitespace" << endl;
        continue;
      }
      dbg << rnwmsg << "setting comment" << endl;
      sc.SetState(RNW_COMMENT);
    }
  }
  dbg << rnwmsg << "Completing sc" << endl;
  sc.Complete();
  dbg << rnwmsg << "exiting" << thisfunc << endl;
}
void StyleCodeReuse(
    int line            ///  line to style
  , LexAccessor &styler    ///  Style Accessor
  , WordList *keys=NULL ///  word list
  , bool usekeys=false  ///  Should any word= be a keyword?  
){
  #ifdef DEBUG
  dbg << rnwmsg << "in " << thisfunc
      << "(" << line << ")"
      << endl;
  #endif // DEBUG
  uint j   = styler.LineStart(line);
  uint max = styler.LineStart(line+1)-1;
  styler.StartSegment(j);
  styler.ColourTo(j+2, RNW_REUSE);
  int endhead = ScanTo(j, max, ">>", styler);

  StyleCodeInHead(j+2, endhead-1, RNW_REUSE, styler, keys, usekeys);
  styler.StartSegment(endhead);
  styler.ColourTo(endhead+2, RNW_REUSE);
  /// no comments allowed on reuse lines
}

void CorrectRStyle(uint start, uint end, int offset, LexAccessor styler){
  #ifdef DEBUG
  dbg << rnwmsg << "in " << thisfunc
      << "(" << start << "," << end << ", " << (offset) << ")"
      << endl;
  #endif // DEBUG
  for(uint i=start; i<end; i++) {
    int style = styler.StyleAt(i);
    StyleContext sc(start, end-start, style, styler);
    for(;sc.More();sc.Forward()){
      style = sc.state + offset;
      sc.SetState(style);    
    }
  }
}
        

} // end anonymous namespace. 
namespace { // fold Functions
  inline void FoldCarryover(int line, LexAccessor& styler){
    #ifdef DEBUG
      dbg << rnwmsg << thisfunc 
          << "("  << line
          << ", " << hex << styler.LevelAt(line) << dec
          << ")"  << endl;
    #endif 
    styler.SetLevel(line+1, styler.LevelAt(line));
  }
  inline void FoldCarryoverFlags(int line, LexAccessor& styler){
    #ifdef DEBUG
      dbg << rnwmsg << thisfunc 
          << "("  << line
          << ", " << hex << styler.LevelAt(line) << dec
          << ")"  << endl;
    #endif 
    styler.SetLevel(line+1, styler.LevelAt(line) & ~SC_FOLDLEVELNUMBERMASK);
  }
  inline void FoldEndCode(int line, LexAccessor& styler){
    int prevBlock = findPrevStartBlockLine(line, styler);

    
  }  
}
LexerRnw Rnw;

LexerRnw::LexerRnw(){
  #ifdef DEBUG
  dbg << rnwmsg << "in " << thisfunc << endl;
  dbg << rnwmsg << "Wordlists:numWordLists=" << numWordLists << endl;
  dbg << rnwmsg << "Wordlists:list 1=" << keyWordLists[1]->len << endl;
  dbg << rnwmsg << "Wordlists:list 2=" << keyWordLists[2]->len << endl;
  dbg << rnwmsg << "Wordlists:list 3=" << keyWordLists[3]->len << endl;
  dbg << rnwmsg << "Wordlists:list 4=" << keyWordLists[4]->len << endl;
  dbg << rnwmsg << "Wordlists:list 5=" << keyWordLists[5]->len << endl;
  dbg << rnwmsg << "Wordlists:list 6=" << keyWordLists[6]->len << endl;
  dbg << rnwmsg << "Wordlists:list 7=" << keyWordLists[7]->len << endl;
  dbg << rnwmsg << "Wordlists:list 8=" << keyWordLists[8]->len << endl;
  #endif
  

  // Rnw Options
  Rnw_props.Set("dummy", "3");
  Rnw_props.Set("fold.code", "1");
  Rnw_props.Set("fold.at.new", "1");
  // R Options
  R_props.Set("fold.compact", "0");
  R_props.Set("fold.at.else", "1");
  // TeX options
  TeX_props.Set("lexer.tex.comment.process", "0");
  TeX_props.Set("lexer.tex.use.keywords", "0");
  TeX_props.Set("lexer.tex.auto.if", "0");
  TeX_props.Set("lexer.tex.interface.default", "0");
  TeX_props.Set("fold.compact", "0");
  TeX_props.Set("fold.comment", "1");
  #ifdef DEBUG
    string rsetting = "R Setting: ";
    string texsetting = "TeX Setting: ";
    dbg << rnwmsg <<             "dummy="        << R_props.GetInt("dummy",-1) << endl;
    dbg << rnwmsg << rsetting << "fold.compact=" << R_props.GetInt("fold.compact",-1) << endl;
    dbg << rnwmsg << rsetting << "fold.at.else=" << R_props.GetInt("fold.at.else",-1) << endl;
    dbg << rnwmsg <<texsetting<< "lexer.tex.comment.process"  << "=" <<  TeX_props.GetInt("lexer.tex.comment.process"  , -1) << endl;
    dbg << rnwmsg <<texsetting<< "lexer.tex.use.keywords"     << "=" <<  TeX_props.GetInt("lexer.tex.use.keywords"     , -1) << endl;
    dbg << rnwmsg <<texsetting<< "lexer.tex.auto.if"          << "=" <<  TeX_props.GetInt("lexer.tex.auto.if"          , -1) << endl;
    dbg << rnwmsg <<texsetting<< "lexer.tex.interface.default"<< "=" <<  TeX_props.GetInt("lexer.tex.interface.default", -1) << endl;
    dbg << rnwmsg <<texsetting<< "fold.compact"               << "=" <<  TeX_props.GetInt("fold.compact"               , -1) << endl;
    dbg << rnwmsg <<texsetting<< "fold.comment"               << "=" <<  TeX_props.GetInt("fold.comment"               , -1) << endl;
    
  #endif
}
LexerRnw::~LexerRnw(){
  dbg << rnwmsg << "in " << thisfunc << "LexerRnw DESTRUCTED!!"  << endl;
}
int LexerRnw::WordListSet(int n, const char *wl) {
  dbg << rnwmsg << thisfunc
      << "("  << n
      << ", " << static_cast<const void*>(wl)
      << ")"  << endl;
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
    dbg << rnwmsg << "in try" << endl;
		Style(startPos, length, initStyle, pAccess, false);
	} catch (...) {
		// Should not throw into caller as may be compiled with different compiler or options
		pAccess->SetErrorStatus(SC_STATUS_FAILURE);
	}
}
void SCI_METHOD LexerRnw::Fold(unsigned int startPos, int length, int initStyle, IDocument *pAccess) {
  dbg << rnwmsg << "in " << thisfunc << endl;
	try {
    dbg << rnwmsg << "in try" << endl;
		Style(startPos, length, initStyle, pAccess, true);
	} catch (...) {
    dbg << rnwerr << "unhandled execption in " << thisfunc << endl; 
		//! Should not throw into caller as may be compiled with different compiler or options
		pAccess->SetErrorStatus(SC_STATUS_FAILURE);
	}
  dbg << rnwmsg << "leaving " << thisfunc << endl;
}
void LexerRnw::Style(unsigned int startPos, int length, int initStyle, IDocument* pAccess, bool fold){
  #ifdef DEBUG
  dbg << rnwmsg << "in " << thisfunc
      << "(startPos=" << startPos 
      << ", length=" << length
      << ", initStyle=" << deparseStyle(initStyle) << ")"
      << endl;
  if(!fold){
    dbg << rnwmsg << "Wordlists:list 1=" << keyWordLists[1]->len << endl;
    dbg << rnwmsg << "Wordlists:list 2=" << keyWordLists[2]->len << endl;
    dbg << rnwmsg << "Wordlists:list 3=" << keyWordLists[3]->len << endl;
    dbg << rnwmsg << "Wordlists:list 4=" << keyWordLists[4]->len << endl;
    dbg << rnwmsg << "Wordlists:list 5=" << keyWordLists[5]->len << endl;
    dbg << rnwmsg << "Wordlists:list 6=" << keyWordLists[6]->len << endl;
    dbg << rnwmsg << "Wordlists:list 7=" << keyWordLists[7]->len << endl;
    dbg << rnwmsg << "Wordlists:list 8=" << keyWordLists[8]->len << endl;
  }
  #endif // DEBUG

  bool foldAtNew = false;//Rnw_props.GetInt("fold.at.new")==1;
  bool foldCode  = false;//Rnw_props.GetInt("fold.code")==1;
  
  LexAccessor  styler(pAccess);
  Accessor TeX_styler(pAccess, &TeX_props);
  Accessor   R_styler(pAccess,   &R_props);
  WordList EmptyWL;
  WordList * R_words[] = {keyWordLists[1],keyWordLists[2],keyWordLists[3],
                          &EmptyWL, &EmptyWL, &EmptyWL, &EmptyWL, &EmptyWL, &EmptyWL};
  WordList * TeX_words[] = {&EmptyWL, &EmptyWL, &EmptyWL, &EmptyWL, 
                            &EmptyWL, &EmptyWL, &EmptyWL, &EmptyWL, &EmptyWL};

  unsigned int lengthDoc = startPos+length ;
  char style = initStyle;
  for ( unsigned int i = startPos; i < lengthDoc; ) {
    #ifdef DEBUG
    dbg << rnwmsg << " Style: for loop: i=" << i << " /" << lengthDoc 
                  << ",  style=" << deparseStyle(style) << endl;
    #endif
    uint nextchange = length;
    if (i >= lengthDoc) break ;
    if (isInTexState(style)) {
      #ifdef DEBUG
      dbg << rnwmsg << "TeX state: i=" << i
                    << " , lengthDoc=" << lengthDoc
                    << endl;
      #endif
      nextchange = findNextCodeBlock(i, lengthDoc, styler);
      #ifdef DEBUG
      dbg << rnwmsg << "TeX state:  nextchange=" << nextchange << endl;
      #endif
      if(nextchange > i){
        // corrects for adjacent blocks
        if(fold){
          dbg << rnwmsg << "TeX Folding"  << endl;
          TeX::FoldDoc(i, nextchange-i+1, style, TeX_words, TeX_styler);
        } 
        else {
          dbg << rnwmsg << "TeX Coloring" << endl;
          TeX::ColouriseDoc(i, nextchange-i, style, TeX_words, TeX_styler);
        }
        i=nextchange;
      }
      style=RNW_DEFAULT;
      #ifdef DEBUG
      dbg << rnwmsg << "end TeX state, lengthDoc=" << lengthDoc
                    << ", i=" << i << endl;
      #endif
    } else
    if (isInCodeBlockState(style)) {
      int line = styler.GetLine(i);
      i = styler.LineStart(line+1);
      #ifdef DEBUG
      dbg << rnwmsg << "Code state, i=" << i
                    << ", lengthDoc=" << lengthDoc 
                    << ", line=" << line 
                    << endl;
      #endif
      if(isEndCodeLine(line, styler)){
        dbg << rnwmsg << "This is an end code line." << endl;
        if(fold){
          FoldEndCode(line, styler);
          FoldCarryover(line, styler);
        } else{
          StyleCodeEnd(line, styler);
        }
        i = styler.LineStart(line+1);
        style = TEX_DEFAULT;
        dbg << rnwmsg << "end End Code Line. moving to i=" << i << endl; 
      }
      else {
        uint codeend = findEndRCode(i, lengthDoc, styler);
        if(isNewCodeChunkLine(line, styler)){
          dbg << rnwmsg << "New code line." << endl;
          i = styler.LineStart(line+1);
          if(fold){
            FoldCarryover(line, styler);
            R::FoldDoc(i, codeend-i+1, 0, R_words, R_styler);
          }
          else {
            StyleCodeHeader(line, styler);
            R::ColouriseDoc(i, codeend-i, 0, R_words, R_styler);
          }
          style=RNW_DEFAULT;
          i=styler.LineStart(styler.GetLine(codeend)+1);
          dbg << rnwmsg << "end New Code Line" << endl; 
        } 
        else if(isCodeReuseLine(line, styler)){
          dbg << rnwmsg << "Reuse code line." << endl;
          if(fold) {
            FoldCarryover(line, styler);
          } else {
            StyleCodeReuse(line, styler);
          }
          i = styler.LineStart(line+1);
          style=R_DEFAULT;
          dbg << rnwmsg << "end Code Reuse Line" << endl; 
        }
      } 
    } else
    if (isInRState(style)) {
      #ifdef DEBUG
      dbg << rnwmsg << "R state, i=" << i
                    << ", lengthDoc=" << lengthDoc << endl;
      #endif
      uint codeend = findEndRCode(i, lengthDoc, styler);
      if(fold){
        R::FoldDoc(i, codeend-i+1, 0, R_words, R_styler);
      } else {
        R::ColouriseDoc(i, codeend-i, 0, R_words, R_styler);
      }
      style=RNW_DEFAULT;
      i=styler.LineStart(styler.GetLine(codeend)+1);
    }
  }
  styler.Flush();
  #ifdef DEBUG
    for(int line  = styler.GetLine(startPos);
            line <= styler.GetLine(lengthDoc);
            line ++) {
      if(fold){
        dbg << rnwmsg << "Levels:" 
            << "line=" << line
            << " level=" << hex << styler.LevelAt(line) << dec
            << endl;
      } 
      else {
        dbg << rnwmsg << "Class:"
            << "line=" << line
            << " class=" << classifyLine(line, styler)
            << endl;
      }
    }
  #endif
}
}}} // end namespace RnwLang::Lexers::Rnw

