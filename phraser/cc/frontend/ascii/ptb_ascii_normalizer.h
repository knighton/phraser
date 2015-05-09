#ifndef CC_FRONTEND_ASCII_PTB_ASCII_NORMALIZER_H_
#define CC_FRONTEND_ASCII_PTB_ASCII_NORMALIZER_H_

#include <string>
#include <unordered_map>
#include <vector>

#include "cc/base/unicode.h"
#include "cc/frontend/base/chr_index.h"

using std::string;
using std::unordered_map;
using std::vector;

class PtbAsciiNormalizer {
  public:
    // Load precomputed lookup tables.
    bool Init(string* error);

    // Unicode -> Penn Treeank ASCII.
    void Normalize(
        const ustring& in, string* out, vector<ChrIndex>* out2in) const;

  private:
    string ascii_data_;
    unordered_map<uchar, uint16_t> unicode2ascii_;
};

#endif  // CC_FRONTEND_ASCII_PTB_ASCII_NORMALIZER_H_
