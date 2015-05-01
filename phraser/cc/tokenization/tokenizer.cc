#include "tokenizer.h"

#include "cc/misc/strings.h"
#include "cc/tokenization/tokenizer_data.h"

bool Tokenizer::Init(
        const string& ascii_data,
        const unordered_map<UChar, uint16_t>& unicode2ascii,
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

void Tokenizer::UnicodeToPTBAscii(const vector<UChar>& in, string* out) const {
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
                ++pointer;
            }
            continue;
        }

        // Else, if it's printable ASCII, use it as is.
        if (0x20 <= c && c < 0x7F) {
            *out += static_cast<char>(c);
            continue;
        }

        // Failing those, add whitespace if it doesn't already end with
        // whitespace.
        if (out->empty() || (*out)[out->size() - 1] != ' ') {
            *out += ' ';
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

void Tokenizer::Tokenize(
        const vector<UChar>& text, vector<string>* tokens,
        vector<Span>* token2clean_or_null) const {
    // Unicode -> ASCII.
    string ptb_ascii;
    UnicodeToPTBAscii(text, &ptb_ascii);

    // ASCII -> tokens.
    ascii_tokenizer_.Tokenize(ptb_ascii, tokens);

    // Tokens -> tokens.
    NormalizeTokens(tokens);
}
