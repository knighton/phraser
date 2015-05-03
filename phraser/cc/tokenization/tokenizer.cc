#include "tokenizer.h"

#include "cc/misc/strings.h"
#include "cc/tokenization/tokenizer_data.h"

bool Tokenizer::Init(
        const string& ascii_data,
        const unordered_map<uchar, uint16_t>& unicode2ascii,
        const unordered_map<string, string>& token2token, string* error) {
    ascii_data_ = ascii_data;
    unicode2ascii_ = unicode2ascii;
    token2token_ = token2token;
    return true;
}

static bool MakeToken2Token(const string& s2z_s, const string& pairs_s,
                            unordered_map<string, string>* token2token,
                            string* error) {
    token2token->clear();

    vector<string> v;

    strings::SplitByWhitespace(s2z_s, &v);
    for (auto& s : v) {
        auto z = s;
        for (auto i = 0u; i < z.size(); ++i) {
            if (z[i] == 's') {
                z[i] = 'z';
            }
        }

        auto it = token2token->find(s);
        if (it != token2token->end()) {
            *error = "[Tokenizer] Duplicate key encountered: [" + s + "].";
            return false;
        }

        (*token2token)[s] = z;
    }

    strings::SplitByWhitespace(pairs_s, &v);
    if (v.size() % 2) {
        return false;
    }

    for (auto i = 0u; i < v.size(); i += 2) {
        auto& from_s = v[i];
        auto& to_s = v[i + 1];

        auto it = token2token->find(from_s);
        if (it != token2token->end()) {
            *error = "[Tokenizer] Duplicate key encountered: [" + from_s + "].";
            return false;
        }

        (*token2token)[from_s] = to_s;
    }

    return true;
}

bool Tokenizer::InitDefault(string* error) {
    auto& ascii_data = tokenizer_data::ASCII_DATA;
    auto& unicode2ascii = tokenizer_data::UNICODE2ASCII;

    unordered_map<string, string> token2token;
    if (!MakeToken2Token(
            tokenizer_data::NRM_TOKEN_S2Z, tokenizer_data::NRM_TOKEN_PAIRS,
            &token2token, error)) {
        return false;
    }

    return Init(ascii_data, unicode2ascii, token2token, error);
}

void Tokenizer::UnicodeToPTBAscii(
        const ustring& in, string* out, vector<uint16_t>* out2in) const {
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

void Tokenizer::NormalizeTokens(vector<string>* tokens) const {
    for (auto i = 0u; i < tokens->size(); ++i) {
        auto& s = (*tokens)[i];

        // Lowercase.
        for (auto j = 0u; j < s.size(); ++j) {
            if ('A' <= s[j] && s[j] <= 'Z') {
                s[j] -= 'A';
                s[j] += 'a';
            }
        }

        // Respell/escape.
        auto it = token2token_.find(s);
        if (it != token2token_.end()) {
            s = it->second;
        }
    }
}

// Assume indexes are correct.
static void CombineMappings(const vector<uint16_t>& ascii2unicode,
                            vector<Span>* spans) {
    for (auto i = 0u; i < spans->size() - 1; ++i) {
        auto& span = (*spans)[i];
        span.begin = ascii2unicode[span.begin];
        span.end_excl = ascii2unicode[span.end_excl];
    }

    // Last token span's exclusive end index goes off the end by one.
    auto& span = (*spans)[spans->size() - 1];
    span.begin = ascii2unicode[span.begin];
    span.end_excl = static_cast<uint16_t>(ascii2unicode.size());
}

bool Tokenizer::Tokenize(
        const ustring& unicode, vector<string>* tokens,
        vector<Span>* token2unicode) const {
    // Unicode -> ASCII.
    string ascii;
    vector<uint16_t> ascii2unicode;
    UnicodeToPTBAscii(unicode, &ascii, &ascii2unicode);

    // ASCII -> tokens.
    if (!ascii_tokenizer_.Tokenize(ascii, tokens, token2unicode)) {
        return false;
    }

    // Transitively combine index mappings.
    CombineMappings(ascii2unicode, token2unicode);

    // Tokens -> tokens.
    NormalizeTokens(tokens);

    return true;
}
