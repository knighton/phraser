#ifndef CC_BASE_SEQUENCE_DETECTION_LIST_MEMBERSHIP_SEQUENCE_DETECTOR_H_
#define CC_BASE_SEQUENCE_DETECTION_LIST_MEMBERSHIP_SEQUENCE_DETECTOR_H_

#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "cc/base/sequence_detection/sequence_match.h"

using std::unordered_map;
using std::unordered_set;
using std::vector;

template <typename Item>
struct BlockInfo {
    unordered_map<Item, vector<size_t>> firstitem2optionxx;
    bool can_be_empty;
};

template <typename Item>
class ListMembershipSequenceDetector {
  public:
    ListMembershipSequenceDetector();
    ~ListMembershipSequenceDetector();

    // Input: a list of blocks.  A block is a list of options.  Each option is a
    // list of Items.  Indexes the input, which is matched against by
    // GetMatches().
    //
    // Returns true on success.
    //
    // Reasons it will fail:
    // * You can't have identical options within the same option list.
    bool Init(const vector<vector<vector<Item>>>& blocks);

    // Dump my contents to stdout.
    void Dump(void (*DumpItem)(const Item& item)) const;

    // Get every possible combination of options and span that match the input.
    //
    // One option list must match from each block, in order, contiguously,
    // against the input for there to be a match.  An option is matched if each
    // token in the option is present in the corresponding index in the input.
    void GetMatches(const vector<vector<Item>>& lists,
                    vector<SequenceMatch>* matches) const;

  private:
    void GetPossibleOptions(
        size_t block_index, const vector<Item>& list,
        unordered_set<size_t>* option_indexes) const;

    bool IsMatch(const vector<Item>& have_list, const Item& need) const;

    void AppendMatchesInner(
        const vector<vector<Item>>& lists, size_t match_begin_index,
        size_t option_begin_index, vector<size_t>* option_choices_to_here,
        vector<SequenceMatch>* matches) const;

    vector<vector<vector<Item>>> blocks_;
    BlockInfo<Item>* block_infos_;
};

#include "list_membership_sequence_detector_impl.h"

#endif  // CC_BASE_SEQUENCE_DETECTION_LIST_MEMBERSHIP_SEQUENCE_DETECTOR_H_
