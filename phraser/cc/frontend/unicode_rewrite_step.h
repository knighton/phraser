#ifndef CC_FRONTEND_UNICODE_REWRITE_STEP_H_
#define CC_FRONTEND_UNICODE_REWRITE_STEP_H_

#include <string>
#include <unordered_map>
#include <vector>

#include "cc/base/unicode.h"
#include "cc/frontend/chr_index.h"

using std::string;
using std::unordered_map;
using std::vector;

struct UnicodeRewriteParams {
    size_t destutter_max_consecutive;
};

struct UnicodeRewriteInfo {
    unordered_map<uchar, size_t>* chr2drop;
};

class UnicodeRewriteStep {
  public:
    virtual ~UnicodeRewriteStep() {}

    virtual bool Rewrite(
        const ustring& in, const UnicodeRewriteParams& more_in,
        ustring* out, vector<ChrIndex>* out2in,
        UnicodeRewriteInfo* more_out, string* error) const = 0;
};

#endif  // CC_FRONTEND_UNICODE_REWRITE_STEP_H_
