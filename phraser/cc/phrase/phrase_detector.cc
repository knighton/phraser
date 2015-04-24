#include "phrase_detector.h"

#include "cc/misc/files.h"

bool PhraseDetector::LoadPhraseConfig(const string& phrase_f) {
    string text;
    if (!files::FileToString(phrase_f, &text)) {
        return false;
    }

    phrases_.resize(phrases_.size() + 1);
    auto& phrase = phrases_[phrases_.size() - 1];
    if (!phrase_parser_.Parse(text, &vocab_, &phrase)) {
        return false;
    }

    detectors_.resize(detectors_.size() + 1);
    auto& detector = detectors_[detectors_.size() - 1];
    if (!detector.Init(phrase.blocks())) {
        return false;
    }

    return true;
}

bool PhraseDetector::InitFromFiles(
        const EnglishConfig& config, const vector<string>& phrase_ff) {
    if (!vocab_.InitWithConfig(config)) {
        return false;
    }

    for (auto& phrase_f : phrase_ff) {
        if (!LoadPhraseConfig(phrase_f)) {
            return false;
        }
    }

    return true;
}
