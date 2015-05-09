#ifndef CC_FRONTEND_BASE_REWRITE_IN_PLACE_STEP_IMPL_H_
#define CC_FRONTEND_BASE_REWRITE_IN_PLACE_STEP_IMPL_H_

template  <typename RewriteInPlaceStep, typename ItemVector>
bool ApplyRewriteInPlaceSteps(
        const vector<RewriteInPlaceStep*>& steps, ItemVector* inout,
        string* error) {
    for (auto& step : steps) {
        if (!step->RewriteInPlace(inout, error)) {
            return false;
        }
    }

    return true;
}

#endif  // CC_FRONTEND_BASE_REWRITE_IN_PLACE_STEP_IMPL_H_
