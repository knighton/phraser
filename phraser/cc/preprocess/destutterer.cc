#include "destutterer.h"

#include "cc/preprocess/destutterer_data.h"

static bool LoadDigits(unordered_set<ucode>* digits, string* error) {
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
    digits_.clear();

    if (!LoadDigits(&digits_, error)) {
        return false;
    }

    return true;
}

bool Destutterer::Destutter(
        const ustring& in, size_t max_consecutive, ustring* out,
        vector<uint16_t>* out2in, unordered_map<ucode, size_t>* chr2drop,
        string* error) const {
    out->clear();
    out2in->clear();
    chr2drop->clear();

    if (in.empty()) {
        return true;
    }

    ucode prev_c = in[0];
    out->emplace_back(prev_c);
    out2in->emplace_back(0);
    auto run_length = 1u;
    for (auto i = 1u; i < in.size(); ++i) {
        auto& c = in[i];

        // Different character, or the same character but a digit?  Reset to it.
        if (c != prev_c || digits_.find(c) != digits_.end()) {
            out->emplace_back(c);
            out2in->emplace_back(i);
            run_length = 1;
            prev_c = c;
            continue;
        }

        // A repeat character but not at the limit?  Keep it.
        ++run_length;
        if (run_length <= max_consecutive) {
            out->emplace_back(c);
            out2in->emplace_back(i);
            continue;
        }

        // Repeated too many times?  Drop it.
        ++(*chr2drop)[c];
    }

    return true;
}
