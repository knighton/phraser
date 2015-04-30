#ifndef CC_TOKENIZATION_TOKENIZER_H_
#define CC_TOKENIZATION_TOKENIZER_H_

#include <string>
#include <unordered_map>
#include <vector>

#include "cc/misc/unicode.h"
#include "cc/tokenization/lapos_ascii_tokenizer.h"

using std::string;
using std::unordered_map;
using std::vector;

// Tokenizer.
//
// HTML -> Unicode -> PTB ASCII -> PTB tokens -> normalized PTB tokens.
//
// It's essentially pre/post-processing around an ASCII tokenizer that does the
// actual work.  Our custom mappings (eg, Unicode -> PTB ASCII) are precomputed
// for performance.
class Tokenizer {
  public:
    // Init with configuration.
    bool Init(const unordered_map<string, UChar>& html2unicode,
              const string& ascii_data,
              const unordered_map<UChar, uint16_t>& unicode2ascii,
              const unordered_map<string, string>& token2token, string* error);

    // Initialize from definitions in tokenizer_data.h.
    bool InitDefault(string* error);

    // Text -> tokens.
    //
    // Normalize the text to Penn Treebank ASCII, tokenize it, then postprocess
    // the results.
    //
    // Returns false on error.
    void Tokenize(const vector<UChar>& text, bool replace_html_entities,
                  vector<string>* tokens) const;

  private:
    bool ParseHTMLEntity(
        const vector<UChar>& html, size_t begin, size_t end_excl,
        UChar* out) const;

    void HandlePossibleHTMLEntity(
        const vector<UChar>& html, size_t amp_index, size_t semicolon_index,
        vector<UChar>* out) const;

    bool IsPossibleHTMLEntityChar(UChar c) const;

    void ReplaceHTMLEntities(
        const vector<UChar>& html, vector<UChar>* unicode) const;

    void UnicodeToPTBAscii(
        const vector<UChar>& unicode, string* ptb_ascii) const;

    void NormalizeTokens(vector<string>* tokens) const;

    // Step 1: HTML entity -> Unicode code point.
    unordered_map<string, UChar> html2unicode_;
    size_t max_html_length_;

    // Step 2: Unicode -> ASCII (index into ascii_data).
    //
    // If not present, maps to whitespace (0x20).
    string ascii_data_;
    unordered_map<UChar, uint16_t> unicode2ascii_;

    // Step 3: ASCII tokenizer trained on the Penn Treebank.
    LaposASCIITokenizer ascii_tokenizer_;

    // Step 4: Token -> token.
    //
    // Used for tokenization postprocessing (escaping and spelling
    // canonicalization to match PTB training data).
    unordered_map<string, string> token2token_;
};

#endif  // CC_TOKENIZATION_TOKENIZER_H_
