#ifndef CC_FRONTEND_BASE_TOKEN_H_
#define CC_FRONTEND_BASE_TOKEN_H_

#include <string>
#include <vector>

using std::string;
using std::vector;

struct Token {
    // A special token indicating ' or 's.
    static string POS;

    // The possible words this token means.  Usually just one.  In the case of
    // contractions, it may be two ('d -> did, would).
    vector<string> possible_words;

    // Penn POS tag.
    string tag;
};

#endif  // CC_FRONTEND_BASE_TOKEN_H_
