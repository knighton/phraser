#ifndef CC_FRONTEND_FRONTEND_H_
#define CC_FRONTEND_FRONTEND_H_

#include "cc/frontend/unicode_rewrite_step.h"

class Frontend {
  public:
    Frontend() : is_ok_(false) {}
    ~Frontend();

    bool Init(string* error);

  private:
    bool is_ok_;

    // Unicode text -> Unicode text.
    vector<UnicodeRewriteStep*> unicode_steps_no_html_;
    vector<UnicodeRewriteStep*> unicode_steps_with_html_;

#if 0
    // Uncode text -> Penn Treebank ASCII text.
    AsciiConverter u2a_;

    // ASCII -> ASCII (inserts spaces for tokenization).
    vector<AsciiRewriteStep> ascii_steps_;

    // ASCII text -> tokens.
    WhitespaceTokenizer tok_;

    // Tokens -> tokens.
    vector<TokenRewriteStep> token_steps_;

    // Tokens -> tagged tokens.
    Tagger tag_;

    // Tagged tokens -> list of (possible tokens, tag).
    Uncontractor uncontract_;
#endif
};

#endif  // CC_FRONTEND_FRONTEND_H_
