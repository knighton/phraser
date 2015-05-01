#include "analyzer.h"

bool Analyzer::Init(
        const string& lapos_model_f, const vector<string>& phrase_config_ff,
        string* error) {
    if (!preproc_.Init(error)) {
        return false;
    }

    if (!tok_.InitDefault(error)) {
        return false;
    }

    if (!phrase_.InitFromFiles(lapos_model_f, phrase_config_ff, error)) {
        return false;
    }

    ready_ = true;
    return true;
}

bool Analyzer::Analyze(
        const vector<UChar>& original_text, const AnalysisOptions& options,
        AnalysisResult* result, string* error) const {
    if (!ready_) {
        *error = "[Analyzer] You forgot to call Init().";
        return false;
    }

    result->Clear();
    error->clear();

    result->original_text = original_text;

    if (!preproc_.Preprocess(
            result->original_text, options.destutter_max_consecutive,
            &result->clean_text, &result->clean2original, &result->chr2drop,
            error)) {
        return false;
    }

    vector<string> tokens;
    tok_.Tokenize(result->clean_text, options.replace_html_entities,
                  &result->tokens);

    vector<PhraseDetectionResult> phrase_results;
    if (!phrase_.Detect(result->tokens, &result->phrase_results, error)) {
        return false;
    }

    return true;
}
