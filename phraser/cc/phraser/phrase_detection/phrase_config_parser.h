#ifndef CC_PHRASER_PHRASE_DETECTION_PHRASE_CONFIG_PARSER_H_
#define CC_PHRASER_PHRASE_DETECTION_PHRASE_CONFIG_PARSER_H_

#include <string>

#include "cc/english/english_expression_evaluator.h"
#include "cc/phrase_detection/phrase_config.h"

using std::string;

// Parses phrase files.
//
// Syntax:
//
//     | (name) = (list of block names, separated by commas)
//     | (list of blocks, separated by newlines)
//
// where
//
// * The number of block names must match the number of blocks
// * There must be at least one block
// * A block is
//
//     | (divider)
//     | (1+ config lines)
//
// * A divider is a line that is nothing but 1+ dash characters
// * A config line is a list of tokens and expressions, separated by
//   whitespace
// * A token is just a string with no internal spaces
// * An expression is a string enclosed by ( and )
class PhraseConfigParser {
  public:
    bool Parse(const string& text, EnglishExpressionEvaluator* eval,
               PhraseConfig* phrase, string* error) const;
};

#endif  // CC_PHRASER_PHRASE_DETECTION_PHRASE_CONFIG_PARSER_H_
