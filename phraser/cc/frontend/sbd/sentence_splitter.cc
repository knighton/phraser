#include "sentence_splitter.h"

#include <vector>

using boost::smatch;
using boost::regex_match;
using boost::regex_replace;
using std::vector;

void SentenceSplitter::Init() {
    ellipsis_re_ = "\\.( *\\.){2,}";
    url_re_ = "^http*";

    abbrevs_ = {
        "mr",
        "ms",
        "mrs",

        "st",
    };
}

namespace {

void GetPrevWord(const string& text, size_t end_incl, string* word) {
    word->clear();

    if (!(end_incl < text.size())) {
        return;
    }

    bool found = false;
    size_t i = end_incl;
    while (i < text.size()) {
        if (text[i] == ' ') {
            found = true;
            break;
        }
        --i;
    }

    size_t begin;
    if (found) {
        begin = i + 1;
    } else {
        begin = 0;
    }

    *word = text.substr(begin, end_incl + 1 - begin);
}

void GetNextWord(const string& text, size_t begin, string* word) {
    while (begin < text.size() && text[begin] == ' ') {
        ++begin;
    }

    size_t i = begin;
    while (i < text.size()) {
        if (text[i] == ' ') {
            break;
        }
        ++i;
    }
    *word = text.substr(begin, i - begin);
}

void LowerCase(string* s) {
    for (auto& c : *s) {
        if ('A' <= c && c <= 'Z') {
            c -= 'A';
            c += 'a';
        }
    }
}

bool IsCapitalized(const string& s) {
    if (s.empty()) {
        return false;
    }

    return 'A' <= s[0] && s[0] <= 'Z';
}

bool IsEmptyOrWhitespace(const string& s, size_t begin, size_t end_excl) {
    if (begin + 1 == end_excl) {
        return true;
    }

    for (auto i = begin; i < end_excl; ++i) {
        auto& c = s[i];
        if (c != ' ') {
            return false;
        }
    }

    return true;
}

}  // namespace

void SentenceSplitter::Split(const string& in, string* out) const {
    out->clear();

    // Replace (ASCII) ellipses with a magic character.
    string text = regex_replace(in, ellipsis_re_, "\x7F");

    vector<size_t> ends;
    for (auto i = 1u; i < text.size() - 1; ++i) {
        auto& c = text[i];

        // Handle '!'.
        if (c == '!') {
            ends.emplace_back(i);
            continue;
        }

        // Handle '?'.
        if (c == '?') {
            // Does the ? stand alone?
            if (text[i - 1] == ' ') {
                ends.emplace_back(i);
                continue;
            }

            // Make sure the ? isn't part of a URI.
            string prev_word;
            GetPrevWord(text, i - 1, &prev_word);
            smatch what;
            if (!regex_match(prev_word, what, url_re_)) {
                ends.emplace_back(i);
                continue;
            }

            continue;
        }

        // Not a '.'?  Reject.
        if (c != '.') {
            continue;
        }

        // Not the end of a token?  Reject.
        auto& next_c = text[i + 1];
        if (next_c != ' ') {
            continue;
        }

        // Is the previous word an abbreviation?  Reject.
        string prev_word;
        GetPrevWord(text, i - 1, &prev_word);
        string lower_prev_word = prev_word;
        LowerCase(&lower_prev_word);
        if (abbrevs_.find(lower_prev_word) != abbrevs_.end()) {
            printf("FOUND ABBREV\n");
            continue;
        }

        // Is the following word is capitalized?  Accept.
        string next_word;
        GetNextWord(text, i + 1, &next_word);
        if (IsCapitalized(next_word)) {
            ends.emplace_back(i);
            continue;
        }

        // If previous word is an initial, and next word is lowercase?  Reject.
        if (prev_word.size() == 1 && IsCapitalized(prev_word)) {
            continue;
        }

        // Accept.
        ends.emplace_back(i);
    }

    ends.emplace_back(text.size() - 1);

    size_t begin = 0;
    vector<size_t> actual_ends;
    for (auto& end_incl : ends) {
        if (!IsEmptyOrWhitespace(text, begin, end_incl)) {
            actual_ends.emplace_back(end_incl);
        }
        begin = end_incl + 1;
    }

    begin = 0;
    for (auto& end_incl : actual_ends) {
        for (auto i = begin; i < end_incl; ++i) {
            auto& c = text[i];
            if (c == 0x7F) {
                *out += " ... ";
            } else {
                *out += c;
            }
        }

        *out += ' ';
        *out += text[end_incl];
        *out += ' ';

        begin = end_incl + 1;
    }
}
