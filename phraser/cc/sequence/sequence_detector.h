#ifndef CC_SEQUENCES_SEQUENCE_DETECTOR_H_
#define CC_SEQUENCES_SEQUENCE_DETECTOR_H_

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "cc/sequence/sequence_match.h"

using std::string;
using std::unordered_map;
using std::unordered_set;
using std::vector;

// Info about a block (list of options) for efficient lookup.
template <typename Atom>
struct BlockInfo {
    unordered_map<Atom, vector<size_t>> firstatom2optionxx;
    bool can_be_empty;
};

// Finds contiguous sequences of lists in the input list.
//
// Atoms:
// * An Atom is the smallest piece of sequence.
// * Matches look like "[atom, atom], [], [atom, atom, atom, atom], [atom]".
// * GetMatches() does lookups against Atoms.
//
// Tokens:
// * Tokens are what the user passes into GetMatches().
// * Tokens are either
//   * Atoms themselves (match means equality), or
//   * lists of Atoms (match means list membership).
//
// Example (where Token = Atom):
//
//     [a]      []     [cat]     [lounged around]
//     [the]    [fat]  [dog]     [snored]
//     [some] x        [horse] x [dozed off]
//     [my]                      [ate]
//     [your]                    [slept]
//
//     "I told you my lazy fat cat slept all day"
//         -> []
//
//     "I told you my fat cat slept all day and your dog at my homework"
//         -> [ SequenceMatch( 2,  7, [3, 1, 0, 4])
//              SequenceMatch(10, 13, [4, 0, 1, 3]) ]
template <typename Atom, typename Token, typename AtomTokenComparer>
class SequenceDetector {
  public:
    SequenceDetector();
    SequenceDetector(const SequenceDetector& other);
    SequenceDetector<Atom, Token, AtomTokenComparer>&
        operator=(const SequenceDetector& other);
    ~SequenceDetector();

    // Constructs an index given some blocks.
    //
    // Returns true on success.
    //
    // Reasons it will fail:
    // * You can't have identical options within the same option list.
    bool Init(const vector<vector<vector<Atom>>>& blocks, string* error);

    // Dump my contents to stdout.
    void Dump(void (*DumpAtom)(const Atom& atom)) const;

    // Get every possible match.
    //
    // One option list must match from each block, in order, contiguously,
    // against the input for there to be a match.  An option is matched if each
    // atom in the option matches the token at the corresponding index in the
    // input.
    void GetMatches(const vector<Token>& tokens,
                    vector<SequenceMatch>* matches) const;

  private:
    void GetPossibleOptions(
        size_t block_index, const Token& token,
        unordered_set<size_t>* option_indexes) const;

    void AppendMatchesInner(
        const vector<Token>& tokens, size_t match_begin_index,
        size_t option_begin_index, vector<size_t>* option_choices_to_here,
        vector<SequenceMatch>* matches) const;

    // List of option lists to match against.
    // * Each block is a list of options.
    // * Each option is a list of Atoms.
    // * One option must match from each block (in order, contiguously) for
    //   there to be a match.
    vector<vector<vector<Atom>>> blocks_;

    // Some information per block for efficient lookup.
    BlockInfo<Atom>* block_infos_;
};

#include "sequence_detector_impl.h"

#endif  // CC_BASE_SEQUENCE_DETECTION_LIST_MEMBERSHIP_SEQUENCE_DETECTOR_H_
