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
    if (!detector.Init(phrase.blocks(), error)) {
        return false;
    }

    return true;
}

bool PhraseDetector::InitFromFiles(
        const EnglishConfig& config, const vector<string>& phrase_ff,
        string* error) {
    if (!vocab_.InitWithConfig(config)) {
        printf("adsfadfasfd\n");
        return false;
    }

    for (auto& phrase_f : phrase_ff) {
        if (!LoadPhraseConfig(phrase_f, error)) {
            return false;
        }
    }

    return true;
}
