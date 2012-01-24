// Scintilla source code edit control
/** @file StyleContext.cxx
 ** Lexer infrastructure.
 **/
// Copyright 1998-2004 by Neil Hodgson <neilh@scintilla.org>
// This file is in the public domain.

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <assert.h>

#include "ILexer.h"

#include "LexAccessor.h"
#include "Accessor.h"
#include "StyleContext.h"

#ifdef SCI_NAMESPACE
using namespace Scintilla;
#endif



namespace {
void getRange(unsigned int start,
		unsigned int end,
		LexAccessor &styler,
		char *s,
		unsigned int len) {
	unsigned int i = 0;
	while ((i < end - start + 1) && (i < len-1)) {
		s[i] = styler[start + i];
		i++;
	}
	s[i] = '\0';
}
void getRangeLowered(unsigned int start,
		unsigned int end,
		LexAccessor &styler,
		char *s,
		unsigned int len) {
	unsigned int i = 0;
	while ((i < end - start + 1) && (i < len-1)) {
		s[i] = static_cast<char>(tolower(styler[start + i]));
		i++;
	}
	s[i] = '\0';
}
}
void StyleContext::GetNextChar(unsigned int pos) {
  chNext = static_cast<unsigned char>(styler.SafeGetCharAt(pos+1));
  if (styler.IsLeadByte(static_cast<char>(chNext))) {
    chNext = chNext << 8;
    chNext |= static_cast<unsigned char>(styler.SafeGetCharAt(pos+2));
  }
  // End of line?
  // Trigger on CR only (Mac style) or either on LF from CR+LF (Dos/Win)
  // or on LF alone (Unix). Avoid triggering two times on Dos/Win.
  atLineEnd = (ch == '\r' && chNext != '\n') ||
        (ch == '\n') ||
        (currentPos >= endPos);
}
StyleContext::StyleContext(unsigned int startPos, unsigned int length,
                      int initStyle, LexAccessor &styler_, char chMask/* =31 */) :
  styler(styler_),
  endPos(startPos + length),
  currentPos(startPos),
  atLineStart(true),
  atLineEnd(false),
  state(initStyle & chMask), // Mask off all bits which aren't in the chMask.
  chPrev(0),
  ch(0),
  chNext(0) {
  styler.StartAt(startPos, chMask);
  styler.StartSegment(startPos);
  unsigned int pos = currentPos;
  ch = static_cast<unsigned char>(styler.SafeGetCharAt(pos));
  if (styler.IsLeadByte(static_cast<char>(ch))) {
    pos++;
    ch = ch << 8;
    ch |= static_cast<unsigned char>(styler.SafeGetCharAt(pos));
  }
  GetNextChar(pos);
}
void StyleContext::Complete() {
  styler.ColourTo(currentPos - 1, state);
  styler.Flush();
}
bool StyleContext::More() const {
  return currentPos < endPos;
}
void StyleContext::Forward() {
  if (currentPos < endPos) {
    atLineStart = atLineEnd;
    chPrev = ch;
    currentPos++;
    if (ch >= 0x100)
      currentPos++;
    ch = chNext;
    GetNextChar(currentPos + ((ch >= 0x100) ? 1 : 0));
  } else {
    atLineStart = false;
    chPrev = ' ';
    ch = ' ';
    chNext = ' ';
    atLineEnd = true;
  }
}
void StyleContext::Forward(int nb) {
  for (int i = 0; i < nb; i++) {
    Forward();
  }
}
void StyleContext::ChangeState(int state_) {
  state = state_;
}
void StyleContext::SetState(int state_) {
  styler.ColourTo(currentPos - 1, state);
  state = state_;
}
void StyleContext::ForwardSetState(int state_) {
  Forward();
  styler.ColourTo(currentPos - 1, state);
  state = state_;
}
int StyleContext::LengthCurrent() {
  return currentPos - styler.GetStartSegment();
}
int StyleContext::GetRelative(int n) {
  return static_cast<unsigned char>(styler.SafeGetCharAt(currentPos+n));
}
bool StyleContext::Match(char ch0) const {
  return ch == static_cast<unsigned char>(ch0);
}
bool StyleContext::Match(char ch0, char ch1) const {
  return (ch == static_cast<unsigned char>(ch0)) && (chNext == static_cast<unsigned char>(ch1));
}
bool StyleContext::Match(const char *s) {
  if (ch != static_cast<unsigned char>(*s))
    return false;
  s++;
  if (!*s)
    return true;
  if (chNext != static_cast<unsigned char>(*s))
    return false;
  s++;
  for (int n=2; *s; n++) {
    if (*s != styler.SafeGetCharAt(currentPos+n))
      return false;
    s++;
  }
  return true;
}
bool StyleContext::MatchIgnoreCase(const char *s) {
  if (MakeLowerCase(ch) != static_cast<unsigned char>(*s))
    return false;
  s++;
  if (MakeLowerCase(chNext) != static_cast<unsigned char>(*s))
    return false;
  s++;
  for (int n=2; *s; n++) {
    if (static_cast<unsigned char>(*s) !=
      MakeLowerCase(static_cast<unsigned char>(styler.SafeGetCharAt(currentPos+n))))
      return false;
    s++;
  }
  return true;
}

void StyleContext::GetCurrent(char *s, unsigned int len) {
	getRange(styler.GetStartSegment(), currentPos - 1, styler, s, len);
}

void StyleContext::GetCurrentLowered(char *s, unsigned int len) {
	getRangeLowered(styler.GetStartSegment(), currentPos - 1, styler, s, len);
}



