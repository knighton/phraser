#ifndef CC_SEQUENCE_DETECTION_VECTOR_MEMBERSHIP_SEQUENCE_DETECTOR_IMPL_H_
#define CC_SEQUENCE_DETECTION_VECTOR_MEMBERSHIP_SEQUENCE_DETECTOR_IMPL_H_

#include <cassert>

template <typename Atom>
size_t VectorMembershipAtomTokenComparer<Atom>::NumAtoms(
        const vector<Atom>& token) {
    return token.size();
}

template <typename Atom>
const Atom* VectorMembershipAtomTokenComparer<Atom>::FirstAtom(
        const vector<Atom>& token) {
    assert(token.size());
    return &(token[0]);
}

template <typename Atom>
const Atom* VectorMembershipAtomTokenComparer<Atom>::LastAtom(
        const vector<Atom>& token) {
    assert(token.size());
    return &(token[token.size() - 1]);
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

template <typename Atom>
json::Object* VectorMembershipAtomTokenComparer<Atom>::AtomToJSON(
        const Atom& atom) {
    return new json::Object(atom);
}

template <typename Atom>
json::Object* VectorMembershipAtomTokenComparer<Atom>::TokenToJSON(
        const vector<Atom>& token) {
    return new json::Object(token);
}

#endif  // CC_SEQUENCE_DETECTION_VECTOR_MEMBERSHIP_SEQUENCE_DETECTOR_IMPL_H_
