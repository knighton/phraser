#include "ptb_ascii_normalizer.h"

#include "cc/frontend/ascii/ptb_ascii_normalizer_data.h"

bool PtbAsciiNormalizer::Init(string* error) {
    ascii_data_ = ptb_ascii_normalizer_data::ASCII_DATA;
    unicode2ascii_ = ptb_ascii_normalizer_data::UNICODE2ASCII;
    return true;
}

void PtbAsciiNormalizer::Normalize(
        const ustring& in, string* out, vector<ChrIndex>* out2in) const {
    out->clear();
    out2in->clear();
    for (auto i = 0u; i < in.size(); ++i) {
        auto& c = in[i];

        // If it maps to printable ASCII or is printable ASCII but overridden,
        // use that.
        auto it = unicode2ascii_.find(c);
        if (it != unicode2ascii_.end()) {
            auto& index = it->second;
            const char* pointer = &ascii_data_[index];
            while (*pointer != '\n') {
                *out += *pointer;
                out2in->emplace_back(i);
                ++pointer;
            }
            continue;
        }

        // Else, if it's printable ASCII, use it as is.
        if (0x20 <= c && c < 0x7F) {
            *out += static_cast<char>(c);
            out2in->emplace_back(i);
            continue;
        }

        // Failing those, add whitespace if it doesn't already end with
        // whitespace.
        if (out->empty() || (*out)[out->size() - 1] != ' ') {
            *out += ' ';
            out2in->emplace_back(i);
        }
    }
}
