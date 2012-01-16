/*! @file lexers.h
 *  definitions for Lexers
 *
 *  Here I break the convention of Fold<<Name>>Doc() in favor of c++ namespaces where
 *  we use <<Name>>::FoldDoc()  This way new languages can be defined by preprocessor macro.
 */


#ifdef ACCESSOR_H
#ifdef SCI_NAMESPACE
using Scintilla::Accessor
#endif
#define __COLOURIZEDOCDEF__  void ColouriseDoc(unsigned int startPos, int length, int initStyle, WordList *keywordlists[], Accessor &styler);
#define __FLDXDOCDEF__  void FoldDoc(unsigned int startPos, int length, int, WordList *[], Accessor &styler);
#define __DOC_DEFINITIONS__ __COLOURIZEDOCDEF__ __FLDXDOCDEF__
#define NEWLANG(name) namespace name{__DOC_DEFINITIONS__}
#else  // ACCESSOR_H
#define NEWLANG(name)
#endif // ACCESSSOR_H 
namespace RnwLang{ namespace Lexers{
  NEWLANG(R)
  NEWLANG(TeX)
  NEWLANG(Rnw)
    enum { /// Styles
    /// DEFAULT is tex
    /// TEX  range from 0-9
      TEX_DEFAULT     = 0
    , TEX_SPECIAL
    , TEX_GROUP
    , TEX_SYMBOL
    , TEX_COMMAND
    , TEX_TEXT
    , TEX_RNW_SPECIAL
    , TEX_END         =9
    /// Rnw / Sweave only 10-11
    , RNW_DEFAULT     = 10
    , RNW_REUSE
    , RNW_KEYWORD
    , RNW_OPPERATOR
    , RNW_COMMENT
    , RNW_END         = 19
    /// Word lists 20-29
    , WORD0           = 20
    , WORD1
    , WORD2
    , WORD3
    , WORD4
    , WORD5
    , WORD6
    , WORD7
    , WORD8
    , WORDS_END       = 29
    /// R 30+
    , R_DEFAULT       = 30
    , R_COMMENT
    , R_KWORD
    , R_BASEKWORD
    , R_OTHERKWORD
    , R_NUMBER
    , R_STRING
    , R_STRING2
    , R_OPERATOR
    , R_IDENTIFIER
    , R_INFIX
    , R_INFIXEOL
    , R_END
  };
}} // end RnwLang::Lexers
 