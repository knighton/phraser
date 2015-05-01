#include "html_entity_parser.h"

bool HTMLEntityParser::InitDefault() {
    return false;  // XXX
}

namespace {

bool ParseHex(const vector<UChar>& in, size_t begin, size_t end_excl,
              UChar* code) {
    *code = 0;
    for (auto i = begin; i < end_excl; ++i) {
        auto c = in[i];
        if ('0' <= c && c <= '9') {
            c -= '0';
        } else if ('a' <= c && c <= 'z') {
            c -= 'a';
            c += 10;
        } else if ('A' <= c && c <= 'Z') {
            c -= 'A';
            c += 10;
        } else {
            return false;
        }
        *code *= 16;
        *code += c;
    }
    return true;
}

bool ParseDec(const vector<UChar>& in, size_t begin, size_t end_excl,
              UChar* code) {
    *code = 0;
    for (auto i = begin; i < end_excl; ++i) {
        auto c = in[i];
        if ('0' <= c && c <= '9') {
            c -= '0';
        } else {
            return false;
        }
        *code *= 10;
        *code += c;
    }
    return true;
}

bool ParseName(const vector<UChar>& in, size_t begin, size_t end_excl,
               string* name) {
    name->clear();
    for (auto i = begin; i < end_excl; ++i) {
        auto& c = in[i];
        if (('0' <= c && c <= '9') || ('a' <= c && c <= 'z') ||
                ('A' <= c && c <= 'Z')) {
            *name += static_cast<char>(c);
        } else {
            return false;
        }
    }
    return true;
}

}  // namespace

bool HTMLEntityParser::ParseHTMLEntity(
        const vector<UChar>& in, size_t begin, size_t end_excl,
        UChar* code) const {
    size_t length = end_excl - begin;
    if (1 <= length && in[begin] == '#') {
        if (2 <= length && in[begin + 1] == 'x') {
            if (!ParseHex(in, begin + 2, end_excl, code)) {
                return false;
            }
        } else {
            if (!ParseDec(in, begin + 1, end_excl, code)) {
                return false;
            }
        }
    } else {
        if (max_name_length_ < length) {
            return false;
        }

        string name;
        if (!ParseName(in, begin, end_excl, &name)) {
            return false;
        }

        auto it = name2code_.find(name);
        if (it == name2code_.end()) {
            return false;
        }

        *code = it->second;
    }

    return true;
}

void HTMLEntityParser::AppendPossibleHTMLEntity(
        const vector<UChar>& in, size_t amp_index, size_t semicolon_index,
        vector<UChar>* out) const {
    UChar c;
    if (ParseHTMLEntity(in, amp_index + 1, semicolon_index, &c)) {
        out->emplace_back(c);
        return;
    }

    for (auto i = amp_index; i <= semicolon_index; ++i) {
        out->emplace_back(in[i]);
    }
}

bool HTMLEntityParser::IsPossibleHTMLEntityChar(UChar c) const {
    if (c == '#') {
        return true;
    }

    if ('0' <= c && c <= '9') {
        return true;
    }

    if ('a' <= c && c <= 'z') {
        return true;
    }

    if ('A' <= c && c <= 'Z') {
        return true;
    }

    return false;
}

void HTMLEntityParser::Replace(
        const vector<UChar>& in, vector<UChar>* out) const {
    size_t begin = ~0ul;
    bool inside = false;
    for (auto i = 0u; i < in.size(); ++i) {
        auto& c = in[i];

        // Not inside an HTML entity.
        if (!inside) {
            if (c == '&') {
                begin = i;
                inside = true;
            } else {
                out->emplace_back(c);
            }
            continue;
        }

        // Inside an HTML entity.
        if (c == '&') {
            // Ampersand?  Reject what we buffered and start here.
            for (auto j = begin; j < i; ++j) {
                out->emplace_back(in[j]);
            }
            begin = i;
        } else if (c == ';') {
            // Semicolon?  Try to parse it (append entity or string).
            AppendPossibleHTMLEntity(in, begin, i, out);
            begin = ~0ul;
            inside = false;
        } else if (!IsPossibleHTMLEntityChar(c)) {
            // Not possible inside an HTML entity?  Reject it.
            for (auto j = begin; j <= i; ++j) {
                out->emplace_back(in[j]);
            }
            begin = ~0ul;
            inside = false;
        }
    }

    if (inside) {
        for (auto i = begin; i < in.size(); ++i) {
            out->emplace_back(in[i]);
        }
    }
}
