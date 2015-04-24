#include <cassert>
#include <cstdio>
#include <unordered_map>

#include "cc/english/english_config.h"
#include "cc/phrase/phrase_detector.h"
#include "cc/sequence/vector_membership_sequence_detector.h"
#include "cc/tagging/lapos/lapos_tagger.h"

static void RunTagger() {
    vector<string> sen = {"I", "am", "a", "cat", ",", "and", "that", "is",
                          "that", "."};
    vector<string> tags;
    LaposTagger tagger;
    assert(tagger.Init("phraser/cc/third_party/lapos/model_wsj02-21/model.la"));
    string error;
    assert(tagger.Tag(sen, &tags, &error));
    assert(tags.size() == sen.size());
    for (auto i = 0; i < tags.size(); ++i) {
        printf("%s/%s ", sen[i].c_str(), tags[i].c_str());
    }
    printf("\n");
}

#define RED "\033[1;31m"
#define RESET "\033[0m"

static void OutputError(const string& error) {
    printf(RED "%s\n" RESET, error.c_str());
}

#undef RESET
#undef RED

int main() {
    EnglishConfig en_config;
    en_config.lapos_model_f = "phraser/cc/third_party/lapos/model_wsj02-21/model.la";

    PhraseDetector pd;
    vector<string> phrase_config_ff = {
        "phraser/config/simple.txt",
    };

    string error;
    if (!pd.InitFromFiles(en_config, phrase_config_ff, &error)) {
        OutputError(error);
        return 1;
    }

    vector<string> tokens = {"i", "will", "kill", "you"};
    vector<PhraseDetectionResult> results;
    if (!pd.Detect(tokens, &results, &error)) {
        OutputError(error);
        return 2;
    }
}
