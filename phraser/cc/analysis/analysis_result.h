#ifndef CC_ANALYSIS_ANALYSIS_RESULT_H_
#define CC_ANALYSIS_ANALYSIS_RESULT_H_

#include <string>
#include <unordered_map>
#include <vector>

#include "cc/misc/json.h"
#include "cc/misc/unicode.h"
#include "cc/phrase_detection/phrase_detection_result.h"

using std::string;
using std::unordered_map;
using std::vector;

#define TEXT_MAX_LEN (1ul << 16)

struct ExclusiveSpan {
    uint16_t begin;
    uint16_t end_excl;
};

struct AnalysisResult {
    void Clear();

    // Input:
    vector<UChar> original_text;

    // Preprocessing results:
    // * Text after preprocessing.
    // * Index in 'clean_text' -> index in 'original_text'.
    // * Code point -> number of times it was dropped by destuttering.
    vector<UChar> clean_text;
    vector<uint16_t> clean2original;
    unordered_map<UChar, size_t> chr2drop;

    // Tokenization results:
    // * The fully processed tokens.
    // * Token index -> span in 'clean_text' that the token is from.
    vector<string> tokens;
    vector<ExclusiveSpan> token2clean;

    // Phrase detection results:
    vector<PhraseDetectionResult> phrase_results;
};

#endif  // CC_ANALYSIS_ANALYSIS_RESULT_H_
