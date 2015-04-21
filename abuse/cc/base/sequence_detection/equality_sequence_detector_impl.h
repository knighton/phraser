#ifndef CC_BASE_SEQUENCE_DETECTION_EQUALITY_SEQUENCE_DETECTOR_IMPL_H_
#define CC_BASE_SEQUENCE_DETECTION_EQUALITY_SEQUENCE_DETECTOR_IMPL_H_

template <typename Atom>
Atom* EqualityAtomTokenComparer<Atom>::FirstAtom(const Atom& token) {
    return &token;
}

template <typename Atom>
Atom* EqualityAtomTokenComparer<Atom>::LastAtom(const Atom& token) {
    return &token;
}

template <typename Atom>
bool EqualityAtomTokenComparer<Atom>::IsMatch(
        const Atom& token_have, const Atom& atom_need) {
    return token_have == atom_need;
}

#endif  // CC_BASE_SEQUENCE_DETECTION_EQUALITY_SEQUENCE_DETECTOR_IMPL_H_
