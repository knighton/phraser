#ifndef CC_FRONTEND_FRONTEND_H_
#define CC_FRONTEND_FRONTEND_H_

#include <vector>

#include "cc/frontend/ascii/ptb_ascii_normalizer.h"
#include "cc/frontend/base/unicode_rewrite_step.h"
#include "cc/frontend/frontend_options.h"
#include "cc/frontend/frontend_result.h"

using std::vector;

class Frontend {
  public:
    Frontend() : is_ok_(false) {}
    ~Frontend();

    bool Init(string* error);

    // Returns false on failure.
    bool Parse(const ustring& original_text, const FrontendOptions& options,
               FrontendResult* result, string* error) const;

  private:
    bool is_ok_;

    // Unicode text -> Unicode text (replace HTML, destutter).
    vector<UnicodeRewriteStep*> unicode_steps_no_html_;
    vector<UnicodeRewriteStep*> unicode_steps_with_html_;

    // Uncode text -> Penn Treebank ASCII text.
    PtbAsciiNormalizer u2a_;

#if 0
    // ASCII -> ASCII (insert spaces for tokenization).
    vector<AsciiRewriteStep*> ascii_steps_;

    // ASCII text -> tokens.
    WhitespaceTokenizer tok_;

    // Tokens -> tokens.
    vector<TokenRewriteStep*> token_steps_;

    // Tokens -> tagged tokens.
    Tagger tag_;

    // Tagged tokens -> list of (possible tokens, tag).
    Uncontractor uncontract_;
#endif
};

#endif  // CC_FRONTEND_FRONTEND_H_
