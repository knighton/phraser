#include "reduction_data.h"

unordered_map<string, vector<vector<string>>> REDUCED2NORMAL = {
    {"'tis", {{"it", "", "is", ""}}}
    {"'twas", {{"it", "", "was", ""}}}
    {"aight", {{"alright", ""}}}
    {"alot", {{"a", "", "lot", ""}}}
    {"aright", {{"alright", ""}}}
    {"bout", {{"about", ""}}}
    {"cannot", {{"can", "", "not", ""}}}
    {"coulda", {{"could", "", "have", ""}}}
    {"d'ya", {{"do", "", "you", ""}}}
    {"d'ye", {{"do", "", "you", ""}}}
    {"didja", {{"did", "", "you", ""}}}
    {"doncha", {{"do", "", "n't", "", "you", ""}}}
    {"donchaknow", {{"do", "", "n't", "", "you", "", "know", ""}}}
    {"getcha", {{"get", "", "you", "PRP"}, {"get", "", "your", "PRP$"}}}
    {"gimme", {{"give", "", "me", ""}}}
    {"gonna", {{"going", "", "to", ""}}}
    {"gotta", {{"got", "", "to", ""}}}
    {"hafta", {{"have", "", "to", ""}}}
    {"i'ma", {{"i", "", "'m", "", "going", "", "to", ""}}}
    {"i'mma", {{"i", "", "'m", "", "going", "", "to", ""}}}
    {"ima", {{"i", "", "'m", "", "going", "", "to", ""}}}
    {"imma", {{"i", "", "'m", "", "going", "", "to", ""}}}
    {"innit", {{"is", "", "n't", "", "it", ""}}}
    {"kinda", {{"kind", "", "of", ""}}}
    {"lemme", {{"let", "", "me", ""}}}
    {"lotta", {{"lot", "", "of", ""}}}
    {"more'n", {{"more", "", "than", ""}}}
    {"musta", {{"must", "", "have", ""}}}
    {"oughta", {{"ought", "", "to", ""}}}
    {"outta", {{"out", "", "of", ""}}}
    {"probly", {{"probably", ""}}}
    {"prolly", {{"probably", ""}}}
    {"shoulda", {{"should", "", "have", ""}}}
    {"sorta", {{"sort", "", "of", ""}}}
    {"sup", {{"what", "", "is", "", "up", ""}}}
    {"trynna", {{"trying", "", "to", ""}}}
    {"wanna", {{"want", "", "a", ""}}}
    {"wanna", {{"want", "", "to", ""}}}
    {"wassup", {{"what", "", "is", "", "up", ""}}}
    {"whaddaya", {{"what", "", "do", "", "you", ""}, {"what", "", "are", "", "you", ""}}}
    {"whassup", {{"what", "", "is", "", "up", ""}}}
    {"whatcha", {{"what", "", "are", "", "you", ""}, {"what", "", "have", "", "you", ""}}}
    {"woulda", {{"would", "", "have", ""}}}
    {"wouldja", {{"would", "", "you", ""}}}
};

