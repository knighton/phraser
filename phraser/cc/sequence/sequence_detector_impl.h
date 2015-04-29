#ifndef CC_SEQUENCES_SEQUENCE_DETECTOR_IMPL_H_
#define CC_SEQUENCES_SEQUENCE_DETECTOR_IMPL_H_

#include <algorithm>
#include <cstdio>

template <typename Atom, typename Token, typename AtomTokenComparer>
SequenceDetector<Atom, Token, AtomTokenComparer>::SequenceDetector() :
    block_infos_(NULL) {
}

template <typename Atom, typename Token, typename AtomTokenComparer>
SequenceDetector<Atom, Token, AtomTokenComparer>::SequenceDetector(
        const SequenceDetector<Atom, Token, AtomTokenComparer>& other) {
    blocks_ = other.blocks_;

    if (other.block_infos_) {
        block_infos_ = new BlockInfo<Atom>[other.blocks_.size()];
        for (auto i = 0u; i < other.blocks_.size(); ++i) {
            block_infos_[i] = other.block_infos_[i];
        }
    }
}

template <typename Atom, typename Token, typename AtomTokenComparer>
SequenceDetector<Atom, Token, AtomTokenComparer>&
    SequenceDetector<Atom, Token, AtomTokenComparer>::operator=(
        const SequenceDetector<Atom, Token, AtomTokenComparer>& other) {
    blocks_ = other.blocks_;

    if (other.block_infos_) {
        block_infos_ = new BlockInfo<Atom>[other.blocks_.size()];
        for (auto i = 0u; i < other.blocks_.size(); ++i) {
            block_infos_[i] = other.block_infos_[i];
        }
    }
}

template <typename Atom, typename Token, typename AtomTokenComparer>
SequenceDetector<Atom, Token, AtomTokenComparer>::~SequenceDetector() {
    if (block_infos_) {
        delete [] block_infos_;
        block_infos_ = NULL;
    }
}

template <typename Atom, typename Token, typename AtomTokenComparer>
bool SequenceDetector<Atom, Token, AtomTokenComparer>::Init(
        const vector<vector<vector<Atom>>>& blocks, string* error) {
    blocks_ = blocks;

    // For each list of options,
    block_infos_ = new BlockInfo<Atom>[blocks_.size()];
    for (auto i = 0u; i < blocks_.size(); ++i) {
        auto& options = blocks_[i];

        // Sort options for locality during lookup.
        sort(options.begin(), options.end());

        // Verify that there are no duplicate options.
        for (auto j = 1u; j < options.size(); ++j) {
            if (options[j - 1] == options[j]) {
                *error = "[SequenceDetector] Duplicate options detected during "
                         "Init.";
                return false;
            }
        }

        // Index the option for lookup.
        auto& bi = block_infos_[i];
        bi.can_be_empty = false;
        for (auto j = 0u; j < options.size(); ++j) {
            auto& atoms = options[j];
            if (atoms.size()) {
                bi.firstatom2optionxx[atoms[0]].emplace_back(j);
            } else {
                bi.can_be_empty = true;
            }
        }
    }

    return true;
}

template <typename Atom, typename Token, typename AtomTokenComparer>
json::Object* SequenceDetector<Atom, Token, AtomTokenComparer>::ToJSON() const {
    vector<json::Object*> block_objects;
    block_objects.reserve(blocks_.size());
    for (auto i = 0u; i < blocks_.size(); ++i) {
        auto& options = blocks_[i];
        vector<json::Object*> option_objects;
        option_objects.reserve(options.size());
        for (auto j = 0u; j < options.size(); ++j) {
            auto& atoms = options[j];
            vector<json::Object*> atom_objects;
            atom_objects.reserve(atoms.size());
            for (auto k = 0u; k < atoms.size(); ++k) {
                auto& atom = atoms[k];
                auto obj = AtomTokenComparer::AtomToJSON(atom);
                atom_objects.emplace_back(obj);
            }
            option_objects.emplace_back(new json::Object(atom_objects));
        }
        block_objects.emplace_back(new json::Object(option_objects));
    }

    return new json::Object({
        {"blocks", new json::Object(block_objects)},
    });
}

template <typename Atom, typename Token, typename AtomTokenComparer>
void SequenceDetector<Atom, Token, AtomTokenComparer>::GetPossibleOptions(
        size_t block_index, const Token& token,
        unordered_set<size_t>* option_indexes) const {
    option_indexes->clear();

    if (!AtomTokenComparer::NumAtoms(token)) {
        return;
    }

    const Atom* a = AtomTokenComparer::FirstAtom(token);
    const Atom* z_encl = AtomTokenComparer::LastAtom(token);
    for (const Atom* atom = a; atom <= z_encl; ++atom) {
        auto& first2optionxx = block_infos_[block_index].firstatom2optionxx;
        auto it = first2optionxx.find(*atom);
        if (it == first2optionxx.end()) {
            continue;
        }
        for (auto& option_index : it->second) {
            option_indexes->insert(option_index);
        }
    }
}

template <typename Atom, typename Token, typename AtomTokenComparer>
void SequenceDetector<Atom, Token, AtomTokenComparer>::AppendMatchesInner(
        const vector<Token>& tokens, size_t match_begin_index,
        size_t option_begin_index, vector<size_t>* option_choices_to_here,
        vector<SequenceMatch>* matches) const {
    // Bail if we're out of input but still have sequences to match.
    if (!(option_begin_index < tokens.size())) {
        return;
    }

    // Get the sequence option we are going to try to match against.
    size_t cur_block_index = option_choices_to_here->size() - 1;
    size_t option_index = option_choices_to_here->at(cur_block_index);
    const vector<Atom>& atoms_to_match = blocks_[cur_block_index][option_index];

    // Check each atom in the sequence.
    //
    // The caller has already checked index 0 (that is how control flow got\
    // here).
    for (auto i = 1u; i < atoms_to_match.size(); ++i) {
        auto& atom_need = atoms_to_match[i];

        // Make sure we haven't run out of input tokens.
        size_t input_index = option_begin_index + i;
        if (input_index == tokens.size()) {
            return;
        }

        // Check for membership.
        if (!AtomTokenComparer::IsMatch(
                tokens[input_index], atoms_to_match[i])) {
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
        size_t z_excl = option_begin_index + atoms_to_match.size();
        m->Init(a, z_excl, *option_choices_to_here);
        return;
    }

    // Else, if we're out of input, the match was a failure.
    option_begin_index += atoms_to_match.size();
    if (option_begin_index == tokens.size()) {
        return;
    }

    // We have atoms and sequence options to match left, so try to match the
    // beginning of the next sequence.
    auto& token = tokens[option_begin_index];
    unordered_set<size_t> option_indexes;
    GetPossibleOptions(next_block_index, token, &option_indexes);
    for (auto choice : option_indexes) {
        option_choices_to_here->emplace_back(choice);
        AppendMatchesInner(tokens, match_begin_index, option_begin_index,
                           option_choices_to_here, matches);
        option_choices_to_here->pop_back();
    }
}

template <typename Atom, typename Token, typename AtomTokenComparer>
void SequenceDetector<Atom, Token, AtomTokenComparer>::GetMatches(
        const vector<Token>& tokens, vector<SequenceMatch>* matches) const {
    matches->clear();

    // For each possible starting index,
    unordered_map<size_t, vector<SequenceMatch>> begin2matches;
    for (auto i = 0u; i < tokens.size(); ++i) {
        auto& token = tokens[i];

        // Get all subsequence options that start with an atom matching our
        // token.
        size_t block_index = 0;
        unordered_set<size_t> option_indexes;
        GetPossibleOptions(block_index, token, &option_indexes);

        // Get all matches that start at this index.
        vector<size_t> option_choices_to_here;
        option_choices_to_here.reserve(blocks_.size());
        option_choices_to_here.resize(1);
        for (auto option_index : option_indexes) {
            option_choices_to_here[0] = option_index;
            AppendMatchesInner(tokens, i, i, &option_choices_to_here, matches);
        }
    }
}

#endif  // CC_SEQUENCES_SEQUENCE_DETECTOR_IMPL_H_
