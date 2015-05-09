#include "lapos_tokenizer.h"

#include <cstdlib>
#include <iostream>
#include <list>
#include <string>
#include <vector>
#include <sstream>

using std::string;
using std::vector;
using std::istringstream;

namespace {

void Replace(string& s, const string& s1, const string& s2, const char skip=0,
             bool left=true) {
    string::size_type pos = 0;
    while (1) {
        string::size_type i = s.find(s1, pos);
        if (i == string::npos) {
            break;
        }
        if (skip) {
            if (left && i > 0 && s[i - 1] == skip) {
                pos = i + 1;
                continue;
            } else if (i < s.size() - 1 && s[i + 1] == skip) {
                pos = i + 1;
                continue;
            }
        }
        s.replace(i, s1.size(), s2);
        pos = i + s2.size();
    }
}

void SeparateCommas(string& s) {
    const int n = s.size();

    string t;
    for (int i = 0; i < n; i++) {
        if (s[i] == ',') {
            if (!(i > 0 && isdigit(s[i - 1]) && i < n - 1 &&
                  isdigit(s[i + 1]))) {
                t += " , ";
                continue;
            }
        }
        t += string(1, s[i]);
    }

    s = t;
}

void ReallyTokenize(const string& input_text, vector<string>* out) {
    out->clear();

    if (input_text.size() == 0) {
        return;
    }

    string s = input_text;

    Replace(s, "``", " `` ");
    if (s[0] == '"') {
        s.replace(0, 1, "`` ");
    }
    if (s.size() > 2 && s[0] == '`' && s[1] != '`') {
        s.replace(0, 1, "` ");
    }
    Replace(s, " \"", "  `` ");
    Replace(s, "(\"", "( `` ");
    Replace(s, "[\"", "[ `` ");
    Replace(s, "{\"", "{ `` ");
    Replace(s, "<\"", "< `` ");
    Replace(s, " `", "  ` ", '`', false);
    Replace(s, "(`", "( ` ", '`', false);
    Replace(s, "[`", "[ ` ", '`', false);
    Replace(s, "{`", "{ ` ", '`', false);
    Replace(s, "<`", "< ` ", '`', false);

    Replace(s, "...", " ... ");

    SeparateCommas(s);
    Replace(s, ";", " ; ");
    Replace(s, ":", " : ");
    Replace(s, "@", " @ ");
    Replace(s, "#", " # ");
    Replace(s, "$", " $ ");
    Replace(s, "%", " % ");
    Replace(s, "&", " & ");

    int pos = s.size() - 1;
    while (pos > 0 && s[pos] == ' ') {
        pos--;
    }
    while (pos > 0) {
        char c = s[pos];
        if (c == '[' || c == ']' || c == ')' || c == '}' || c == '>' ||
            c == '"' || c == '\'') {
            pos--;
            continue;
        }
        break;
    }
    if (s[pos] == '.' && !(pos > 0 && s[pos - 1] == '.')) {
        s.replace(pos, 1, " .");
    }

    Replace(s, "?", " ? ");
    Replace(s, "!", " ! ");
    Replace(s, "[", " [ ");
    Replace(s, "]", " ] ");
    Replace(s, "(", " ( ");
    Replace(s, ")", " ) ");
    Replace(s, "{", " { ");
    Replace(s, "}", " } ");
    Replace(s, "<", " < ");
    Replace(s, ">", " > ");

    Replace(s, "--", " -- ");

    s.replace(string::size_type(0), 0, " ");
    s.replace(s.size(), 0, " ");

    Replace(s, "''", " '' ");
    Replace(s, "\"", " '' ");

    Replace(s, "' ", " ' ", '\'');
    Replace(s, "'s ", " 's ");
    Replace(s, "'S ", " 'S ");
    Replace(s, "'m ", " 'm ");
    Replace(s, "'M ", " 'M ");
    Replace(s, "'d ", " 'd ");
    Replace(s, "'D ", " 'D ");
    Replace(s, "'ll ", " 'll ");
    Replace(s, "'re ", " 're ");
    Replace(s, "'ve ", " 've ");
    Replace(s, "n't ", " n't ");
    Replace(s, "'LL ", " 'LL ");
    Replace(s, "'RE ", " 'RE ");
    Replace(s, "'VE ", " 'VE ");
    Replace(s, "N'T ", " N'T ");

    Replace(s, " Cannot ", " Can not ");
    Replace(s, " cannot ", " can not ");
    Replace(s, " D'ye ", " D' ye ");
    Replace(s, " d'ye ", " d' ye ");
    Replace(s, " Gimme ", " Gim me ");
    Replace(s, " gimme ", " gim me ");
    Replace(s, " Gonna ", " Gon na ");
    Replace(s, " gonna ", " gon na ");
    Replace(s, " Gotta ", " Got ta ");
    Replace(s, " gotta ", " got ta ");
    Replace(s, " Lemme ", " Lem me ");
    Replace(s, " lemme ", " lem me ");
    Replace(s, " More'n ", " More 'n ");
    Replace(s, " more'n ", " more 'n ");
    Replace(s, "'Tis ", " 'T is ");
    Replace(s, "'tis ", " 't is ");
    Replace(s, "'Twas ", " 'T was ");
    Replace(s, "'twas ", " 't was ");
    Replace(s, " Wanna ", " Wan na ");
    Replace(s, " wanna ", " wan na ");

    if (s[s.size() - 1] == '\'') {
        s.replace(s.size() - 1, 1, " ' ");
    }

    istringstream is(s);
    string t;
    while (is >> t) {
        out->emplace_back(t);
    }
}

size_t FindOneOf(const string& s, size_t search_from, const string& cc) {
    for (auto i = search_from; i < s.size(); ++i) {
        for (auto& c : cc) {
            if (s[i] == c) {
                return i;
            }
        }
    }
    return ~0ul;
}

size_t FindFirstNot(const string& s, size_t search_from, char c) {
    for (auto i = search_from; i < s.size(); ++i) {
        if (s[i] != c) {
            return i;
        }
    }
    return ~0ul;
}

bool GetSpans(const string& text, const vector<string>& tokens,
              vector<TokenSpan>* spans) {
    spans->reserve(tokens.size());
    size_t search_from = 0;
    for (auto& token : tokens) {
        TokenSpan span;
        if (token == "''") {
            size_t x = FindOneOf(text, search_from, "\"'");
            if (UINT16_MAX < x) {
                return false;
            }
            span.begin = static_cast<uint16_t>(x);
            if (text.size() < span.begin) {
                return false;
            } else if (text[span.begin] == '"') {
                span.end_excl = span.begin + 1;
            } else {
                span.end_excl = span.begin + 2;
            }
        } else if (token == "``") {
            size_t x = FindOneOf(text, search_from, "\"`");
            if (UINT16_MAX < x) {
                return false;
            }
            span.begin = static_cast<uint16_t>(x);
            if (text.size() < span.begin) {
                return false;
            } else if (text[span.begin] == '"') {
                span.end_excl = span.begin + 1;
            } else {
                span.end_excl = span.begin + 2;
            }
        } else {
            size_t x = FindFirstNot(text, search_from, ' ');
            if (UINT16_MAX < x) {
                return false;
            }
            span.begin = static_cast<uint16_t>(x);
            if (text.size() < span.begin) {
                return false;
            } else {
                span.end_excl =
                    span.begin + static_cast<uint16_t>(token.size());
            }
        }

        // Assume that the characters match (see asumptions list in Tokenize()).
        if (text.size() < span.end_excl) {
            return false;
        }

        spans->emplace_back(span);
        search_from = span.end_excl;
    }

    return true;
}

}  // namespace

bool LaposTokenizer::Tokenize(
        const string& in, vector<string>* out, vector<TokenSpan>* spans) const {
    // Assumptions we can make in order to efficiently compute original spans:
    //
    // * The input string's size < UINT16_MAX.
    //
    // * Tokenization only inserts spacing.
    //   * Except for " -> `` or ''.
    //
    // * `` and '' may already exist in the input string (due to preprocessing).
    //
    // * The input string only contains printable ASCII (therefore the only
    //   possible whitespace character is space/0x20).

    // Tokenize.
    ReallyTokenize(in, out);

    // Map tokens to spans.
    if (!GetSpans(in, *out, spans)) {
        return false;
    }

    return true;
}
