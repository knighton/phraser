#include "phrase_detector.h"

#include "cc/base/files.h"

bool PhraseDetector::LoadPhraseConfig(const string& text, string* error) {
    phrases_.resize(phrases_.size() + 1);
    auto& phrase = phrases_[phrases_.size() - 1];
    if (!phrase_parser_.Parse(text, &vocab_, &phrase, error)) {
        return false;
    }

    detectors_.resize(detectors_.size() + 1);
    auto& detector = detectors_[detectors_.size() - 1];
    if (!detector.Init(phrase.blocks, error)) {
        return false;
    }

    return true;
}

bool PhraseDetector::Init(const vector<string>& phrase_configs, string* error) {
    if (!vocab_.Init(error)) {
        return false;
    }

    for (auto& phrase_config : phrase_configs) {
        if (!LoadPhraseConfig(phrase_config, error)) {
            return false;
        }
    }

    return true;
}

json::Object* PhraseDetector::ToJSON() const {
    vector<json::Object*> phrases;
    phrases.reserve(phrases_.size());
    for (auto& phrase : phrases_) {
        phrases.emplace_back(phrase.ToJSON());
    }

    vector<json::Object*> detectors;
    detectors.reserve(detectors_.size());
    for (auto& detector : detectors_) {
        detectors.emplace_back(detector.ToJSON());
    }

    return new json::Object({
        {"vocab", vocab_.ToJSON()},
        {"phrases", new json::Object(phrases)},
        {"detectors", new json::Object(detectors)},
    });
}

bool PhraseDetector::Detect(
        const vector<string>& tokens, vector<PhraseDetectionResult>* results,
        string* error) const {
    results->clear();

    // Convert token strings -> lists of integers using all the evaluators.
    vector<vector<TokenGroupID>> group_id_lists;
    if (!vocab_.EvaluateTokens(tokens, &group_id_lists, error)) {
        return false;
    }

    // For each phrase (config and detector),
    for (auto i = 0u; i < detectors_.size(); ++i) {
        auto& detector = detectors_[i];
        auto& phrase = phrases_[i];

        // Get the list of sequence matches, if any.
        vector<SequenceMatch> sequence_matches;
        detector.GetMatches(group_id_lists, &sequence_matches);
        if (sequence_matches.empty()) {
            continue;
        }

        // Create the PhraseDetectionResult.
        results->resize(results->size() + 1);
        auto& result = (*results)[results->size() - 1];
        result.phrase_name = phrase.name;
        result.piece_names = phrase.block_names;
        result.matches.reserve(sequence_matches.size());

        // For each sequence match,
        for (auto& sequence_match : sequence_matches) {
            PhraseMatch m;

            // Get piece begin indexes.
            size_t cur_index = sequence_match.begin();
            for (auto j = 0u; j < sequence_match.option_choices().size(); ++j) {
                auto& piece_index = j;
                auto& choice = sequence_match.option_choices()[j];
                m.piece_begin_indexes.emplace_back(cur_index);
                cur_index += phrase.blocks[piece_index][choice].size();
            }
            m.end_excl = cur_index;

            // Sanity check against what we already know.
            if (m.end_excl != sequence_match.end_excl()) {
                *error = "Code error.";
                return false;
            }

            result.matches.emplace_back(m);
        }
    }

    return true;
}
