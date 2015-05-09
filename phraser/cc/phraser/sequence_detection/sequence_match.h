#ifndef CC_PHRASER_SEQUENCE_DETECTION_SEQUENCE_MATCH_H_
#define CC_PHRASER_SEQUENCE_DETECTION_SEQUENCE_MATCH_H_

#include <cstddef>
#include <vector>

using std::vector;

class SequenceMatch {
  public:
    void Init(size_t begin, size_t end_excl,
              const vector<size_t>& option_choices);

    size_t begin() const { return begin_; }
    size_t end_excl() const { return end_excl_; }
    const vector<size_t>& option_choices() const { return option_choices_; }

  private:
    size_t begin_;
    size_t end_excl_;
    vector<size_t> option_choices_;
};

#endif  // CC_PHRASER_SEQUENCE_DETECTION_SEQUENCE_MATCH_H_
