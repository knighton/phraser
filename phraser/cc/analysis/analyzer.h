#ifndef CC_ANALYSIS_ANALYZER_H_
#define CC_ANALYSIS_ANALYZER_H_

#include "cc/analysis/analysis_options.h"
#include "cc/analysis/analysis_result.h"
#include "cc/base/unicode/unicode.h"
#include "cc/phrase_detection/phrase_detector.h"
#include "cc/preprocess/preprocessor.h"
#include "cc/tokenization/tokenizer.h"

class Analyzer {
  public:
    Analyzer() : ready_(false) {}

    // Returns false on fatal error.
    bool Init(const vector<string>& phrase_config_texts, string* error);

    // Returns false if the analysis failed.
    bool Analyze(const ustring& text, const AnalysisOptions& options,
                 AnalysisResult* result, string* error) const;

  private:
    bool ready_;
    Preprocessor preproc_;
    Tokenizer tok_;
    PhraseDetector phrase_;
};

#endif  // CC_ANALYSIS_ANALYZER_H_
