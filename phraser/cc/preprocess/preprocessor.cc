#include "preprocessor.h"

bool Preprocessor::Init(string* error) {
    if (!html_.Init(error)) {
        return false;
    }

    if (!destutter_.Init(error)) {
        return false;
    }

    ready_ = true;
    return true;
}

namespace {

enum StepID {
    SI_HTML,
    SI_DESTUTTER,
};

// Transitively combine two index mappings into the second one.  Assumes the
// mappings are correct.
//
// That is, (input -> mid) + (mid -> output) = (input -> output).
void CombineMappings(const vector<uint16_t>& mid2in,
                     vector<uint16_t>* out2mid) {
    for (auto i = 0u; i < out2mid->size(); ++i) {
        auto& out_index = i;
        auto& mid_index = (*out2mid)[out_index];
        auto& in_index = mid2in[mid_index];
        (*out2mid)[out_index] = in_index;
    }
}

}  // namespace

bool Preprocessor::Preprocess(
        const vector<UChar>& in, bool replace_html_entities,
        size_t destutter_max_consecutive, vector<UChar>* out,
        vector<uint16_t>* out2in, unordered_map<UChar, size_t>* chr2drop,
        string* error) const {
    out->clear();
    out2in->clear();
    chr2drop->clear();

    if (!ready_) {
        *error = "[Preprocessor] You forgot to call Init().";
        return false;
    }

    // Figure out the steps to do and in what order, based on the params.
    vector<int> steps_to_do;
    if (replace_html_entities) {
        steps_to_do = {SI_HTML, SI_DESTUTTER};
    } else {
        steps_to_do = {SI_DESTUTTER};
    }

    // Scratch space for intermediate steps.
    vector<UChar> input_for_odd_steps;
    vector<UChar> input_for_even_steps;
    vector<uint16_t> out2in_for_odd_steps;
    vector<uint16_t> out2in_for_even_steps;

    // Pointers to the inputs/outputs of a single step (these are configured for
    // each step).  This is done to minimize copying.
    const vector<UChar>* cur_input;
    vector<UChar>* cur_output;
    vector<uint16_t>* cur_out2in;
    vector<uint16_t>* prev_out2in;

    for (auto i = 0u; i < steps_to_do.size(); ++i) {
        // Decide which data to use for this step.
        //
        // [input -> odd -> even -> odd -> even -> ... -> output].
        if (steps_to_do.size() == 1) {  // Only step.
            cur_input = &in;
            cur_output = out;
            cur_out2in = out2in;
            prev_out2in = NULL;
        } else if (i == 0) {  // First step of multiple.
            cur_input = &in;
            cur_output = &input_for_odd_steps;
            cur_out2in = &out2in_for_even_steps;
            prev_out2in = NULL;
        } else if (i == steps_to_do.size() - 1) {  // Last step of multiple.
            if (i % 2) {
                cur_input = &input_for_odd_steps;
                prev_out2in = &out2in_for_even_steps;
            } else {
                cur_input = &input_for_even_steps;
                prev_out2in = &out2in_for_odd_steps;
            }
            cur_output = out;
            cur_out2in = out2in;
        } else {  // Intermediate step.
            if (i % 2) {
                cur_input = &input_for_odd_steps;
                cur_output = &input_for_even_steps;
                cur_out2in = &out2in_for_odd_steps;
                prev_out2in = &out2in_for_even_steps;
            } else {
                cur_input = &input_for_even_steps;
                cur_output = &input_for_odd_steps;
                cur_out2in = &out2in_for_even_steps;
                prev_out2in = &out2in_for_odd_steps;
            }
        }

        // Actually perform the preprocessing step.
        auto& this_step = steps_to_do[i];
        switch (this_step) {
        case SI_HTML:
            html_.Replace(*cur_input, cur_output, cur_out2in);
            break;
        case SI_DESTUTTER:
            if (!destutter_.Destutter(
                    *cur_input, destutter_max_consecutive, cur_output,
                    cur_out2in, chr2drop, error)) {
                return false;
            }
            break;
        }

        // If we have previously performed a processing step, combine its
        // mapping and our mapping, like the intermediate step never happened.
        if (prev_out2in) {
            CombineMappings(*prev_out2in, cur_out2in);
        }
    }

    return true;
}
