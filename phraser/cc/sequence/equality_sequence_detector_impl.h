#ifndef CC_SEQUENCES_EQUALITY_SEQUENCE_DETECTOR_IMPL_H_
#define CC_SEQUENCES_EQUALITY_SEQUENCE_DETECTOR_IMPL_H_

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

#endif  // CC_SEQUENCES_EQUALITY_SEQUENCE_DETECTOR_IMPL_H_
