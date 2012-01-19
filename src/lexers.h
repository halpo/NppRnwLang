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
    , RNW_OPERATOR
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
    , R_COMMENT       = 31
    , R_KWORD         = 32
    , R_BASEKWORD     = 33
    , R_OTHERKWORD    = 34
    , R_NUMBER        = 35
    , R_STRING        = 36
    , R_STRING2       = 37
    , R_OPERATOR      = 38
    , R_IDENTIFIER    = 39
    , R_INFIX         = 40
    , R_INFIXEOL      = 41
    , R_END           = 42
  };
//undefine value for R Lexer
#undef SCE_R_DEFAULT     
#undef SCE_R_COMMENT     
#undef SCE_R_KWORD       
#undef SCE_R_BASEKWORD   
#undef SCE_R_OTHERKWORD  
#undef SCE_R_NUMBER      
#undef SCE_R_STRING      
#undef SCE_R_STRING2     
#undef SCE_R_OPERATOR    
#undef SCE_R_IDENTIFIER  
#undef SCE_R_INFIX       
#undef SCE_R_INFIXEOL    
//redefine value for R Lexer
#define SCE_R_DEFAULT     R_DEFAULT   
#define SCE_R_COMMENT     R_COMMENT   
#define SCE_R_KWORD       R_KWORD     
#define SCE_R_BASEKWORD   R_BASEKWORD 
#define SCE_R_OTHERKWORD  R_OTHERKWORD
#define SCE_R_NUMBER      R_NUMBER    
#define SCE_R_STRING      R_STRING    
#define SCE_R_STRING2     R_STRING2   
#define SCE_R_OPERATOR    R_OPERATOR  
#define SCE_R_IDENTIFIER  R_IDENTIFIER
#define SCE_R_INFIX       R_INFIX     
#define SCE_R_INFIXEOL    R_INFIXEOL  
}} // end RnwLang::Lexers
 