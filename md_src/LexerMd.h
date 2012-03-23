

#ifndef __LEXERMD_H_
#define __LEXERMD_H_

#include "LexerPlugin.h"
#include "PluginDebug.h"

namespace Lexers{
    namespace Markdown {
      class LexerMarkdown : public LexerBase {
        void SCI_METHOD Lex(unsigned int startPos, int lengthDoc, int initStyle, IDocument *pAccess);
        void SCI_METHOD Fold(unsigned int startPos, int lengthDoc, int initStyle, IDocument *pAccess);
      };
      void ColorizeMarkdownDoc(unsigned int startPos, int length, int initStyle,
                               WordList **, Accessor &styler);
    }  // namespace Markdown
}   // namespace Lexers

#endif  // __LEXERMD_H_


