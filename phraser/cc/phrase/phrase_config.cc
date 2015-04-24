#include "phrase_config.h"

static void DumpList(const vector<string>& v) {
    printf("[");
    if (v.size()) {
        printf("%s", v[0].c_str());
    }
    for (auto i = 1u; i < v.size(); ++i) {
        printf(", %s", v[i].c_str());
    }
    printf("]");
}

void PhraseConfig::Dump(size_t indent_level, size_t spaces_per_indent) const {
    string indent0 = string(indent_level * spaces_per_indent, ' ');
    string indent1 = string((indent_level + 1) * spaces_per_indent, ' ');

    printf("%sPhraseConfig {\n", indent0.c_str());
    printf("%sName: %s\n", indent1.c_str(), name.c_str());
    printf("%sSubsequence names: ", indent1.c_str());
    DumpList(block_names);
    printf("\n");
    printf("%sBlocks: (see its corresponding SequenceDetector)\n",
           indent1.c_str());
    printf("%s}\n", indent0.c_str());
}
