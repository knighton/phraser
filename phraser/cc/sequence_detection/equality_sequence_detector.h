#ifndef CC_SEQUENCES_EQUALITY_SEQUENCE_DETECTOR_H_
#define CC_SEQUENCES_EQUALITY_SEQUENCE_DETECTOR_H_

#include "cc/misc/json.h"
#include "cc/sequence_detection/sequence_detector.h"

template <typename Atom>
class EqualityAtomTokenComparer {
  public:
    static size_t NumAtoms();
    static const Atom* FirstAtom(const Atom& token);
    static const Atom* LastAtom(const Atom& token);
    static bool IsMatch(const Atom& token_have, const Atom& atom_need);
    static json::Object* AtomToJSON(const Atom& atom);
    static json::Object* TokenToJSON(const Atom& token);
};

template <typename Atom>
using EqualitySequenceDetector =
    SequenceDetector<Atom, Atom, EqualityAtomTokenComparer<Atom>>

#include "equality_sequence_detector_impl.h"

#endif  // CC_SEQUENCES_EQUALITY_SEQUENCE_DETECTOR_H_
