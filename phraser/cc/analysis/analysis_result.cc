#include "analysis_result.h"

void AnalysisResult::Clear() {
    original_text.clear();

    clean_text.clear();
    clean2original.clear();
    chr2drop.clear();

    tokens.clear();
    token2clean.clear();

    phrase_results.clear();
};
