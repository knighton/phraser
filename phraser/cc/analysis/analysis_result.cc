#include "analysis_result.h"

#include <cstdio>

#include "cc/misc/json.h"
#include "cc/misc/strings.h"

void AnalysisResult::Clear() {
    original_text.clear();

    clean_text.clear();
    clean2original.clear();
    chr2drop.clear();

    tokens.clear();
    token2clean.clear();

    phrase_results.clear();
};

json::Object* AnalysisResult::ToJSON() const {
    map<string, json::Object*> chr2drop_j;
    for (auto& it : chr2drop) {
        auto chr_s = strings::StringPrintf("0x", it.first);
        chr2drop_j[chr_s] = new json::Object(it.second);
    }

    vector<json::Object*> token2clean_j;
    for (auto& span : token2clean) {
        vector<uint16_t> v;
        v.emplace_back(span.begin);
        v.emplace_back(span.end_excl);
        token2clean_j.emplace_back(new json::Object(v));
    }

    vector<json::Object*> phrase_results_j;
    for (auto& pr : phrase_results) {
        phrase_results_j.emplace_back(pr.ToJSON());
    }

    return new json::Object({
        {"original_text",  new json::Object(original_text)},

        {"clean_text",     new json::Object(clean_text)},
        {"clean2original", new json::Object(clean2original)},
        {"chr2drop",       new json::Object(chr2drop_j)},

        {"tokens",         new json::Object(tokens)},
        {"token2clean",    new json::Object(token2clean_j)},

        {"phrase_results", new json::Object(phrase_results_j)},
    });
}

static void PrintableAsciiOnly(const string& in, string* out) {
    out->clear();
    for (auto& c : in) {
        *out += PrintableAsciiOnly(c);
    }
}

static void PrintableAsciiOnly(const ustring& in, string* out) {
    out->clear();
    for (auto& c : in) {
        *out += PrintableAsciiOnly(c);
    }
}

void AnalysisResult::Dump() const {
    string original_as_ascii;
    PrintableAsciiOnly(original_text, &original_as_ascii);

    string clean_as_ascii;
    PrintableAsciiOnly(clean_text, &clean_as_ascii);

    printf("AnalysisResult:\n");
    printf("* Original:\n");
    printf("  * original_text (to ascii) = [%s]\n", original_as_ascii.data());
    printf("\n");
    printf("* Preprocessing results:\n");
    printf("  * clean_text (to ascii) =    [%s]\n", clean_as_ascii.data());
    printf("  * chr2drop = {");
    for (auto& it : chr2drop) {
        printf("0x%x = %zu,", it.first, it.second);
    }
    printf("}\n");
    printf("\n");

    printf("* Tokenization results:\n");
    printf("  * tokens = [");
    for (auto& s : tokens) {
        printf("%s ", s.data());
    }
    printf("]\n");
    printf("  * token2clean =");
    for (auto& span : token2clean) {
        printf(" [%u %u)", span.begin, span.end_excl);
    }
    printf("\n\n");

    printf("* Phrase detection results:\n");
    printf("  * phrase_results = %zu\n", phrase_results.size());
    printf("\n");
    for (auto i = 0u; i < phrase_results.size(); ++i) {
        string s;
        phrase_results[i].ToJSON()->AppendToString(&s);
        printf("%s\n", s.data());
        s.clear();
    }
}

static void AppendCharHTML(uchar c, string* s) {
    if (0x20 <= c && c <= 0x7E) {
        *s += static_cast<char>(c);
    } else if (c == '\n') {
        *s += ' ';
    } else {
        *s += "&#";
        *s += strings::StringPrintf("%d", c);
        *s += ';';
    }
}

void AnalysisResult::ToHTML(string* s) const {
    *s += "<div class=\"comment\">";

    if (phrase_results.empty()) {
        for (auto& c : original_text) {
            AppendCharHTML(c, s);
        }
        *s += "</div>\n";
        return;
    }

    for (auto& phrase : phrase_results) {
        for (auto& match : phrase.matches) {
            // Token indexes.
            uint16_t begin = static_cast<uint16_t>(
                match.piece_begin_indexes[0]);
            uint16_t end_excl = static_cast<uint16_t>(match.end_excl);

            // Clean char indexes.
            begin = token2clean[begin].begin;
            end_excl = token2clean[end_excl - 1].end_excl;

            // Original char indexes.
            begin = clean2original[begin];
            end_excl = clean2original[end_excl];

            for (auto i = 0u; i < original_text.size(); ++i) {
                if (i == begin) {
                    *s += "<span class=\"match\">";
                }
                if (i == end_excl) {
                    *s += "</span>";
                }
                auto& c = original_text[i];
                AppendCharHTML(c, s);
            }
        }
    }
    *s += "</div>\n";
}
