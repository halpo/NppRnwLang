/*! @file lexers.h
 *  definitions for Lexers
 *
 *  Here I break the convention of Fold<<Name>>Doc() in favor of c++ namespaces where
 *  we use <<Name>>::FoldDoc()  This way new languages can be defined by preprocessor macro.
 */
#ifndef RNWLANG_SRC_LEXERS_H_
#define RNWLANG_SRC_LEXERS_H_
#include "Accessor.h"

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
    , TEX_SPECIAL     = 1
    , TEX_GROUP       = 2
    , TEX_SYMBOL      = 3
    , TEX_COMMAND     = 4
    , TEX_TEXT        = 5
    , TEX_RNW_SPECIAL = 6
    , TEX_END         = 9
    /// Rnw / Sweave only 10-11
    , RNW_DEFAULT     = 10
    , RNW_REUSE       = 11
    , RNW_KEYWORD     = 12
    , RNW_OPERATOR    = 13
    , RNW_COMMENT     = 14
    , RNW_NAME        = 15
    , RNW_END         = 19
    /// Word lists 20-29
    , WORD0           = 20
    , WORD1           = 21
    , WORD2           = 22
    , WORD3           = 23
    , WORD4           = 24
    , WORD5           = 25
    , WORD6           = 26
    , WORD7           = 27
    , WORD8           = 28
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
}} // end RnwLang::Lexers
#if 1 // redefine SCE_R values
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
#endif // redefine SCE_R
#if 1 // redefine SCE_TEX
#undef  SCE_TEX_DEFAULT      
#undef  SCE_TEX_SPECIAL      
#undef  SCE_TEX_GROUP        
#undef  SCE_TEX_SYMBOL       
#undef  SCE_TEX_COMMAND      
#undef  SCE_TEX_TEXT         
#define SCE_TEX_DEFAULT      TEX_DEFAULT     
#define SCE_TEX_SPECIAL      TEX_SPECIAL     
#define SCE_TEX_GROUP        TEX_GROUP       
#define SCE_TEX_SYMBOL       TEX_SYMBOL      
#define SCE_TEX_COMMAND      TEX_COMMAND     
#define SCE_TEX_TEXT         TEX_TEXT        
#define SCE_TEX_END          TEX_END         
#endif // redefine SCE_TEX 
#endif //RNWLANG_SRC_LEXERS_H_

