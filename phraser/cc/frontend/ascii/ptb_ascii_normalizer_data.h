#ifndef CC_FRONTEND_ASCII_PTB_ASCII_NORMALIZER_DATA_H_
#define CC_FRONTEND_ASCII_PTB_ASCII_NORMALIZER_DATA_H_

#include <string>
#include <unordered_map>

using std::string;
using std::unordered_map;

// Automatically generated file -- do not edit!

namespace ptb_ascii_normalizer_data {

// Block containing newline-separated (as space is a valid character) strings
// concatenated together.
extern string ASCII_DATA;

// Unicode -> PTB ASCII (index into ASCII_DATA).
//
// If not present, maps to whitespace (0x20).
//
// How this mapping was created:
// 1. If nonprintable ASCII, normalize to a space.
// 2. Normalize the various open/close quotes to smart quotes.
// 3. Convert currency symbols to dollars and cents (to match PTB training
//    data).
// 4. Convert smart quotes to PTB tokens.
// 5. Decompose the code points according to NFKD.
// 6. De-confuse the resulting code points according to the "SA" confusables
//    table, excluding ones that are already ASCII.
// 7. Drop non-ASCII.
// 8. Condense results that are all spaces to a single space.
// 9. Drop parenthesized characters that don't map to Latin.
extern unordered_map<uint32_t, uint16_t> UNICODE2ASCII;

}  // namespace ptb_ascii_normalizer_data

#endif  // CC_FRONTEND_ASCII_PTB_ASCII_NORMALIZER_DATA_H_
