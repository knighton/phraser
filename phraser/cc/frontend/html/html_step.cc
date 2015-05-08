#include "html_step.h"

bool HtmlStep::Init(string* error) {
    return html_.Init(error);
}

bool HtmlStep::Rewrite(
        const ustring& in, const UnicodeRewriteParams& more_in, ustring* out,
        vector<ChrIndex>* out2in, UnicodeRewriteInfo* more_out,
        string* error) const {
    while (html_.Replace(in, out, out2in)) {
        ;
    }

    return true;
}
