#include "phrase_detection_result.h"

#include <cstdio>

json::Object* PhraseMatch::ToJSON() const {
    // For clang++ on OS X.
    const vector<uint64_t>& tmp =
        *reinterpret_cast<const vector<uint64_t>*>(&piece_begin_indexes);
    return new json::Object({
        {"piece_begin_indexes", new json::Object(tmp)},
        {"end_excl", new json::Object(static_cast<int64_t>(end_excl))},
    });
}

json::Object* PhraseDetectionResult::ToJSON() const {
    vector<json::Object*> match_objects;
    match_objects.reserve(matches.size());
    for (auto& match : matches) {
        match_objects.emplace_back(match.ToJSON());
    }

    return new json::Object({
        {"phrase_name", new json::Object(phrase_name)},
        {"piece_names", new json::Object(piece_names)},
        {"matches", new json::Object(match_objects)},
    });
}
