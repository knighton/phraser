#include "phrase_detection_result.h"

#include <cstdio>

static void DumpInts(const vector<size_t>& v) {
    printf("[");
    if (v.size()) {
        printf("%zu", v[0]);
    }
    for (auto i = 1u; i < v.size(); ++i) {
        printf(", %zu", v[i]);
    }
    printf("]");
}

static void DumpStrings(const vector<string>& v) {
    printf("[");
    if (v.size()) {
        printf("%s", v[0].c_str());
    }
    for (auto i = 1u; i < v.size(); ++i) {
        printf(", %s", v[i].c_str());
    }
    printf("]");
}

void PhraseMatch::Dump(
        size_t indent_level, size_t space_per_indent) const {
    string indent0(indent_level * space_per_indent, ' ');
    string indent1((indent_level + 1) * space_per_indent, ' ');

    printf("%sPhraseMatch {\n", indent0.c_str());
    printf("%sSubsequence begin indexes: ", indent1.c_str());
    DumpInts(piece_begin_indexes);
    printf("\n");
    printf("%sMatch end index (exclusive): %zu\n", indent1.c_str(), end_excl);
    printf("%s}\n", indent0.c_str());
}

void PhraseDetectionResult::Dump(
        size_t indent_level, size_t space_per_indent) const {
    string indent0 = string(indent_level * space_per_indent, ' ');
    string indent1 = string((indent_level + 1) * space_per_indent, ' ');

    printf("%sPhraseDetectionResult {\n", indent0.c_str());

    printf("%sPhrase name: %s\n", indent1.c_str(), phrase_name.c_str());

    printf("%sPhrase subsequence names: ", indent1.c_str());
    DumpStrings(piece_names);
    printf("\n");

    printf("%sMatches: [\n", indent1.c_str());
    for (auto i = 0u; i < matches.size(); ++i) {
        matches[i].Dump(indent_level + 2, space_per_indent);
    }
    printf("%s]\n", indent1.c_str());

    printf("%s}\n", indent0.c_str());
}
