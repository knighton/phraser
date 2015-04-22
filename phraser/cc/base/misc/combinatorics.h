#ifndef CC_BASE_MISC_COMBINATORICS_H_
#define CC_BASE_MISC_COMBINATORICS_H_

#include <vector>

using std::vector;

namespace combinatorics {

// Returns true if there are more iterations left.  Intended to be used in a
// while loop.
template <typename T>
bool NextChooseOneFromEach(
    const vector<T>& num_options_per_field, vector<T>* values);

// Convert each NUM_(ENUM NAME)S -> every possible value for that enum.  Sets
// 'fields' to every possible combination of the fields.
//
// Returns false on invalid input.
template <typename T>
bool ExpandCatchalls(const vector<T>& fields_with_catchalls,
                     const vector<T>& num_options_per_field,
                     vector<vector<T>>* fields);

}  // namespace combinatorics

#include "combinatorics_impl.h"

#endif  // CC_BASE_MISC_COMBINATORICS
