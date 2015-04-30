#ifndef CC_PHRASING_PHRASE_DETECTOR_H_
#define CC_PHRASING_PHRASE_DETECTOR_H_

#include <string>
#include <vector>

#include "cc/english/english_config.h"
#include "cc/english/english_expression_evaluator.h"
#include "cc/misc/json.h"
#include "cc/phrase_detection/phrase_config.h"
#include "cc/phrase_detection/phrase_detection_result.h"
#include "cc/phrase_detection/phrase_config_parser.h"
#include "cc/sequence_detection/vector_membership_sequence_detector.h"

using std::string;
using std::vector;

class PhraseDetector {
  public:
    bool InitFromFiles(const EnglishConfig& english_config,
                       const vector<string>& phrase_config_ff,
                       string* error);

    // Dump to JSON.
    json::Object* ToJSON() const;

    bool Detect(const vector<string>& tokens,
                vector<PhraseDetectionResult>* results, string* error) const;

  private:
    bool LoadPhraseConfig(const string& phrase_f, string* error);

    // Thing that parses PhraseConfigs.
    PhraseConfigParser phrase_parser_;

    // Expression/token vocabulary (shared by all phrases).
    EnglishExpressionEvaluator vocab_;

    // Info and sequence detector for each phrase config.
    vector<PhraseConfig> phrases_;
    vector<VectorMembershipSequenceDetector<TokenGroupID>> detectors_;
};

#endif  // CC_PHRASING_PHRASE_DETECTOR_H_
