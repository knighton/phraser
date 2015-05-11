#ifndef CC_FRONTEND_BASE_ASCII_REWRITE_STEP_H_
#define CC_FRONTEND_BASE_ASCII_REWRITE_STEP_H_

#include <string>
#include <vector>

#include "cc/frontend/base/chr_index.h"

using std::string;
using std::vector;

struct AsciiRewriteParams {
};

struct AsciiRewriteInfo {
};

class AsciiRewriteStep {
  public:
    virtual ~AsciiRewriteStep() {}

    virtual bool Rewrite(
        const string& in, const AsciiRewriteParams& more_in,
        string* out, vector<ChrIndex>* out2in,
        AsciiRewriteInfo* more_out, string* error) const = 0;
};

#endif  // CC_FRONTEND_BASE_ASCII_REWRITE_STEP_H_
