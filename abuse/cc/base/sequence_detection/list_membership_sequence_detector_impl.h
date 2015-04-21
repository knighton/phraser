#ifndef CC_BASE_SEQUENCE_DETECTION_LIST_MEMBERSHIP_SEQUENCE_DETECTOR_IMPL_H_
#define CC_BASE_SEQUENCE_DETECTION_LIST_MEMBERSHIP_SEQUENCE_DETECTOR_IMPL_H_

template <typename Atom>
Atom* ListMembershipAtomTokenComparer<Atom>::FirstAtom(
        const vector<Atom>& token) {
    return &tokens[0];
}

template <typename Atom>
Atom* ListMembershipAtomTokenComparer<Atom>::LastAtom(
        const vector<Atom>& token) {
    return tokens.size();
}

template <typename Atom>
bool ListMembershipAtomTokenComparer<Atom>::IsMatch(
        const vector<Atom>& token_have, const Atom& atom_need) {
    if (auto& atom_have : token_have) {
        if (atom_have == atom_need) {
            return true;
        }
    }
    return false;
}

#endif  // CC_BASE_SEQUENCE_DETECTION_LIST_MEMBERSHIP_SEQUENCE_DETECTOR_IMPL_H_
