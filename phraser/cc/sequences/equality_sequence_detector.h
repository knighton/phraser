#ifndef CC_BASE_SEQUENCE_DETECTION_EQUALITY_SEQUENCE_DETECTOR_H_
#define CC_BASE_SEQUENCE_DETECTION_EQUALITY_SEQUENCE_DETECTOR_H_

#include <vector>

#include "cc/base/sequence_detection/sequence_detector.h"

using std::vector; 

template <typename Atom>
class EqualityAtomTokenComparer {
  public:
    static Atom* FirstAtom(const vector<Atom>& token);
    static Atom* LastAtom(const vector<Atom>& token);
    static bool IsMatch(const vector<Atom>& token_have, const Atom& atom_need);
};

template <typename Atom>
using EqualitySequenceDetector =
    SequenceDetector<Atom, vector<Atom>, EqualityAtomTokenComparer<Atom>>

#include "equality_sequence_detector_impl.h"

#endif  // CC_BASE_SEQUENCE_DETECTION_EQUALITY_SEQUENCE_DETECTOR_H_
