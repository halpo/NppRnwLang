// Scintilla source code edit control
/** @file StyleContext.cxx
 ** Lexer infrastructure.
 **/
// Copyright 1998-2004 by Neil Hodgson <neilh@scintilla.org>
// This file is in the public domain.

#ifndef STYLECONTEXT_H
#define STYLECONTEXT_H

#ifdef SCI_NAMESPACE
namespace Scintilla {
#endif

static inline int MakeLowerCase(int ch) {
	if (ch < 'A' || ch > 'Z')
		return ch;
	else
		return ch - 'A' + 'a';
}

// All languages handled so far can treat all characters >= 0x80 as one class
// which just continues the current token or starts an identifier if in default.
// DBCS treated specially as the second character can be < 0x80 and hence
// syntactically significant. UTF-8 avoids this as all trail bytes are >= 0x80
class StyleContext {
	LexAccessor &styler;
	unsigned int endPos;
	StyleContext &operator=(const StyleContext &);
	void GetNextChar(unsigned int pos);
public:
	unsigned int currentPos;
	bool atLineStart;
	bool atLineEnd;
	int state;
	int chPrev;
	int ch;
	int chNext;

	StyleContext(unsigned int startPos, unsigned int length,
                        int initStyle, LexAccessor &styler_, char chMask=31);
	void Complete();
	bool More() const;
	void Forward();
	void Forward(int nb);
	void ChangeState(int state_);
	void SetState(int state_);
	void ForwardSetState(int state_);
	int LengthCurrent();
	int GetRelative(int n);
	bool Match(char ch0) const;
	bool Match(char ch0, char ch1) const;
	bool Match(const char *s);
	bool MatchIgnoreCase(const char *s);
	// Non-inline
	void GetCurrent(char *s, unsigned int len);
	void GetCurrentLowered(char *s, unsigned int len);
};

#ifdef SCI_NAMESPACE
}
#endif

#endif
