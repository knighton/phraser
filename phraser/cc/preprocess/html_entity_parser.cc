#include "html_entity_parser.h"

#include "cc/preprocess/html_entity_parser_data.h"

bool HTMLEntityParser::Init(string* error) {
    name2code_ = html_entity_parser_data::HTML2UNICODE;
    max_name_length_ = 0;
    for (auto& it : name2code_) {
        auto& name = it.first;
        if (max_name_length_ < name.size()) {
            max_name_length_ = name.size();
        }
    }
    return true;
}

namespace {

bool ParseHex(const ustring& in, size_t begin, size_t end_excl,
              ucode* code) {
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

bool ParseDec(const ustring& in, size_t begin, size_t end_excl, ucode* code) {
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

bool ParseName(const ustring& in, size_t begin, size_t end_excl, string* name) {
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
        const ustring& in, size_t begin, size_t end_excl,
        ucode* code) const {
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
        const ustring& in, size_t amp_index, size_t semicolon_index,
        ustring* out, vector<uint16_t>* out2in) const {
    ucode c;
    if (ParseHTMLEntity(in, amp_index + 1, semicolon_index, &c)) {
        out->emplace_back(c);
        out2in->emplace_back(amp_index);
        return;
    }

    for (auto i = amp_index; i <= semicolon_index; ++i) {
        out->emplace_back(in[i]);
        out2in->emplace_back(i);
    }
}

bool HTMLEntityParser::IsPossibleHTMLEntityChar(ucode c) const {
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
        const ustring& in, ustring* out, vector<uint16_t>* out2in) const {
    out->clear();
    out2in->clear();

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
                out2in->emplace_back(i);
            }
            continue;
        }

        // Inside an HTML entity.
        if (c == '&') {
            // Ampersand?  Reject what we buffered and start here.
            for (auto j = begin; j < i; ++j) {
                out->emplace_back(in[j]);
                out2in->emplace_back(j);
            }
            begin = i;
        } else if (c == ';') {
            // Semicolon?  Try to parse it (append entity or string).
            AppendPossibleHTMLEntity(in, begin, i, out, out2in);
            begin = ~0ul;
            inside = false;
        } else if (!IsPossibleHTMLEntityChar(c)) {
            // Not possible inside an HTML entity?  Reject it.
            for (auto j = begin; j <= i; ++j) {
                out->emplace_back(in[j]);
                out2in->emplace_back(j);
            }
            begin = ~0ul;
            inside = false;
        }
    }

    if (inside) {
        for (auto i = begin; i < in.size(); ++i) {
            out->emplace_back(in[i]);
            out2in->emplace_back(i);
        }
    }
}
