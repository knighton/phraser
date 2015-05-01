#ifndef CC_ANALYSIS_ANALYSIS_OPTIONS_H_
#define CC_ANALYSIS_ANALYSIS_OPTIONS_H_

#include <cstddef>

struct AnalysisOptions {
    AnalysisOptions() :
        destutter_max_consecutive(3),
        replace_html_entities(true)
    {}

    // Preprocessing flags:
    // * Maximum number of consecutive code points before we start dropping them.
    size_t destutter_max_consecutive;

    // Tokenization flags:
    // * Whether to replace HTML entities in the text with their Unicode
    //   equivalents.
    bool replace_html_entities;
};

#endif  // CC_ANALYSIS_ANALYSIS_OPTIONS_H_
