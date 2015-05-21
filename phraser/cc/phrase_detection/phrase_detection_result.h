#ifndef CC_PHRASING_PHRASE_DETECTION_RESULT_H_
#define CC_PHRASING_PHRASE_DETECTION_RESULT_H_

#include <string>
#include <vector>

#include "cc/base/json.h"

using std::string;
using std::vector;

struct PhraseMatch {
    vector<size_t> piece_begin_indexes;
    size_t end_excl;

    json::Object* ToJSON() const;
};

struct PhraseDetectionResult {
    string phrase_name;
    vector<string> piece_names;
    vector<PhraseMatch> matches;

    json::Object* ToJSON() const;
};

#endif  // CC_PHRASING_PHRASE_DETECTION_RESULT_H_
