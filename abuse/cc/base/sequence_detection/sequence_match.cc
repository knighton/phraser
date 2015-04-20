#include "sequence_match.h"

void SequenceMatch::Init(size_t begin, size_t end_excl,
                         const vector<size_t>& option_choices) {
    begin_ = begin;
    end_excl_ = end_excl;
    option_choices_ = option_choices;
}
