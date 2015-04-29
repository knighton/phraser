#include "phrase_detector.h"

#include "cc/misc/files.h"

bool PhraseDetector::LoadPhraseConfig(const string& phrase_f, string* error) {
    string text;
    if (!files::FileToString(phrase_f, &text)) {
        *error = "[PhraseDetector] Unable to read file: [" + phrase_f + "]";
        return false;
    }

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

bool PhraseDetector::InitFromFiles(
        const EnglishConfig& config, const vector<string>& phrase_ff,
        string* error) {
    if (!vocab_.InitWithConfig(config, error)) {
        return false;
    }

    for (auto& phrase_f : phrase_ff) {
        if (!LoadPhraseConfig(phrase_f, error)) {
            return false;
        }
    }

    return true;
}

void PhraseDetector::Dump(size_t indent_level, size_t spaces_per_indent) const {
    // TODO
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
            size_t cur_index = sequence_match.begin();
            for (auto j = 0u; j < sequence_match.option_choices().size(); ++j) {
                auto& piece_index = j;
                auto& choice = sequence_match.option_choices()[j];
                m.piece_begin_indexes.emplace_back(cur_index);
                cur_index += phrase.blocks[piece_index][choice].size();
            }
            m.end_excl = cur_index;
            result.matches.emplace_back(m);
        }
    }

    return true;
}
