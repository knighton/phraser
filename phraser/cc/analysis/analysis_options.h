#ifndef CC_ANALYSIS_ANALYSIS_OPTIONS_H_
#define CC_ANALYSIS_ANALYSIS_OPTIONS_H_

#include <cstddef>

struct AnalysisOptions {
    AnalysisOptions() :
        destutter_max_consecutive(3),
        replace_html_entities(true)
    {}

    // Preprocessing.
    size_t destutter_max_consecutive;

    // Tokenization.
    bool replace_html_entities;
};

#endif  // CC_ANALYSIS_ANALYSIS_OPTIONS_H_
