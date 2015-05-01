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

bool Preprocessor::Preprocess(
        const vector<UChar>& original, bool replace_html_entities,
        size_t destutter_max_consecutive, vector<UChar>* clean,
        vector<uint16_t>* clean2original_or_null,
        unordered_map<UChar, size_t>* chr2drop_or_null, string* error) const {
    if (!ready_) {
        *error = "[Preprocessor] You forgot to call Init().";
        return false;
    }

    // Replace HTML entities.
    vector<UChar> text;
    const vector<UChar>* pointer;
    if (replace_html_entities) {
        html_.Replace(original, &text);
        pointer = &text;
    } else {
        pointer = &original;
    }

    // Destutter.
    unordered_map<UChar, size_t> chr2drop;
    if (!destutter_.Destutter(
            *pointer, destutter_max_consecutive, clean, clean2original_or_null,
            chr2drop_or_null, error)) {
        return false;
    }

    return true;
}
