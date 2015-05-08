#ifndef CC_FRONTEND_REWRITE_STEP_H_
#define CC_FRONTEND_REWRITE_STEP_H_

#include <string>
#include <vector>

using std::string;
using std::vector;

// Apply a list of steps that mutate the input vector, putting the final result
// in the output vector.
//
// Also creates a mapping of output index to input index.
//
// If any step failed, terminates by setting the error string and returning
// false.  Else, returns true.
//
// Pass any additional input or output needed by any of the steps in 'more_in'
// and 'more_out'.
template  <typename RewriteStep, typename ItemVector, typename Index,
           typename AdditionalInput, typename AdditionalOutput>
bool ApplyRewriteSteps(
    const vector<RewriteStep>& steps, const ItemVector& in,
    const AdditionalInput& more_in, ItemVector* out, vector<Index>* out2in,
    AdditionalOutput* more_out, string* error);

#include "rewrite_step_impl.h"

#endif  // CC_FRONTEND_REWRITE_STEP_H_
