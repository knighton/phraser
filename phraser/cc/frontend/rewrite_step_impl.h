#ifndef CC_FRONTEND_REWRITE_STEP_IMPL_H_
#define CC_FRONTEND_REWRITE_STEP_IMPL_H_

namespace {

template <typename Index>
void CombineMappings(const vector<Index>& mid2in, vector<Index>* out2mid) {
    for (auto i = 0u; i < out2mid->size(); ++i) {
        auto& out_index = i;
        auto& mid_index = (*out2mid)[out_index];
        auto& in_index = mid2in[mid_index];
        (*out2mid)[out_index] = in_index;
    }
}

}  // namespace

template <typename RewriteStep, typename ItemVector, typename Index,
          typename AdditionalInput, typename AdditionalOutput>
bool ApplyRewriteSteps(
        const vector<RewriteStep>& steps, const ItemVector& in,
        const AdditionalInput& more_in, ItemVector* out, vector<Index>* out2in,
        AdditionalOutput* more_out, string* error) {
    // Scratch space for intermediate steps.
    ItemVector input_for_odd_steps;
    ItemVector input_for_even_steps;
    vector<Index> out2in_for_odd_steps;
    vector<Index> out2in_for_even_steps;

    // Pointers to the inputs/outputs of a single step (these are configured for
    // each step).  This is done to minimize copying.
    const ItemVector* cur_input;
    ItemVector* cur_output;
    vector<Index>* cur_out2in;
    vector<Index>* prev_out2in;

    for (auto i = 0u; i < steps.size(); ++i) {
        // Decide which data to use for this step.
        //
        // [input -> odd -> even -> odd -> even -> ... -> output].
        if (steps.size() == 1) {  // Only step.
            cur_input = &in;
            cur_output = out;
            cur_out2in = out2in;
            prev_out2in = NULL;
        } else if (i == 0) {  // First step of multiple.
            cur_input = &in;
            cur_output = &input_for_odd_steps;
            cur_out2in = &out2in_for_even_steps;
            prev_out2in = NULL;
        } else if (i == steps.size() - 1) {  // Last step of multiple.
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

        auto& step = steps[i];
        if (!step.Apply(in, more_in, out, out2in, more_out, error)) {
            return false;
        }

        if (prev_out2in) {
            CombineMappings(*prev_out2in, cur_out2in);
        }
    }

    return true;
}

#endif  // CC_FRONTEND_REWRITE_STEP_IMPL_H_
