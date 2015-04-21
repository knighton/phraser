#ifndef CC_BASE_SEQUENCE_DETECTION_LIST_MEMBERSHIP_SEQUENCE_DETECTOR_IMPL_H_
#define CC_BASE_SEQUENCE_DETECTION_LIST_MEMBERSHIP_SEQUENCE_DETECTOR_IMPL_H_

#include <algorithm>
#include <cstdio>

#define LMSD ListMembershipSequenceDetector

using std::unordered_set;

template <typename Item>
LMSD<Item>::LMSD() : block_infos_(NULL) {
}

template <typename Item>
LMSD<Item>::~LMSD() {
    if (block_infos_) {
        delete block_infos_;
        block_infos_ = NULL;
    }
}

template <typename Item>
bool LMSD<Item>::Init(const vector<vector<vector<Item>>>& blocks) {
    blocks_ = blocks;

    // For each list of options,
    block_infos_ = new BlockInfo<Item>[blocks_.size()];
    for (auto i = 0u; i < blocks_.size(); ++i) {
        auto& options = blocks_[i];

        // Sort options for locality during lookup.
        sort(options.begin(), options.end());

        // Verify that there are no duplicate options.
        for (auto j = 1u; j < options.size(); ++j) {
            if (options[j - 1] == options[j]) {
                return false;
            }
        }

        // Index the option for lookup.
        auto& bi = block_infos_[i];
        bi.can_be_empty = false;
        for (auto j = 0u; j < options.size(); ++j) {
            auto& items = options[j];
            if (items.size()) {
                bi.firstitem2optionxx[items[0]].emplace_back(j);
            } else {
                bi.can_be_empty = true;
            }
        }
    }

    return true;
}

template <typename Item>
void LMSD<Item>::Dump(void (*DumpItem)(const Item& item)) const {
    printf("ListMembershipSequenceDetector {\n");
    for (auto i = 0u; i < blocks_.size(); ++i) {
        auto& options = blocks_[i];
        printf("\tOption list %d: {\n", i);
        for (auto j = 0u; i < options.size(); ++j) {
            auto& items = options[j];
            printf("\t\tOption %d:", j);
            for (auto& item : items) {
                puts(" ");
                DumpItem(item);
            }
            puts("\n");
        }
        printf("\t}\n");
    }
    printf("}\n");
}

template <typename Item>
void LMSD<Item>::GetPossibleOptions(
        size_t block_index, const vector<Item>& list,
        unordered_set<size_t>* option_indexes) const {
    option_indexes->clear();
    for (auto& item : list) {
        auto& first2optionxx = block_infos_[block_index].firstitem2optionxx;
        auto& it = first2optionxx.find(item);
        if (it == first2optionxx.end()) {
            continue;
        }
        for (auto option_index : *it) {
            option_indexes->insert(option_index);
        }
    }
}

template <typename Item>
bool LMSD<Item>::IsMatch(const vector<Item>& have_list,
                         const Item& need) const {
    for (auto& have : have_list) {
        if (have == need) {
            return true;
        }
    }
    return false;
}

template <typename Item>
void LMSD<Item>::AppendMatchesInner(
        const vector<vector<Item>>& lists, size_t match_begin_index,
        size_t option_begin_index, vector<size_t>* option_choices_to_here,
        vector<SequenceMatch>* matches) const {
    // Bail if we're out of input but still have sequences to match.
    if (!(option_begin_index < lists.size())) {
        return;
    }

    // Get the sequence option we are going to try to match against.
    size_t cur_block_index = option_choices_to_here->size() - 1;
    size_t option_index = option_choices_to_here->at(cur_block_index);
    const vector<Item>& items_to_match = blocks_[cur_block_index][option_index];

    // Check each item in the sequence.
    //
    // The caller has already checked index 0 (that is how control flow got\
    // here).
    for (auto i = 1u; i < items_to_match.size(); ++i) {
        auto& item_need = items_to_match[i];

        // Make sure we haven't run out of input lists.
        size_t input_index = option_begin_index + i;
        if (input_index == lists.size()) {
            return;
        }

        // Check for membership.
        if (!IsMatch(lists[input_index], items_to_match[i])) {
            return;
        }
    }

    // We successfully matched the option.  If that was the last block, we're
    // done, so append the result and return.
    size_t next_block_index = cur_block_index + 1;
    if (next_block_index == blocks_.size()) {
        matches->resize(matches->size() + 1);
        SequenceMatch* m = &(*matches)[matches->size() - 1];
        size_t a = match_begin_index;
        size_t z_excl = option_begin_index + items_to_match.size();
        m->Init(a, z_excl, *option_choices_to_here);
        return;
    }

    // Else, if we're out of input, the match was a failure.
    option_begin_index += items_to_match.size();
    if (option_begin_index == lists.size()) {
        return;
    }

    // We have items and sequence options to match left, so try to match the
    // beginning of the next sequence.
    auto& list = lists[option_begin_index];
    unordered_set<size_t> option_indexes;
    GetPossibleOptions(next_block_index, list, &option_indexes);
    for (auto choice : option_indexes) {
        option_choices_to_here->emplace_back(choice);
        AppendMatchesInner(lists, match_begin_index, option_begin_index,
                           option_choices_to_here, matches);
        option_choices_to_here->pop_back();
    }
}

template <typename Item>
void LMSD<Item>::GetMatches(const vector<vector<Item>>& lists,
                            vector<SequenceMatch>* matches) const {
    matches->clear();

    // For each possible starting index,
    unordered_map<size_t, vector<SequenceMatch>> begin2matches;
    for (auto i = 0u; i < lists.size(); ++i) {
        auto& list = lists[i];

        // Get all subsequence options that start with an item in our list.
        size_t block_index = 0;
        unordered_set<size_t> option_indexes;
        GetPossibleOptions(block_index, list, &option_indexes);

        // Get all matches that start at this index.
        vector<size_t> option_choices_to_here;
        option_choices_to_here.reserve(blocks_.size());
        option_choices_to_here.resize(1);
        for (auto option_index : option_indexes) {
            option_choices_to_here[0] = option_index;
            AppendMatchesInner(lists, i, i, &option_choices_to_here, matches);
        }
    }
}

#endif  // CC_BASE_SEQUENCE_DETECTION_LIST_MEMBERSHIP_SEQUENCE_DETECTOR_IMPL_H_
