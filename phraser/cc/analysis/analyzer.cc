#include "analyzer.h"

bool Analyzer::Init(const vector<string>& phrase_configs, string* error) {
    if (!preproc_.Init(error)) {
        return false;
    }

    if (!tok_.InitDefault(error)) {
        return false;
    }

    if (!phrase_.Init(phrase_configs, error)) {
        return false;
    }

    ready_ = true;
    return true;
}

bool Analyzer::Analyze(
        const ustring& original_text, const AnalysisOptions& options,
        AnalysisResult* result, string* error) const {
    if (!ready_) {
        *error = "[Analyzer] You forgot to call Init().";
        return false;
    }

    result->Clear();
    error->clear();

    result->original_text = original_text;

    if (!preproc_.Preprocess(
            result->original_text, options.replace_html_entities,
            options.destutter_max_consecutive, &result->clean_text,
            &result->clean2original, &result->chr2drop, error)) {
        return false;
    }

    if (!tok_.Tokenize(result->clean_text, &result->tokens,
                       &result->token2clean)) {
        return false;
    }

    vector<PhraseDetectionResult> phrase_results;
    if (!phrase_.Detect(result->tokens, &result->phrase_results, error)) {
        return false;
    }

    return true;
}
