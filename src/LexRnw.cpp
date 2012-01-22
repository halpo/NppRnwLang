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
#include <stdexcept>

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
inline bool isEOL(const int ch){
  return (ch == '\r') || (ch == '\n');
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
string cl1(int style){
  if(isInTexState(style)) return "T";
  if(isInCodeBlockState(style)) return "C";
  if(isInRState(style)) return "R";
  return "W";  
}
string classifyLine(int line, LexAccessor& styler, bool text=true){
  stringstream s;
  for(int pos = styler.LineStart(line); pos < styler.LineStart(line+1); ++pos){
    if(text)
      s << cl1(styler.StyleAt(pos));
    else
      s << setw(2) << static_cast<int>(styler.StyleAt(pos));
  }
  return s.str();
}
}
namespace { // Finder functions
int ScanTo(uint pos, const uint max,  char const * const s, LexAccessor &styler){
  #ifdef DEBUG
  dbg << rnwmsg <<  thisfunc 
      << "("  << pos 
      << ", " << max
      << ", " << s
      << ")" << endl;
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
  if(styler.SafeGetCharAt(j)=='@' && isWhite(styler.SafeGetCharAt(j+1,'\n')))
    return true;
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
  if(line == -1)
    return max;
  else
    return styler.LineStart(line);
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
    return styler.LineStart(eline+1); 
    /// @return returns the first character in the new block.
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
  dbg << thisfunc
      << "("  << i
      << ", " << max
      << ")"  << endl;
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
  return styler.LineStart(lineend);
}
}
namespace { // Style Functions
void StyleCodeInHead(
    uint j              /// first char in range
  , uint end            /// end of range, exclusive
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
  StyleContext sc(j, end-j, bgstyle, styler);
  for(;sc.More(); sc.Forward()){ // scan through declarations
    dbg << rnwmsg << "StlyeCodeInHead:for loop:" 
        << " position=" << sc.currentPos
        << " ch=" << ctos(sc.ch) << endl;
    if(sc.state == RNW_OPERATOR){
      dbg << rnwmsg << "in RNW_OPERATOR" << endl;
      sc.SetState(bgstyle);
    }
    if(IsAWordStart(sc.ch)){
      dbg << rnwmsg << "in bgstyle=" << deparseStyle(bgstyle) 
          << " usekeys="  << usekeys
          << endl;
      if(usekeys){
        char s[100];
        sc.GetCurrent(s, sizeof(s));
        if(keys->InList(s))
          sc.SetState(RNW_KEYWORD);
      } 
      else {
        sc.SetState(RNW_KEYWORD);
      }
    }
    if (sc.state == RNW_KEYWORD){
      // continue if still keyword
      dbg << rnwmsg << "in RNWSetState_KEYWORD" << endl;
      if(!IsAWordChar(sc.ch))
        sc.SetState(bgstyle);
    }
    if (sc.ch == '=' || sc.ch == ','){
      dbg << rnwmsg << " found " << ctos(sc.ch) << endl;
      sc.SetState(RNW_OPERATOR);
    }
  }
  dbg << rnwmsg << "Completing sc " 
      << "currentPos=" << sc.currentPos
      << endl;
  sc.Complete();
  dbg << rnwmsg << "sc COMPLETED!" << endl;
}
void StyleCodeHeader(
    int line            ///  line to style
  , LexAccessor &styler ///  Style Accessor
  , WordList *keys=NULL ///  word list
  , bool usekeys=false  ///  Should any word= be a keyword?
  , int toEnd=-1
) {
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
  uint max = styler.LineStart(line+1);
  if(styler.GetLine(max)==line)max -= 1;
  styler.StartAt(j);
  styler.StartSegment(j);
  dbg << rnwmsg 
      << "StartSegment=" << styler.GetStartSegment() 
      << ", max=" << max
      << ", length=" << styler.Length()
      << endl;
  styler.ColourTo(max-1, RNW_DEFAULT);
  dbg << rnwmsg << "Flushing" << endl;
  styler.Flush();
  dbg << rnwmsg << __func__
      << "  j="   << j   << "?=" << styler.GetStartSegment()
      << ", max=" << max
      << endl;
  dbg << rnwmsg << __func__ 
      << ":StyleAt line " << j 
      << " is " << classifyLine(line,styler,false)
      << endl;
  if(toEnd<0)
    toEnd = ScanTo(j, max, ">>=", styler);
  uint endhead = j+toEnd;
  StyleCodeInHead(j+2, endhead, RNW_DEFAULT, styler, keys, usekeys);
  dbg << rnwmsg << "styling from " << endhead << " to " << endhead +3 << endl;
  if(max-1 > static_cast<uint>(endhead + 3)){
    styler.StartAt(endhead + 3);
    styler.StartSegment(endhead + 3);
    styler.ColourTo(max-1, RNW_COMMENT);
  }
  styler.Flush();
}
void StyleCodeEnd(
    int line            ///  line to style
  , LexAccessor &styler ///  Style Accessor
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
  if(styler.GetLine(max)==line) max -= 1;
  
  dbg << rnwmsg << __func__
      << "  j=" << j
      << ", max=" << max
      << endl;
  StyleContext sc(j, max-j-1, RNW_DEFAULT, styler);
  for(; sc.More() && (!sc.atLineEnd); sc.Forward()){
    dbg << rnwmsg << "StyleCodeEnd:for loop:" 
        << sc.currentPos << "/" << max
        << "(" << ctos(sc.ch) << ")"
        << endl;
    if(sc.state == RNW_COMMENT) continue;
    if(sc.state == RNW_OPERATOR){
      dbg << rnwmsg << __func__ << ":Resetting to RNW_DEFAULT" << endl;
      sc.SetState(RNW_DEFAULT);
    }
    if(sc.state==RNW_DEFAULT) {
      if(sc.ch=='@'  && sc.atLineStart){
        dbg << rnwmsg << "style @:"<< ctos(sc.ch) << "->" << deparseStyle(RNW_OPERATOR) << endl;
        sc.SetState(RNW_OPERATOR);
        sc.ForwardSetState(RNW_DEFAULT);
      } else throw logic_error("Malformed end code line");
      if(isWhite(sc.ch)){
        dbg << rnwmsg << " whitespace found. " << sc.atLineEnd << endl;
        continue;
      }
      if(!isWhite(sc.ch) && isWhite(sc.chPrev)){
        dbg << rnwmsg << "setting comment" << endl;
        sc.SetState(RNW_COMMENT);
      }
    }
  }
  dbg << rnwmsg << "Completing sc("
      << sc.currentPos << ")"
      << endl;
  sc.Complete();
  dbg << rnwmsg << "exiting" << __func__ << endl;
}
void StyleCodeReuse(
    int line            ///  line to style
  , LexAccessor &styler ///  Style Accessor
  , WordList *keys=NULL ///  word list
  , bool usekeys=false  ///  Should any word= be a keyword?  
){
  #ifdef DEBUG
  dbg << rnwmsg << "in " << thisfunc
      << "(" << line << ")"
      << endl;
  #endif // DEBUG
  styler.SetLineState(line, RNW_REUSE);
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
LexerRnw::LexerRnw(){
  #ifdef DEBUG
  dbg << rnwmsg << "in " << thisfunc << endl;
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
    dbg << rnwerr << "Unhandled Exception" << endl;
		// Should not throw into caller as may be compiled with different compiler or options
		pAccess->SetErrorStatus(SC_STATUS_FAILURE);
	}
}
void SCI_METHOD LexerRnw::Fold(unsigned int startPos, int length, int initStyle, IDocument *pAccess) {
  dbg << rnwmsg << "in " << thisfunc << endl;
	try {
    dbg << rnwmsg << "in try" << endl;
		// Style(startPos, length, initStyle, pAccess, true);
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

  unsigned int endDoc = startPos+length ;
  char style = initStyle;
  for ( unsigned int i = startPos; i < endDoc; ) {
    #ifdef DEBUG
    dbg << rnwmsg << " Style: for loop: i=" << i << " /" << endDoc 
                  << ",  style=" << deparseStyle(style) << endl;
    #endif
    uint nextchange = length;
    if (i >= endDoc) break ;
    if (isInTexState(style)) {
      #ifdef DEBUG
      dbg << rnwmsg << "TeX state: i=" << i
                    << " , endDoc=" << endDoc
                    << endl;
      #endif 
      nextchange = findNextCodeBlock(i, endDoc, styler);
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
      dbg << rnwmsg << "end TeX state, endDoc=" << endDoc
                    << ", i=" << i << endl;
      #endif
    } else
    if (isInCodeBlockState(style)) {
      int line = styler.GetLine(i);
      uint end = styler.LineStart(line+1);
      #ifdef DEBUG
      dbg << rnwmsg << "Code state, i=" << i
                    << ", endDoc=" << endDoc 
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
        i = end;
        style = TEX_DEFAULT;
        styler.StartSegment(i);
        styler.ColourTo(i, style);
        dbg << rnwmsg << "end End Code Line. moving to i=" << i << endl; 
      }
      else {
        dbg << rnwmsg << "not an  end of code" << endl;
        uint codebegin = end;
        uint codeend   = findEndRCode(codebegin, endDoc, styler);
        if(isNewCodeChunkLine(line, styler)){
          dbg << rnwmsg << "New code line." 
              << "  codebegin=" << codebegin
              << ", codeend=" << codeend
              << endl;
          if(fold){
            FoldCarryover(line, styler);
            if(codeend > codebegin){
              // R::FoldDoc(codebegin, codeend-codebegin, R_DEFAULT, R_words, R_styler);
            }
          }
          else { // style
            StyleCodeHeader(line, styler);
            if(codeend > codebegin){
              R::ColouriseDoc(codebegin, codeend-codebegin, R_DEFAULT, R_words, R_styler);
            }
          }
          style=RNW_DEFAULT;
          i=codeend;
          dbg << rnwmsg << "end New Code Line" << endl; 
        } 
        else if(isCodeReuseLine(line, styler)){
          dbg << rnwmsg << "Reuse code line." << endl;
          if(fold) {
            FoldCarryover(line, styler);
          } else {
            StyleCodeReuse(line, styler);
          }
          i = end;
          style=R_DEFAULT;
          dbg << rnwmsg << "end Code Reuse Line" << endl; 
        } 
        else {
          dbg << rnwmsg << "unknown chunk line type" << endl;
          style=R_DEFAULT;
          //throw logic_error("Ill-formed code line");
        }
      } 
    } 
    else if (isInRState(style)) {
      #ifdef DEBUG
      dbg << rnwmsg << "R state, i=" << i
                    << ", endDoc=" << endDoc << endl;
      #endif
      uint codeend = findEndRCode(i, endDoc, styler);
      if(fold){
        // R::FoldDoc(i, codeend-i+1, 0, R_words, R_styler);
      } else {
        R::ColouriseDoc(i, codeend-i, 0, R_words, R_styler);
      }
      style=RNW_DEFAULT;
      i=codeend;
    }
  }
  styler.Flush();
  #ifdef DEBUG
    for(int line  = styler.GetLine(startPos);
            line <= styler.GetLine(endDoc);
            line ++) {
      if(fold){
        dbg << rnwmsg << "Levels:" 
            << "line=" << line
            << " level=" << hex << styler.LevelAt(line) << dec
            << endl;
      } 
      else {
        dbg << rnwmsg << "Class:"
            << "line=" << setw(3) << line
            << " class=" << classifyLine(line, styler, false)
            << endl;
        dbg << rnwmsg << "?end code:"
            << "line=" << setw(3) << line
            << " is end=" << isEndCodeLine(line, styler)
            << endl;
      }
    }
  #endif
}
}}} // end namespace RnwLang::Lexers::Rnw

