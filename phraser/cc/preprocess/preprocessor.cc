#include "preprocessor.h"

bool Preprocessor::Init(string* error) {
    if (!destutter_.Init(error)) {
        return false;
    }

    ready_ = true;
    return true;
}

bool Preprocessor::Preprocess(
        const vector<UChar>& dirty, size_t destutter_max_consecutive,
        vector<UChar>* clean, vector<uint16_t>* clean2dirty_or_null,
        unordered_map<UChar, size_t>* chr2drop_or_null, string* error) const {
    if (!ready_) {
        *error = "[Preprocessor] You forgot to call Init().";
        return false;
    }

    unordered_map<UChar, size_t> chr2drop;
    if (!destutter_.Destutter(
            dirty, destutter_max_consecutive, clean, clean2dirty_or_null,
            chr2drop_or_null, error)) {
        return false;
    }

    return true;
}
