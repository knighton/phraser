#ifndef CC_FRONTEND_BASE_REWRITE_IN_PLACE_STEP_H_
#define CC_FRONTEND_BASE_REWRITE_IN_PLACE_STEP_H_

#include <string>
#include <vector>

using std::string;
using std::vector;

template  <typename RewriteInPlaceStep, typename ItemVector>
bool ApplyRewriteInPlaceSteps(
    const vector<RewriteInPlaceStep>& steps, ItemVector* inout, string* error);

#endif  // CC_FRONTEND_BASE_REWRITE_IN_PLACE_STEP_H_
