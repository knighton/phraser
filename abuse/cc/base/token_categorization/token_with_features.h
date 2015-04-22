#ifndef CC_BASE_TOKEN_CATEGORIZATION_TOKEN_WITH_FEATURES_H_
#define CC_BASE_TOKEN_CATEGORIZATION_TOKEN_WITH_FEATURES_H_

#include <string>
#include <unordered_map>

using std::string;
using std::unordered_map;

struct TokenWithFeatures {
    string token;
    unordered_map<string, string> dimension2value;
};

#endif  // CC_BASE_TOKEN_CATEGORIZATION_TOKEN_WITH_FEATURES_H_
