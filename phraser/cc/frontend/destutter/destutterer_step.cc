#include "destutterer_step.h"

bool DestuttererStep::Init(string* error) {
    return destutter_.Init(error);
}

bool DestuttererStep::Rewrite(
        const ustring& in, const UnicodeRewriteParams& more_in, ustring* out,
        vector<ChrIndex>* out2in, UnicodeRewriteInfo* more_out,
        string* error) const {
    return destutter_.Destutter(
        in, more_in.destutter_max_consecutive, out, out2in, more_out->chr2drop,
        error);
}
