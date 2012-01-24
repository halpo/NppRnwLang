// Scintilla source code edit control
/** @file CharacterSet.cxx
 ** Simple case functions for ASCII.
 ** Lexer infrastructure.
 **/
// Copyright 1998-2010 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <assert.h>

#include "CharacterSet.h"

#ifdef SCI_NAMESPACE
using namespace Scintilla;
#endif

#ifdef SCI_NAMESPACE
namespace Scintilla {
#endif
CharacterSet::CharacterSet(setBase base/* =setNone */, const char *initialSet/* ="" */, int size_/* =0x80 */, bool valueAfter_/* =false */) {
  size = size_;
  valueAfter = valueAfter_;
  bset = new bool[size];
  for (int i=0; i < size; i++) {
    bset[i] = false;
  }
  AddString(initialSet);
  if (base & setLower)
    AddString("abcdefghijklmnopqrstuvwxyz");
  if (base & setUpper)
    AddString("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
  if (base & setDigits)
    AddString("0123456789");
}
CharacterSet::~CharacterSet() {
  delete []bset;
  bset = 0;
  size = 0;
}
void CharacterSet::Add(int val) {
  assert(val >= 0);
  assert(val < size);
  bset[val] = true;
}
void CharacterSet::AddString(const char *setToAdd) {
  for (const char *cp=setToAdd; *cp; cp++) {
    int val = static_cast<unsigned char>(*cp);
    assert(val >= 0);
    assert(val < size);
    bset[val] = true;
  }
}
bool CharacterSet::Contains(int val) const {
  assert(val >= 0);
  if (val < 0) return false;
  return (val < size) ? bset[val] : valueAfter;
}

int CompareCaseInsensitive(const char *a, const char *b) {
	while (*a && *b) {
		if (*a != *b) {
			char upperA = MakeUpperCase(*a);
			char upperB = MakeUpperCase(*b);
			if (upperA != upperB)
				return upperA - upperB;
		}
		a++;
		b++;
	}
	// Either *a or *b is nul
	return *a - *b;
}

int CompareNCaseInsensitive(const char *a, const char *b, size_t len) {
	while (*a && *b && len) {
		if (*a != *b) {
			char upperA = MakeUpperCase(*a);
			char upperB = MakeUpperCase(*b);
			if (upperA != upperB)
				return upperA - upperB;
		}
		a++;
		b++;
		len--;
	}
	if (len == 0)
		return 0;
	else
		// Either *a or *b is nul
		return *a - *b;
}

#ifdef SCI_NAMESPACE
}
#endif
