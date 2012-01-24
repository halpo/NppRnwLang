// Scintilla source code edit control
/** @file LexAccessor.h
 ** Interfaces between Scintilla and lexers.
 **/
// Copyright 1998-2010 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#ifndef LEXACCESSOR_H
#define LEXACCESSOR_H
#include "ILexer.h"

#ifdef SCI_NAMESPACE
namespace Scintilla {
#endif

class LexAccessor {
private:
	IDocument *pAccess;
	enum {extremePosition=0x7FFFFFFF};
	/** @a bufferSize is a trade off between time taken to copy the characters
	 * and retrieval overhead.
	 * @a slopSize positions the buffer before the desired position
	 * in case there is some backtracking. */
	enum {bufferSize=4000, slopSize=bufferSize/8};
	char buf[bufferSize+1];
	int startPos;
	int endPos;
	int codePage;
	int lenDoc;
	int mask;
	char styleBuf[bufferSize];
	int validLen;
	char chFlags;
	char chWhile;
	unsigned int startSeg;
	int startPosStyling;

	void Fill(int position);

public:
	LexAccessor(IDocument *pAccess_);
	char operator[](int position);
	/** Safe version of operator[], returning a defined value for invalid position. */
	char SafeGetCharAt(int position, char chDefault=' ');
	bool IsLeadByte(char ch);
	bool Match(int pos, const char *s);
	char StyleAt(int position);
	int GetLine(int position);
	int LineStart(int line);
	int LevelAt(int line);
	int Length() const;
	void Flush();
	int GetLineState(int line);
	int SetLineState(int line, int state);
	// Style setting
	void StartAt(unsigned int start, char chMask=31);
	void SetFlags(char chFlags_, char chWhile_);
	unsigned int GetStartSegment() const;
	void StartSegment(unsigned int pos);
	void ColourTo(unsigned int pos, int chAttr);
	void SetLevel(int line, int level);
	void IndicatorFill(int start, int end, int indicator, int value);
	void ChangeLexerState(int start, int end);
};

#ifdef SCI_NAMESPACE
}
#endif

#endif

