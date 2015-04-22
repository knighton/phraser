#ifndef CC_BASE_MISC_COMBINATORICS_IMPL_H_
#define CC_BASE_MISC_COMBINATORICS_IMPL_H_

#include "combinatorics.h"

namespace combinatorics {

template <typename T>
bool NextChooseOneFromEach(
        const vector<T>& num_options_per_field, vector<T>* values) {
    // If it's empty, we're done.
    if (!num_options_per_field.size()) {
        return false;
    }

    // If any of the fields have no options, we're done.
    for (size_t i = 0; i < num_options_per_field.size(); ++i) {
        if (!num_options_per_field[i]) {
            return false;
        }
    }

    // Initialize the values to return.
    if (values->size() != num_options_per_field.size()) {
        values->resize(num_options_per_field.size());
        return true;
    }

    // Do the increment.
    ++(*values)[0];

    // Carry.
    size_t i = 0;
    while ((*values)[i] == num_options_per_field[i]) {
        (*values)[i] = 0;
        ++i;
        if (i == values->size()) {
            return false;
        }
        ++(*values)[i];
    }

    // Return that we have results.
    return true;
}

template <typename T>
bool ExpandCatchalls(const vector<T>& fields_with_catchalls,
                     const vector<T>& num_options_per_field_if_catchall,
                     vector<vector<T>>* fields) {
    fields->clear();

    if (fields_with_catchalls.size() !=
            num_options_per_field_if_catchall.size()) {
        return false;
    }

    vector<T> num_options_per_field;
    num_options_per_field.resize(fields_with_catchalls.size());
    vector<T> add_per_field;
    add_per_field.resize(fields_with_catchalls.size());
    for (auto i = 0u; i < fields_with_catchalls.size(); ++i) {
        auto& value = fields_with_catchalls[i];
        if (value == num_options_per_field_if_catchall[i]) {
            num_options_per_field[i] = num_options_per_field_if_catchall[i];
            add_per_field[i] = 0;
        } else if (value < num_options_per_field_if_catchall[i]) {
            num_options_per_field[i] = 1;
            add_per_field[i] = value;
        } else {
            return false;
        }
    }

    vector<T> values;
    while (NextChooseOneFromEach(num_options_per_field, &values)) {
        vector<T> new_values = values;
        for (auto i = 0u; i < new_values.size(); ++i) {
            new_values[i] += add_per_field[i];
        }
        fields->emplace_back(new_values);
    }

    return true;
}

}  // namespace combinatorics

#endif  // CC_BASE_MISC_COMBINATORICS_IMPL_H_
