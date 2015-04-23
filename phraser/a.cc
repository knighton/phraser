#include <cassert>
#include <cstdio>
#include <unordered_map>

#include "cc/sequence/vector_membership_sequence_detector.h"

#include "cc/tagging/lapos/lapos_tagger.h"

int main() {
    VectorMembershipSequenceDetector<int32_t> sd;
    (void)(sd);

    vector<string> sen = {"I", "am", "a", "cat", ",", "and", "that", "is",
                          "that", "."};
    vector<string> tags;
    LaposTagger tagger;
    assert(tagger.Init("abuse/cc/third_party/lapos/model_wsj02-21/model.la"));
    assert(tagger.Tag(sen, &tags));
    assert(tags.size() == sen.size());
    for (auto i = 0; i < tags.size(); ++i) {
        printf("%s/%s ", sen[i].c_str(), tags[i].c_str());
    }
    printf("\n");
}
