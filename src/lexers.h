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
    /// R 30+
    , R_DEFAULT       = 20
    , R_COMMENT       = 21
    , R_KWORD         = 22
    , R_BASEKWORD     = 23
    , R_OTHERKWORD    = 24
    , R_NUMBER        = 25
    , R_STRING        = 26
    , R_STRING2       = 27
    , R_OPERATOR      = 28
    , R_IDENTIFIER    = 29
    , R_INFIX         = 30
    , R_INFIXEOL      = 31
    , R_END           = 32
  };
  enum {
    RNW_FOLD_R = 0x0080
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
 