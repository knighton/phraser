#ifndef CC_ANALYSIS_ANALYSIS_OPTIONS_H_
#define CC_ANALYSIS_ANALYSIS_OPTIONS_H_

#include <cstddef>

struct AnalysisOptions {
    AnalysisOptions() :
        track_index_translation(true),
        destutter_max_consecutive(3),
        track_chr2drop(true),
        replace_html_entities(true)
    {}

    // General flags:
    // * Map tokens to spans in the original text.
    bool track_index_translations;

    // Preprocessing flags:
    // * Maximum number of consecutive code points before we start dropping them.
    // * Whether to keep track of code point drop counts from destuttering.
    size_t destutter_max_consecutive;
    bool track_chr2drop;

    // Tokenization flags:
    // * Whether to replace HTML entities in the text with their Unicode
    //   equivalents.
    bool replace_html_entities;
};

#endif  // CC_ANALYSIS_ANALYSIS_OPTIONS_H_
