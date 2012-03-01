// Scintilla source code edit control
/** @file LexAccessor.h
 ** Interfaces between Scintilla and lexers.
 **/
// Copyright 1998-2010 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#include "LexAccessor.h"
#include <cassert>
#ifdef SCI_NAMESPACE
namespace Scintilla {
#endif

void LexAccessor::Fill(int position) {
  startPos = position - slopSize;
  if (startPos + bufferSize > lenDoc)
    startPos = lenDoc - bufferSize;
  if (startPos < 0)
    startPos = 0;
  endPos = startPos + bufferSize;
  if (endPos > lenDoc)
    endPos = lenDoc;

  pAccess->GetCharRange(buf, startPos, endPos-startPos);
  buf[endPos-startPos] = '\0';
}
LexAccessor::LexAccessor(IDocument *pAccess_) :
  pAccess(pAccess_), startPos(extremePosition), endPos(0),
  codePage(pAccess->CodePage()), lenDoc(pAccess->Length()),
  mask(127), validLen(0), chFlags(0), chWhile(0),
  startSeg(0), startPosStyling(0) {
}
char LexAccessor::operator[](int position) {
  if (position < startPos || position >= endPos) {
    Fill(position);
  }
  return buf[position - startPos];
}
/** Safe version of operator[], returning a defined value for invalid position. */
char LexAccessor::SafeGetCharAt(int position, char chDefault) {
  if (position < startPos || position >= endPos) {
    Fill(position);
    if (position < startPos || position >= endPos) {
      // Position is outside range of document
      return chDefault;
    }
  }
  return buf[position - startPos];
}
bool LexAccessor::IsLeadByte(char ch) {
  return pAccess->IsDBCSLeadByte(ch);
}

bool LexAccessor::Match(int pos, const char *s) {
  for (int i=0; *s; i++) {
    if (*s != SafeGetCharAt(pos+i))
      return false;
    s++;
  }
  return true;
}
char LexAccessor::StyleAt(int position) {
  return static_cast<char>(pAccess->StyleAt(position) & mask);
}
int LexAccessor::GetLine(int position) {
  return pAccess->LineFromPosition(position);
}
int LexAccessor::LineStart(int line) {
  return pAccess->LineStart(line);
}
int LexAccessor::LevelAt(int line) {
  return pAccess->GetLevel(line);
}
int LexAccessor::Length() const {
  return lenDoc;
}
void LexAccessor::Flush() {
  startPos = extremePosition;
  if (validLen > 0) {
    pAccess->SetStyles(validLen, styleBuf);
    startPosStyling += validLen;
    validLen = 0;
  }
}
int LexAccessor::GetLineState(int line) {
  return pAccess->GetLineState(line);
}
int LexAccessor::SetLineState(int line, int state) {
  return pAccess->SetLineState(line, state);
}
// Style setting
void LexAccessor::StartAt(unsigned int start, char chMask) {
  // Store the mask specified for use with StyleAt.
  mask = chMask;
  pAccess->StartStyling(start, chMask);
  startPosStyling = start;
}
void LexAccessor::SetFlags(char chFlags_, char chWhile_) {
  chFlags = chFlags_;
  chWhile = chWhile_;
}
unsigned int LexAccessor::GetStartSegment() const {
  return startSeg;
}
void LexAccessor::StartSegment(unsigned int pos) {
  startSeg = pos;
}
void LexAccessor::ColourTo(unsigned int pos, int chAttr) {
  // Only perform styling if non empty range
  if (pos != startSeg - 1) {
    assert(pos >= startSeg);
    if (pos < startSeg) {
      return;
    }

    if (validLen + (pos - startSeg + 1) >= bufferSize)
      Flush();
    if (validLen + (pos - startSeg + 1) >= bufferSize) {
      // Too big for buffer so send directly
      pAccess->SetStyleFor(pos - startSeg + 1, static_cast<char>(chAttr));
    } else {
      if (chAttr != chWhile)
        chFlags = 0;
      chAttr |= chFlags;
      for (unsigned int i = startSeg; i <= pos; i++) {
        assert((startPosStyling + validLen) < Length());
        styleBuf[validLen++] = static_cast<char>(chAttr);
      }
    }
  }
  startSeg = pos+1;
}
void LexAccessor::SetLevel(int line, int level) {
  pAccess->SetLevel(line, level);
}
void LexAccessor::IndicatorFill(int start, int end, int indicator, int value) {
  pAccess->DecorationSetCurrentIndicator(indicator);
  pAccess->DecorationFillRange(start, value, end - start);
}
void LexAccessor::ChangeLexerState(int start, int end) {
  pAccess->ChangeLexerState(start, end);
}

#ifdef SCI_NAMESPACE
}
#endif

