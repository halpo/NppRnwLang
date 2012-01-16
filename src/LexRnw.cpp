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
namespace {
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

bool inline isInTexState(const char state){
  return TEX_DEFAULT <= state && state < TEX_END;
}
bool inline isInRState(const char state){
  return R_DEFAULT <= state && state < R_END;
}
bool inline isInCodeBlockState(const char state){
  return RNW_DEFAULT <= state && state < RNW_END;
}

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
    uint line          /// number of line to check
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
    return styler.LineStart(eline+1)-1; 
    /// @return returns the last position in the line with '@' if found.
  }
  return max; /// @return max if end of block not found.
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
    return styler.LineStart(line+1)-1; 
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
  return styler.LineStart(lineend)-1;
}

// Style Functions //////////////////////
void StyleCodeInHead(
    uint j
  , uint end
  , char bgstyle
  , LexAccessor &styler
  , WordList *keys=NULL ///  word list
  , bool usekeys=false  ///  Should any word= be a keyword?  
){
  StyleContext sc(j, end, bgstyle, styler);
  for(;sc.More(); sc.Forward()){ // scan through declarations
    if(sc.state == RNW_OPPERATOR)
      sc.SetState(bgstyle);
    if(sc.state == bgstyle){
      if(IsAWordStart(sc.ch)){
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
    if (sc.state == RNW_KEYWORD){
      // continue if still keyword
      if(!(
        IsAWordChar(sc.ch) ||
        isWhite(sc.ch)
      )) sc.SetState(bgstyle);
    }
    if (sc.ch=='='){
      sc.SetState(RNW_OPPERATOR);
      sc.Forward();
    }
  }
  sc.Complete();
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
  uint j   = styler.LineStart(line);
  uint max = styler.LineStart(line+1)-1;
  styler.StartSegment(j);
  styler.ColourTo(j+2, RNW_DEFAULT);
  int endhead = ScanTo(j, max, ">>=", styler);

  StyleCodeInHead(j+2, endhead-1, RNW_DEFAULT, styler, keys, usekeys);

  styler.StartSegment(endhead);
  styler.ColourTo(endhead+3, RNW_DEFAULT);
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
  uint j   = styler.LineStart(line);
  uint max = styler.LineStart(line+1)-1;
  if(styler.SafeGetCharAt(j)=='@')
    styler.ColourTo(j, RNW_OPPERATOR);
  for(j++; j<max && isWhite(styler.SafeGetCharAt(j)); j++) NULL;
  if(j<max){
    styler.ColourTo(max, RNW_COMMENT);
  }
}
void StyleCodeReuse(
    int line            ///  line to style
  , LexAccessor &styler    ///  Style Accessor
  , WordList *keys=NULL ///  word list
  , bool usekeys=false  ///  Should any word= be a keyword?  
){
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
LexerRnw Rnw;

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
		// Style(startPos, length, initStyle, pAccess, false);
	} catch (...) {
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
void Style(unsigned int startPos, int length, int initStyle, IDocument* pAccess, bool fold){
  #ifdef DEBUG
  dbg << rnwmsg << "in " << thisfunc
      << "(" << startPos << ", " << length
      << deparseStyle(initStyle) << ")"
      << endl;
  #endif // DEBUG

  PropSetSimple R_props, TeX_props, Rnw_props;
  
  LexAccessor styler(pAccess);
  Accessor TeX_styler(pAccess, &TeX_props);
  Accessor   R_styler(pAccess,   &R_props);
  WordList * keywordlists[8];
  WordList * R_words[] = {keywordlists[3],keywordlists[4],keywordlists[5], NULL, NULL, NULL, NULL, NULL};
  
  unsigned int lengthDoc = startPos+length ;
  for ( unsigned int i = startPos; i < lengthDoc; ) {
    #ifdef DEBUG
    dbg << rnwmsg << " Style: for loop: i=" << i << " /" << lengthDoc << endl;
    #endif
    char style = styler.StyleAt(i);
    uint nextchange = length;
    if ( i >= lengthDoc ) break ;
    if ( isInTexState(style) ) {
      nextchange = findNextCodeBlock(i, lengthDoc, styler);
      #ifdef DEBUG
      dbg << rnwmsg << "TeX state, lengthDoc=" << lengthDoc
                    << ", nextchange="         << nextchange << endl;
      #endif
      TeX::ColouriseDoc(startPos, nextchange-1, style, keywordlists, TeX_styler);
      i=nextchange;
      style=RNW_DEFAULT;
    } else
    if ( isInCodeBlockState(style) ) {
      int line = styler.GetLine(i);
      if(isNewCodeChunkLine(line, styler)){
        StyleCodeHeader(line, styler);
        line++;
        i = styler.LineStart(line);
        uint codeend = findEndRCode(i, lengthDoc, styler);
        R::ColouriseDoc(i, codeend-i, 0, R_words, R_styler);
        CorrectRStyle(i, codeend, R_DEFAULT, styler);
        style=RNW_DEFAULT;
        i=styler.LineStart(styler.GetLine(codeend)+1);
      } else if(isCodeReuseLine(line, styler)){
        StyleCodeReuse(line, styler);
        i = styler.LineStart(line+1);
      } else if(isEndCodeLine(line, styler)){
        StyleCodeEnd(line, styler);
        i = styler.LineStart(line+1);
        style = TEX_DEFAULT;
      }
      #ifdef DEBUG
      dbg << rnwmsg << "Code state, lengthDoc=" << lengthDoc
                    << ", nextchange="          << nextchange << endl;
      #endif
    } else
    if (isInRState(style)){
      uint codeend = findEndRCode(i, lengthDoc, styler);
      R::ColouriseDoc(i, codeend-i, 0, R_words, R_styler);
      CorrectRStyle(i, codeend, R_DEFAULT, styler);
      style=RNW_DEFAULT;
      i=styler.LineStart(styler.GetLine(codeend)+1);
    }
  }
  styler.Flush();
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
    if ( isInTexState(style) ) {
      nextchange = findNextCodeBlock(i, lengthDoc, styler);
      #ifdef DEBUG
      dbg << rnwmsg << "TeX state, lengthDoc=" << lengthDoc
                    << ", nextchange="         << nextchange << endl;
      #endif
      TeX::ColouriseDoc(startPos, nextchange-1, style, keywordlists, styler);
      i=nextchange;
      style=RNW_DEFAULT;
    } else
    if ( isInCodeBlockState(style) ) {
      int line = styler.GetLine(i);
      int maxline = styler.LineStart(lengthDoc);
      if(isNewCodeChunkLine(line, styler)){
        StyleCodeHeader(line, styler);
        line++;
        i = styler.LineStart(line);
        int lineend   = findEndBlockLine(line, maxline, styler);
        if(lineend==-1)
          lineend = maxline;
        int nexthead  = findNextCodeBlockLine(line, lineend, styler);
        if(nexthead!=-1)
          lineend = nexthead;
        int nextreuse = findNextCodeReuseLine(line, lineend, styler);
        if(nextreuse!=-1)
          lineend = nextreuse;
        uint codeend = styler.LineStart(lineend)-1;
        WordList * R_words[] = {keywordlists[3],keywordlists[4],keywordlists[5], NULL, NULL, NULL, NULL, NULL};
        R::ColouriseDoc(i, codeend-i, R_DEFAULT, R_words, styler);
        
      } else if(isCodeReuseLine(line, styler)){
        StyleCodeReuse(line, styler);
      } else if(isEndCodeLine(line, styler)){
        StyleCodeEnd(line, styler);
        i = styler.LineStart(line+1);
        style = TEX_DEFAULT;
      }
      #ifdef DEBUG
      dbg << rnwmsg << "Code state, lengthDoc=" << lengthDoc
                    << ", nextchange="          << nextchange << endl;
      #endif
    }
  }
}

void FoldDoc(
    unsigned int startPos       /// Starting Position, assumed to be start of a line
  , int length                  /// length to be analyzed
  , int initStyle               /// initial style
  , WordList *keywordlists[]    /// Word Lists
  , Accessor &styler            /// Accessor object
){
  #ifdef DEBUG
  dbg << rnwmsg << "in " << thisfunc
      << "(" << startPos
      << ", " << length
      << ", " << deparseStyle(initStyle)
      << ")" << endl;
  #endif //DEBUG
  unsigned int lengthDoc = startPos+length ;
  for ( unsigned int i = startPos; i < lengthDoc; ) {
    char style = styler.StyleAt(i);
    uint nextchange = length;
    if ( i >= lengthDoc ) break ;
    if ( isInTexState(style) ) {
      nextchange = findNextCodeBlock(i, lengthDoc, styler);
      #ifdef DEBUG
      dbg << rnwmsg << "TeX state, lengthDoc=" << lengthDoc
                    << ", nextchange="         << nextchange << endl;
      #endif
      TeX::FoldDoc(startPos, nextchange, style, keywordlists, styler);
      i=nextchange;
      //styler.StartStyling(
    } else
    if ( isInRState(style) ) {
      #ifdef DEBUG
      dbg << rnwmsg << "R state, lengthDoc=" << lengthDoc
                    << ", nextchange="       << nextchange << endl;
      #endif
      R::FoldDoc(startPos, nextchange, style-R_DEFAULT, keywordlists, styler);
    } else
    if ( isInCodeBlockState(style) ) {
      #ifdef DEBUG
      dbg << rnwmsg << "Code state, lengthDoc=" << lengthDoc
                    << ", nextchange="          << nextchange << endl;
      #endif
    }
  }

}

}}} // end namespace RnwLang::Lexers::Rnw

