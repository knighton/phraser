#ifndef CC_ANALYSIS_ANALYSIS_RESULT_H_
#define CC_ANALYSIS_ANALYSIS_RESULT_H_

#include <string>
#include <vector>

#include "cc/misc/unicode.h"
#include "cc/phrase_detection/phrase_detection_result.h"

using std::string;
using std::vector;

struct AnalysisResult {
    void Clear();

    vector<UChar> original_text;
    vector<UChar> clean_text;
    vector<string> tokens;
    vector<PhraseDetectionResult> phrase_results;
};

#endif  // CC_ANALYSIS_ANALYSIS_RESULT_H_
