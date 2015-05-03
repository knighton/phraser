#ifndef CC_TOKENIZATION_TOKENIZER_H_
#define CC_TOKENIZATION_TOKENIZER_H_

#include <string>
#include <unordered_map>
#include <vector>

#include "cc/misc/unicode.h"
#include "cc/tokenization/lapos_ascii_tokenizer.h"
#include "cc/tokenization/span.h"

using std::string;
using std::unordered_map;
using std::vector;

// Tokenizer.
//
// Unicode -> PTB ASCII -> PTB tokens -> normalized PTB tokens.
//
// It's essentially pre/post-processing around an ASCII tokenizer that does the
// actual work.  Our custom mappings (eg, Unicode -> PTB ASCII) are precomputed
// for performance.
class Tokenizer {
  public:
    // Init with configuration.
    bool Init(const string& ascii_data,
              const unordered_map<uchar, uint16_t>& unicode2ascii,
              const unordered_map<string, string>& token2token, string* error);

    // Initialize from definitions in tokenizer_data.h.
    bool InitDefault(string* error);

    // Text -> tokens.
    //
    // Normalize the text to Penn Treebank ASCII, tokenize it, then postprocess
    // the results.
    //
    // Returns false on error.
    bool Tokenize(const ustring& unicode, vector<string>* tokens,
                  vector<Span>* token2unicode) const;

  private:
    void UnicodeToPTBAscii(
        const ustring& unicode, string* ptb_ascii,
        vector<uint16_t>* ascii2unicode) const;

    void NormalizeTokens(vector<string>* tokens) const;

    // Step 1: Unicode -> ASCII (index into ascii_data).
    //
    // If not present, maps to whitespace (0x20).
    string ascii_data_;
    unordered_map<uchar, uint16_t> unicode2ascii_;

    // Step 2: ASCII tokenizer trained on the Penn Treebank.
    LaposASCIITokenizer ascii_tokenizer_;

    // Step 3: Token -> token.
    //
    // Used for tokenization postprocessing (escaping and spelling
    // canonicalization to match PTB training data).
    unordered_map<string, string> token2token_;
};

#endif  // CC_TOKENIZATION_TOKENIZER_H_
