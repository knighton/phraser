#ifndef CC_PHRASING_PHRASE_DETECTION_RESULT_H_
#define CC_PHRASING_PHRASE_DETECTION_RESULT_H_

#include <string>
#include <vector>

using std::string;
using std::vector;

struct PhraseMatch {
    vector<size_t> piece_begin_indexes;
    size_t end_excl;

    void Dump(size_t indent_level, size_t space_per_indent) const;
};

struct PhraseDetectionResult {
    string phrase_name;
    vector<string> piece_names;
    vector<PhraseMatch> matches;

    void Dump(size_t indent_level, size_t space_per_indent) const;
};

#endif  // CC_PHRASING_PHRASE_DETECTION_RESULT_H_
