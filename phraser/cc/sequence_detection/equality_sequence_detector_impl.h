#ifndef CC_SEQUENCE_DETECTION_EQUALITY_SEQUENCE_DETECTOR_IMPL_H_
#define CC_SEQUENCE_DETECTION_EQUALITY_SEQUENCE_DETECTOR_IMPL_H_

template <typename Atom>
size_t EqualityAtomTokenComparer<Atom>::NumAtoms(const Atom& atom) {
    return 1ul;
}

template <typename Atom>
const Atom* EqualityAtomTokenComparer<Atom>::FirstAtom(const Atom& token) {
    return &token;
}

template <typename Atom>
const Atom* EqualityAtomTokenComparer<Atom>::LastAtom(const Atom& token) {
    return &token;
}

template <typename Atom>
bool EqualityAtomTokenComparer<Atom>::IsMatch(
        const Atom& token_have, const Atom& atom_need) {
    return token_have == atom_need;
}

template <typename Atom>
json::Object* VectorMembershipAtomTokenComparer<Atom>::AtomToJSON(
        const Atom& atom) {
    return new json::Object(atom);
}

template <typename Atom>
json::Object* VectorMembershipAtomTokenComparer<Atom>::TokenToJSON(
        const Atom& token) {
    return new json::Object(token);
}

#endif  // CC_SEQUENCE_DETECTION_EQUALITY_SEQUENCE_DETECTOR_IMPL_H_
