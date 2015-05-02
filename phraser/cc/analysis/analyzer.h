#ifndef CC_MAIN_ANALYZER_H_
#define CC_MAIN_ANALYZER_H_

#include "cc/analysis/analysis_options.h"
#include "cc/analysis/analysis_result.h"
#include "cc/misc/unicode.h"
#include "cc/phrase_detection/phrase_detector.h"
#include "cc/preprocess/preprocessor.h"
#include "cc/tokenization/tokenizer.h"

class Analyzer {
  public:
    Analyzer() : ready_(false) {}

    // Returns false on fatal error.
    bool Init(const string& lapos_model_f,
              const vector<string>& phrase_config_ff, string* error);

    // Returns false if the analysis failed.
    bool Analyze(const ustring& text, const AnalysisOptions& options,
                 AnalysisResult* result, string* error) const;

  private:
    bool ready_;
    Preprocessor preproc_;
    Tokenizer tok_;
    PhraseDetector phrase_;
};

#endif  // CC_MAIN_ANALYZER_H_
