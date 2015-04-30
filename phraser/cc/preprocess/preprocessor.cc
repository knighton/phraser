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
        vector<UChar>* clean, string* error) const {
    if (!ready_) {
        *error = "[Preprocessor] You forgot to call Init().";
        return false;
    }

    unordered_map<UChar, size_t> chr2drop;
    if (!destutter_.Destutter(
            dirty, destutter_max_consecutive, clean, &chr2drop, error)) {
        return false;
    }

    return true;
}
