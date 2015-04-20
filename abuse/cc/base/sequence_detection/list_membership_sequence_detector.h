#ifndef CC_BASE_SEQUENCE_DETECTION_LIST_MEMBERSHIP_MULTI_SEQUENCE_DETECTOR_H_
#define CC_BASE_SEQUENCE_DETECTION_LIST_MEMBERSHIP_MULTI_SEQUENCE_DETECTOR_H_

#include <vector>

#include "cc/base/sequence_detection/sequence_match.h"

using std::vector;

template <typename T>
class ListMembershipSequenceDetector {
  public:
    void Init(const vector<vector<vector<T>>>& option_lists);

    void Dump(void (*dump_t)(const T& t)) const;

    bool GetMatchLists(const vector<vector<T>>& lists,
                       vector<vector<SequenceMatch>>* matches);
};

#endif  // CC_BASE_SEQUENCE_DETECTION_LIST_MEMBERSHIP_MULTI_SEQUENCE_DETECTOR_H_
