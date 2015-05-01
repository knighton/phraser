#include "destutterer.h"

#include "cc/preprocess/destutterer_data.h"

static bool LoadDigits(unordered_set<UChar>* digits, string* error) {
    for (auto i = 0u; i < destutterer_data::DIGITS_SIZE; ++i) {
        auto digit = destutterer_data::DIGITS[i];
        auto it = digits->find(digit);
        if (it != digits->end()) {
            *error = "[Destutterer] List of digits contains duplicates.";
            return false;
        }
        digits->insert(digit);
    }
    return true;
}

bool Destutterer::Init(string* error) {
    if (!LoadDigits(&digits_, error)) {
        return false;
    }

    return true;
}

bool Destutterer::Destutter(
        const vector<UChar>& dirty, size_t max_consecutive,
        vector<UChar>* clean, vector<uint16_t>* clean2dirty_or_null,
        unordered_map<UChar, size_t>* chr2drop_or_null, string* error) const {
    clean->clear();
    if (clean2dirty_or_null) {
        clean2dirty_or_null->clear();
    }
    if (chr2drop_or_null) {
        chr2drop_or_null->clear();
    }

    if (chr2drop_or_null) {
        chr2drop_or_null->clear();
    }

    if (dirty.empty()) {
        return true;
    }

    UChar prev_c = dirty[0];
    clean->emplace_back(prev_c);
    if (clean2dirty_or_null) {
        clean2dirty_or_null->emplace_back(0);
    }
    auto run_length = 1;
    for (auto i = 1u; i < dirty.size(); ++i) {
        auto& c = dirty[i];

        // Different character, or the same character but a digit?  Reset to it.
        if (c != prev_c || digits_.find(c) != digits_.end()) {
            clean->emplace_back(c);
            if (clean2dirty_or_null) {
                clean2dirty_or_null->emplace_back(i);
            }
            run_length = 1;
            prev_c = c;
            continue;
        }

        // A repeat character but not at the limit?  Keep it.
        ++run_length;
        if (run_length <= max_consecutive) {
            clean->emplace_back(c);
            if (clean2dirty_or_null) {
                clean2dirty_or_null->emplace_back(i);
            }
            continue;
        }

        // Repeated too many times?  Drop it.
        if (chr2drop_or_null) {
            ++(*chr2drop_or_null)[c];
        }
    }

    return true;
}
