#ifndef CC_FRONTEND_HTML_HTML_STEP_H_
#define CC_FRONTEND_HTML_HTML_STEP_H_

#include "cc/frontend/base/unicode_rewrite_step.h"
#include "cc/frontend/html/html_entity_parser.h"

class HtmlStep : public UnicodeRewriteStep {
  public:
    bool Init(string* error);

    bool Rewrite(const ustring& in, const UnicodeRewriteParams& more_in,
                 ustring* out, vector<ChrIndex>* out2in,
                 UnicodeRewriteInfo* more_out, string* error) const;

  private:
    HtmlEntityParser html_;
};

#endif  // CC_FRONTEND_HTML_HTML_STEP_H_
