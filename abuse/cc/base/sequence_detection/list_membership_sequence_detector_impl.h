#ifdef CC_BASE_SEQUENCE_DETECTION_LIST_MEMBERSHIP_SEQUENCE_DETECTOR_IMPL_H_
#define CC_BASE_SEQUENCE_DETECTION_LIST_MEMBERSHIP_SEQUENCE_DETECTOR_IMPL_H_

#define LMSD ListMembershipSequenceDetector

template <typename T>
void LMSD::Init(const vector<vector<vector<T>>>& blocks) {
}

template <typename T>
void LMSD::Dump(void (*dump_t)(const T& t)) const {
}

template <typename T>
bool LMSD::GetMatchLists(const vector<vector<T>>& lists,
                         vector<vector<SequenceMatch>>* matches) {
}

#endif  // CC_BASE_SEQUENCE_DETECTION_LIST_MEMBERSHIP_SEQUENCE_DETECTOR_IMPL_H_
