#ifndef CC_BASE_SEQUENCE_DETECTION_VECTOR_MEMBERSHIP_SEQUENCE_DETECTOR_IMPL_H_
#define CC_BASE_SEQUENCE_DETECTION_VECTOR_MEMBERSHIP_SEQUENCE_DETECTOR_IMPL_H_

template <typename Atom>
Atom* VectorMembershipAtomTokenComparer<Atom>::FirstAtom(
        const vector<Atom>& token) {
    return &token[0];
}

template <typename Atom>
Atom* VectorMembershipAtomTokenComparer<Atom>::LastAtom(
        const vector<Atom>& token) {
    return &token[token.size() - 1];
}

template <typename Atom>
bool VectorMembershipAtomTokenComparer<Atom>::IsMatch(
        const vector<Atom>& token_have, const Atom& atom_need) {
    for (auto i = 0u; i < token_have.size(); ++i) {
        if (token_have[i] == atom_need) {
            return true;
        }
    }
    return false;
}

#endif  // CC_BASE_SEQUENCE_DETECTION_VECTOR_MEMBERSHIP_SEQUENCE_DETECTOR_IMPL_H_
