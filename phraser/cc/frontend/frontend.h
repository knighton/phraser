#ifndef CC_FRONTEND_FRONTEND_H_
#define CC_FRONTEND_FRONTEND_H_

#include <vector>

#include "cc/frontend/ascii/ptb_ascii_normalizer.h"
#include "cc/frontend/base/ascii_rewrite_step.h"
#include "cc/frontend/base/token_rewrite_in_place_step.h"
#include "cc/frontend/base/unicode_rewrite_step.h"
#include "cc/frontend/contractions/uncontractor.h"
#include "cc/frontend/frontend_options.h"
#include "cc/frontend/frontend_result.h"
#include "cc/frontend/reductions/unreductor.h"
#include "cc/frontend/tag/lapos_tagger.h"

using std::vector;

class Frontend {
  public:
    Frontend() : is_ok_(false) {}
    ~Frontend();

    bool Init(string* error);

    // Returns false on failure.
    bool Analyze(const ustring& original_text, const FrontendOptions& options,
                 FrontendResult* result, string* error) const;

  private:
    bool is_ok_;

    // Unicode text normalization.
    //
    // 1. HTMLEntityParser
    // 2. Destutterer
    vector<UnicodeRewriteStep*> unicode_steps_no_html_;
    vector<UnicodeRewriteStep*> unicode_steps_with_html_;

    // Unicode to Penn Treebank ASCII conversion.
    PtbAsciiNormalizer u2a_;

    // ASCII text rewriting.
    //
    // 1. PunctuationClassifier
    // 2. LaposSpacer
    // 3. PunctuationDecoder
    vector<AsciiRewriteStep*> ascii_steps_;

#if 0
    // PTB ASCII text to token conversion.
    WhitespaceTokenizer tok_;
#endif

    // In-place token normalization.
    //
    // 1. PTBEscaper
    // 2. Americanizer
    vector<TokenRewriteInPlaceStep*> token_steps_;

    // Un-perform reductions that are not contractions.
    //
    //     strings -> Tokens
    //
    // In cases of ambiguous results (multiple posssible words at a location
    // that have different tags, eg getcha -> get you, get your), provides the
    // correct tags as well.
    Unreductor unreduct_;

    // Tag untagged tokens.
    //
    //     mutates Tokens in place
    //
    // If a Token has multiple options (due to unreduction), tags all options
    // that do not already have tags according to the first option (tagging is
    // comparatively expensive) (unreduction provides override tags in cases of
    // ambiguity).
    LaposTagger tag_;

    // Undo contractions.
    //
    //     mutates Tokens in place
    //
    // Notes:
    // * Relies on contraction suffixes being their own tokens.
    // * Some results are ambiguous: 's and 'd.
    // * Relies on tag to disambiguate verb 's from possessive 's.
    Uncontractor uncontract_;
};

#endif  // CC_FRONTEND_FRONTEND_H_
