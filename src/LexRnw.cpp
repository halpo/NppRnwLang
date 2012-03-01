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
#include "LexerPlugin.h"
#include "PluginDebug.h"
#include <stdexcept>
#include <iomanip>


using namespace std;
using namespace RnwLang;
using namespace RnwLang::Lexers;


namespace RnwLang { namespace Lexers{ namespace Rnw{
int ScanTo(unsigned int pos, const unsigned int max,  char const * const s, LexAccessor &styler){
  unsigned int i=pos;
  for(; i < max; i++){
    if(styler.Match(i, s)) return static_cast<int>(i-pos); 
  }
  /// @return the offset from pos if found, otherwise -1 for not found.
  return -1;
}
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
      #ifdef DEBUG
      dbg << rnwmsg <<  "both << >>= found on line " << line  << endl;
      #endif
      return true;
      /// @returns if the line is a 
    }
  }
  return false;
}
bool inline isCodeReuseLine(
    int line               /// line number of line to check
  , LexAccessor &styler       /// passed on from Colourise and Fold
  , bool allowwhite=false
) {
  uint j = styler.LineStart(line);
  uint nextlinestart = styler.LineStart(line+1);
  while(allowwhite && isWhite(styler.SafeGetCharAt(j)))j++;
  if( styler.Match(j, "<<") ){
    #ifdef DEBUG
    dbg << rnwmsg << __func__ << ":found <<" << endl;
    #endif
    j+=2;
    int endtags = ScanTo(j, nextlinestart-1, ">>", styler);
    if (endtags > -1) {
      #ifdef DEBUG
      dbg << rnwmsg << __func__ << ":found >>" << endl;
      #endif
      j += endtags+2;
      for(char ch = styler.SafeGetCharAt(j, 0x0A);
          (isWhite(ch) || isEOL(ch));
          ch = styler.SafeGetCharAt(++j, 0x0A)){
        // loop to end of line.  Allow trailing white space only.
        #ifdef DEBUG
        dbg << rnwmsg << __func__ 
            << ": j=" << j
            << ": ch#=" << static_cast<int>(ch)
            << ", isEOL=" << isEOL(ch)
            << endl;
        #endif
        if(isEOL(ch)) return true;
        /// allows white space but no comments on reuse lines.
      }
    }
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
  while((!isNewCodeChunkLine(line,styler)) && (line >=0)) --line;
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
    return styler.LineStart(line+1); 
    /// @return returns the last position in the line with '@' if found.
  }
  return max; /// @return max if end of block not found.
}
inline uint findEndRCode(uint i, uint max, LexAccessor &styler){
  #ifdef DEBUG
  dbg << __func__
      << "("  << i
      << ", " << max
      << ")"  << endl;
  #endif
  if(!(i<max)) return max;
  int line     = styler.GetLine(i),
      maxline  = styler.GetLine(max),
      endrline = maxline+1;
  int lineend = findEndBlockLine(line, endrline, styler);
  if(lineend !=-1)
    endrline = lineend;
  int nexthead  = findNextCodeBlockLine(line, endrline, styler);
  if(nexthead!=-1)
    endrline = nexthead;
  int nextreuse = findNextCodeReuseLine(line, endrline, styler);
  if(nextreuse!=-1)
    endrline = nextreuse;
  unsigned int endr = styler.LineStart(endrline);
  if(endrline>=maxline && lineend==-1 && nexthead==-1 && nextreuse==-1)
    endr = max;
  #ifdef DEBUG
  dbg << rnwmsg << __func__
      << ":endr=" << endr
      << ", endrline=" << endrline
      << ", lineend=" << lineend
      << ", nexthead=" << nexthead
      << ", nextreuse=" << nextreuse
      << endl;
  #endif
  return endr;
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
      << ", "   << bgstyle << ")"
      << endl;
  #endif // DEBUG
  StyleContext sc(j, end-j, bgstyle, styler);
  for(;sc.More(); sc.Forward()){ // scan through declarations
    #ifdef DEBUG
    dbg << rnwmsg << "StlyeCodeInHead:for loop:" 
        << " position=" << sc.currentPos
        << " ch=" << ctos(sc.ch) << endl;
    #endif
    if(sc.state == RNW_OPERATOR){
      #ifdef DEBUG
      dbg << rnwmsg << "in RNW_OPERATOR" << endl;
      #endif
      sc.SetState(bgstyle);
    }
    if(IsAWordStart(sc.ch)){
      #ifdef DEBUG
      dbg << rnwmsg << "in bgstyle=" << bgstyle
          << " usekeys="  << usekeys
          << endl;
      #endif
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
      #ifdef DEBUG
      dbg << rnwmsg << "in RNWSetState_KEYWORD" << endl;
      #endif
      if(!IsAWordChar(sc.ch))
        sc.SetState(bgstyle);
    }
    if (sc.ch == '=' || sc.ch == ','){
      #ifdef DEBUG
      dbg << rnwmsg << " found " << ctos(sc.ch) << endl;
      #endif
      sc.SetState(RNW_OPERATOR);
    }
  }
  #ifdef DEBUG
  dbg << rnwmsg << "Completing sc " 
      << "currentPos=" << sc.currentPos
      << endl;
  #endif
  sc.Complete();
  #ifdef DEBUG
  dbg << rnwmsg << "sc COMPLETED!" << endl;
  #endif
}
void StyleCodeHeader(
    int line            ///  line to style
  , LexAccessor &styler ///  Style Accessor
  , WordList *keys=NULL ///  word list
  , bool usekeys=false  ///  Should any word= be a keyword?
  , int toEnd=-1        /// is toEnd already provided?
  , bool reuse=false    ///  style as reuse?  This disallows comments on line.
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
  styler.SetLineState(line,1);
  uint j   = styler.LineStart(line);
  uint max = styler.LineStart(line+1);
  if(styler.GetLine(max)==line){
    #ifdef DEBUG
    dbg << rnwmsg << __func__ << ":special case EOD." << endl;
    #endif
    max = styler.Length();
  }
  styler.StartAt(j);
  styler.StartSegment(j);
  #ifdef DEBUG
  dbg << rnwmsg 
      << "StartSegment=" << styler.GetStartSegment() 
      << ", max=" << max
      << ", length=" << styler.Length()
      << endl;
  #endif
  styler.ColourTo(max-1, RNW_DEFAULT);
  #ifdef DEBUG
  dbg << rnwmsg << "Flushing" << endl;
  #endif
  styler.Flush();
  #ifdef DEBUG
  dbg << rnwmsg << __func__
      << "  j="   << j   << "?=" << styler.GetStartSegment()
      << ", max=" << max
      << endl;
  dbg << rnwmsg << __func__ 
      << ":StyleAt line " << j 
      << " is " << classifyLine(line,styler,false)
      << endl;
  #endif
  if(toEnd<0)
    toEnd = ScanTo(j, max, reuse?">>":">>=", styler);
  uint endhead = j+toEnd;
  StyleCodeInHead(j+2, endhead, RNW_DEFAULT, styler, keys, usekeys);
  #ifdef DEBUG
  dbg << rnwmsg << "styling from " << endhead << " to " << endhead +3 << endl;
  #endif
  if(!reuse && max-1 > endhead + 3){
    /// No comments for reuse==true
    styler.StartAt(endhead + 3);
    styler.StartSegment(endhead + 3);
    styler.ColourTo(max-1, RNW_COMMENT);
  }
  SetEOLFilledAll(styler);
  styler.Flush();
  CheckEOLFilled(styler);
  #ifdef DEBUG
  dbg << rnwmsg << __func__ << ":leaving" << endl;
  #endif
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
  if(styler.GetLine(max)==line){
    #ifdef DEBUG
    dbg << rnwmsg << __func__ << ":Special case last line" << endl;
    #endif
    max = styler.Length();
    if(max-j == 1){
      #ifdef DEBUG
      dbg << rnwmsg << __func__ << ":special case only 1 char" << endl;
      #endif
      styler.StartAt(j);
      styler.StartSegment(j);
      styler.ColourTo(j, RNW_DEFAULT);
      #ifdef DEBUG
      dbg << rnwmsg << __func__ << ":leaving" << endl;
      #endif
      return;
    }
  }
  #ifdef DEBUG
  dbg << rnwmsg << __func__
      << " j=" << j
      << ", max=" << max
      << endl;
  #endif
  StyleContext sc(j, max-j, RNW_DEFAULT, styler);
  for(; sc.More() && (!sc.atLineEnd); sc.Forward()){
    #ifdef DEBUG
    dbg << rnwmsg << "StyleCodeEnd:for loop:" 
        << sc.currentPos << "/" << max
        << "(" << ctos(sc.ch) << ")"
        << endl;
    #endif
    if(sc.state == RNW_COMMENT) continue;
    if(sc.state == RNW_OPERATOR){
      #ifdef DEBUG
      dbg << rnwmsg << __func__ << ":Resetting to RNW_DEFAULT" << endl;
      #endif
      sc.SetState(RNW_DEFAULT);
    }
    if(sc.state==RNW_DEFAULT) {
      if(sc.ch=='@'  && sc.atLineStart){
        #ifdef DEBUG
        dbg << rnwmsg << "style @:"<< ctos(sc.ch) << "->" << RNW_OPERATOR << endl;
        #endif
        sc.SetState(RNW_OPERATOR);
      } else
      if(isWhite(sc.ch)){
        #ifdef DEBUG
        dbg << rnwmsg << " whitespace found. " << sc.atLineEnd << endl;
        #endif
        continue;
      } else
      if(!isWhite(sc.ch) && isWhite(sc.chPrev)){
        #ifdef DEBUG
        dbg << rnwmsg << "setting comment" << endl;
        #endif
        sc.SetState(RNW_COMMENT);
      }
    }
  }
  #ifdef DEBUG
  dbg << rnwmsg << "Completing sc("
      << sc.currentPos << ")"
      << endl;
  #endif
  sc.Complete();
  #ifdef DEBUG
  dbg << rnwmsg << "exiting" << __func__ << endl;
  #endif
}
void StyleCodeReuse(
    int line            ///  line to style
  , LexAccessor &styler ///  Style Accessor
  , WordList *keys=NULL ///  word list
  , bool usekeys=false  ///  Should any word= be a keyword?  
){
  StyleCodeHeader(line, styler, keys, usekeys,-1,true);
  return;
  #ifdef DEBUG
  dbg << rnwmsg << "in " << thisfunc
      << "(" << line << ")"
      << endl;
  #endif // DEBUG
  uint j   = styler.LineStart(line);
  uint max = styler.LineStart(line+1);
  if(styler.GetLine(max)==line){
    #ifdef DEBUG
    dbg << rnwmsg << __func__ << ":special case EOD." << endl;
    #endif
    max = styler.Length();
  }
  styler.StartAt(j);
  styler.StartSegment(j);
  styler.ColourTo(max-1, RNW_DEFAULT);
  int toEnd = ScanTo(j, max, ">>", styler);
  uint endhead = j+toEnd;

  StyleCodeInHead(j+2, endhead, RNW_DEFAULT, styler, keys, usekeys);
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
    if(line>0)styler.SetLevel(line+1, styler.LevelAt(line));
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
  inline void FoldHeadCode(int line, LexAccessor& styler, bool foldatnew=true){
    #ifdef DEBUG
    dbg << rnwmsg << __func__ 
        << "("  << line
        << ", styler"
        << ", " << foldatnew
        << ")"  << endl;
    #endif
    int prevstyle = styler.StyleAt(styler.LineStart(line-1));
    int prevlevel = styler.LevelAt(line-1);
    int newlevel  = -1;
    #ifdef DEBUG
    dbg << rnwmsg << __func__
        << ":prevstyle=" << prevstyle
        << ", prevlevel=" << hex << setw(4) << prevlevel << dec
        << endl;
    #endif
    if(isInRState(prevstyle)){  // new block with in R block.
      int prevcodestart = findPrevStartBlockLine(line-1, styler); 
      newlevel = styler.LevelAt(prevcodestart);
      if(foldatnew) {
        newlevel |= SC_FOLDLEVELHEADERFLAG;
      } else {
        newlevel += 1;
      }
    } else 
    if(isInCodeBlockState(prevstyle)) {  // new following close @
      if(isEndCodeLine(line, styler)){
        newlevel = prevlevel+1;
        newlevel |= SC_FOLDLEVELHEADERFLAG;
      } else if(isNewCodeChunkLine(line, styler)){
        if(foldatnew){
          newlevel = prevlevel;
          newlevel |= SC_FOLDLEVELHEADERFLAG;
        } else {
          newlevel = prevlevel+1;
        }
      }
    } 
    else {  // standard new from TeX.
      newlevel = prevlevel;
      newlevel |= SC_FOLDLEVELHEADERFLAG;
    }
    styler.SetLevel(line, newlevel);
    // styler.SetLevel(line+1, newlevel+1);
  }
  inline void FoldEndCode(int line, LexAccessor& styler){
    #ifdef DEBUG
    dbg << rnwmsg << __func__ 
        << "("  << line
        << ")"  << endl;
    #endif
    int prevBlock     = findPrevStartBlockLine(line-1, styler);
    int startingLevel = styler.LevelAt(prevBlock);
    styler.SetLevel(line  , SC_FOLDLEVELBASE | RNW_FOLD_R);
    styler.SetLevel(line+1, startingLevel  & SC_FOLDLEVELNUMBERMASK);
    #ifdef DEBUG
    dbg << rnwmsg << " leaving " << __func__ 
        << " prevBlock="  << prevBlock
        << ", startingLevel=" <<setw(4) << hex << startingLevel << dec
        << endl;
    #endif
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
  #ifdef DEBUG
  dbg << rnwmsg << "in " << thisfunc << "LexerRnw DESTRUCTED!!"  << endl;
  #endif
}
int LexerRnw::WordListSet(int n, const char *wl) {
  #ifdef DEBUG
  dbg << rnwmsg << thisfunc
      << "("  << n
      << ", " << static_cast<const void*>(wl)
      << ")"  << endl;
  #endif
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
  _debugenter_;
  try {
    LexerRnw* lex = new LexerRnw;
    return dynamic_cast<ILexer*>(lex);//new LexerRnw;
  } catch (...) {
    #ifdef DEBUG
    dbg << rnwerr << thisfunc << (": Unhandled Exception Caught") << endl;
    #endif
    // Should not throw into caller as may be compiled with different compiler or options
    return 0;
  }
}
void SCI_METHOD LexerRnw::Lex(unsigned int startPos, int length, int initStyle, IDocument *pAccess) {
  #ifdef DEBUG
  dbg << rnwmsg << "in " << thisfunc << endl;
  #endif
	try {
    #ifdef DEBUG
    dbg << rnwmsg << "in try" << endl;
    #endif
		Style(startPos, length, initStyle, pAccess, false);
	} catch (...) {
    #ifdef DEBUG
    dbg << rnwerr << "Unhandled Exception" << endl;
    #endif
		// Should not throw into caller as may be compiled with different compiler or options
		pAccess->SetErrorStatus(SC_STATUS_FAILURE);
	}
}
void SCI_METHOD LexerRnw::Fold(unsigned int startPos, int length, int initStyle, IDocument *pAccess) {
  #ifdef DEBUG
  dbg << rnwmsg << "in " << thisfunc << endl;
  #endif
	try {
    #ifdef DEBUG
    dbg << rnwmsg << "in try" << endl;
    #endif
		Style(startPos, length, initStyle, pAccess, true);
	} catch (...) {
    #ifdef DEBUG
    dbg << rnwerr << "unhandled execption in " << thisfunc << endl; 
    #endif
		//! Should not throw into caller as may be compiled with different compiler or options
		pAccess->SetErrorStatus(SC_STATUS_FAILURE);
	}
  #ifdef DEBUG
  dbg << rnwmsg << "leaving " << thisfunc << endl;
  #endif
}
void LexerRnw::Style(unsigned int startPos, int length, int initStyle, IDocument* pAccess, bool fold){
  #ifdef DEBUG
  dbg << rnwmsg << "in " << thisfunc
      << "(startPos=" << startPos 
      << ", length=" << length
      << ", initStyle=" << initStyle << ")"
      << endl;
  #endif // DEBUG

  bool foldAtNew = false;//Rnw_props.GetInt("fold.at.new")==1;
  bool foldCode  = true;//Rnw_props.GetInt("fold.code")==1;
  
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
    dbg << rnwmsg << "Style:loop(" << i << " /" << endDoc 
                  << "):style=" << style << endl;
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
          #ifdef DEBUG
          dbg << rnwmsg << "TeX Folding"  << endl;
          #endif
          TeX::FoldDoc(i, nextchange-i+1, style, TeX_words, TeX_styler);
        } 
        else {
          #ifdef DEBUG
          dbg << rnwmsg << "TeX Coloring" << endl;
          #endif
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
        #ifdef DEBUG
        dbg << rnwmsg << "This is an end code line." << endl;
        #endif
        if(fold){
          if(foldCode){
            FoldEndCode(line, styler);
          } 
        } else{
          StyleCodeEnd(line, styler);
          #ifdef DEBUG
          dbg << rnwmsg << __func__ << ":back in" << endl;
          #endif
        }
        i = end;
        style = TEX_DEFAULT;
        #ifdef DEBUG
        dbg << rnwmsg << "end End Code Line. moving to i=" << i << endl; 
        #endif
      }
      else {
        #ifdef DEBUG
        dbg << rnwmsg << "not an  end of code" << endl;
        #endif
        uint codebegin = end;
        uint codeend   = findEndRCode(codebegin, endDoc, styler);
        if(isNewCodeChunkLine(line, styler)){
          #ifdef DEBUG
          dbg << rnwmsg << "New code line." 
              << "  codebegin=" << codebegin
              << ", codeend="   << codeend
              << endl;
          #endif
          if(fold){
            styler.SetLevel(line, SC_FOLDLEVELBASE << 16);  // bitshift couteracts R bitshift.
            if(codeend > codebegin){ // correct code
              R::FoldDoc(codebegin, codeend-codebegin, R_DEFAULT, R_words, R_styler);
              int linecodeend = styler.GetLine(codeend);
              for(int cline = styler.GetLine(codebegin); cline < linecodeend; ++cline){
                int ol = styler.LevelAt(cline);
                styler.SetLevel(cline, (ol | RNW_FOLD_R) & 0xFFFF);
                #ifdef DEBUG
                dbg << rnwmsg << __func__ << "New:Correct R:" << hex
                    << "line=" << cline
                    << "level:" << ol << "->" << styler.LevelAt(cline) 
                    << dec << endl;
                #endif
              }
            }
            if(foldCode){
              FoldHeadCode(line, styler, foldAtNew);
            } else {
              FoldCarryover(line-1, styler);
            }
          }
          else { // style
            StyleCodeHeader(line, styler);
            styler.Flush();
            #ifdef DEBUG
            dbg << rnwmsg << __func__ << ":R Code(" 
                << codebegin
                << "--" << codeend
                << ")"  << endl;
            #endif
            if(codeend > codebegin){
              #ifdef DEBUG
              dbg << rnwmsg << __func__ << ":Doing R Code" << endl;
              dbg << rnwmsg << __func__ << ":R code Style:";
              for(uint ixx=codebegin; ixx < codeend; ixx++){
                dbg << setw(2) <<  static_cast<unsigned int>(styler.StyleAt(ixx));
              }
              dbg << endl;
              #endif
              R::ColouriseDoc(codebegin, codeend-codebegin, R_DEFAULT, R_words, TeX_styler);
              R_styler.Flush();
              #ifdef DEBUG
              dbg << rnwmsg << __func__ << ":R code Style:";
              for(uint ixx=codebegin; ixx < codeend; ixx++){
                dbg << setw(2) <<  static_cast<unsigned int>(styler.StyleAt(ixx));
              }
              dbg << endl;
              #endif
            }
          }
          style=RNW_DEFAULT;
          i=codeend;
          #ifdef DEBUG
          dbg << rnwmsg << "end New Code Line" << endl; 
          #endif
        } 
        else if(isCodeReuseLine(line, styler)){
          if(fold) {
            FoldCarryover(line-1, styler);
            FoldCarryover(line, styler);
          } 
          else {  // style
            StyleCodeReuse(line, styler);
          }
          i = end;
          style=R_DEFAULT;
        } 
        else { // unknown type
          #ifdef DEBUG
          dbg << rnwmsg << "unknown chunk line type" << endl;
          #endif
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
      int line = styler.GetLine(i);
      unsigned int codeend = findEndRCode(i, endDoc, styler);
      unsigned int prevstart = styler.LineStart(line-1);
      int prevstyle = styler.StyleAt(prevstart);
      if(isInCodeBlockState(prevstyle)){ /// do not stlye on first line of R code blocks
        Style(prevstart, codeend-prevstart+1, RNW_DEFAULT, pAccess, fold);
      } 
      else {
        if(fold){
          int prevlevel = styler.LevelAt(line-1);
          styler.SetLevel(line-1, prevlevel << 16);  // bitshift couteracts R bitshift.
          R::FoldDoc(i, codeend-i+1, R_DEFAULT, R_words, R_styler);
          styler.SetLevel(line-1, prevlevel);
        } 
        else {  // style
          R::ColouriseDoc(i, codeend-i, R_DEFAULT, R_words, R_styler);
        }
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
            << "line="   << setw(3) << line
            << " level=" << setw(4) << hex << styler.LevelAt(line) << dec
            << endl;
      } 
      else { //color
        // dbg << rnwmsg << "Class:"
            // << "line=" << setw(3) << line
            // << " class=" << classifyLine(line, styler, false)
            // << endl;
        dbg << rnwmsg << "LineState:"
            << "line=" << setw(3) << line
            << " state=" << styler.GetLineState(line)
            << endl;
      }
    }
  #endif
}
}}} // end namespace RnwLang::Lexers::Rnw
