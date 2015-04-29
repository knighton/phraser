#include "phrase_config_parser.h"

#include "cc/misc/strings.h"

// Parse the top line of a phrases config.
//
// Format: "<name> = <list of comma-separated block names>"
//
// Example: "threat = subject, aux verb, main verb"
static bool ParseHeaderLine(
        const string& line, string* name, vector<string>* block_names,
        string* error) {
    vector<string> pair;
    strings::Split(line, '=', &pair);

    if (pair.size() != 2) {
        *error = "[PhraseConfigParser] Invalid phrase config header line "
                 "(should look like 'name = block1, block2, block3').";
        return false;
    }

    *name = pair[0];
    strings::Trim(name);

    strings::Split(pair[1], ',', block_names);
    for (auto& s : *block_names) {
        strings::Trim(&s);
        if (s.empty()) {
            *error = "[PhraseConfigParser] Empty block name in phrase config "
                     "header.";
            return false;
        }
    }

    if (block_names->empty()) {
        *error =
            "[PhraseConfigParser] Phrase config header has no block names.";
        return false;
    }

    return true;
}

static bool IsLineADivider(const string& line) {
    if (line.empty()) {
        return false;
    }

    for (auto& c : line) {
        if (c != '-') {
            return false;
        }
    }

    return true;
}

// Parse "These (to be +pres +3rd +plur) (regex s(ome space-separated tokens
static bool ParseLine(
        const string& line, EnglishExpressionEvaluator* vocab,
        vector<TokenGroupID>* group_ids, string* error) {
    bool is_inside_expr = false;
    string tmp;
    for (auto& c : line) {
        if (is_inside_expr) {
            if (c == ')') {
                TokenGroupID group_id;
                if (!vocab->AddExpression(tmp, &group_id, error)) {
                    return false;
                }
                group_ids->emplace_back(group_id);
                tmp.clear();
                is_inside_expr = false;
            } else {
                tmp += c;
            }
        } else {
            if (isspace(c)) {
                if (tmp.size()) {
                    TokenGroupID group_id;
                    if (!vocab->AddToken(tmp, &group_id)) {
                        *error =
                            "[PhraseConfigParser] Adding the token failed.";
                        return false;
                    }
                    group_ids->emplace_back(group_id);
                    tmp.clear();
                }
            } else if (c == '(') {
                if (tmp.size()) {
                    *error = "[PhraseConfigParser] Put a space between tokens "
                             "and Expressions.";
                    return false;
                }
                is_inside_expr = true;
            } else if (c == ')') {
                *error = "[PhraseConfigParser] Encountered closing "
                         "parenthesis, but not inside an Expression.";
                return false;
            } else {
                tmp += c;
            }
        }
    }

    if (tmp.size()) {
        if (is_inside_expr) {
            *error = "[PhraseConfigParser] Unterminated expression.";
            return false;
        }

        TokenGroupID group_id;
        if (!vocab->AddToken(tmp, &group_id)) {
            *error =
                "[PhraseConfigParser] Adding the Token failed.";
            return false;
        }
        group_ids->emplace_back(group_id);
    }

    return true;
}

static bool ParseAndAppendContentLine(
        const string& line, EnglishExpressionEvaluator* english,
        vector<vector<vector<TokenGroupID>>>* blocks, string* error) {
    if (blocks->empty()) {
        *error = "[PhraseConfigParser] Blocks start with a divider line "
                 "('----------').";
        return false;
    }

    auto& cur_block = (*blocks)[blocks->size() - 1];

    cur_block.resize(cur_block.size() + 1);
    auto& cur_option = cur_block[cur_block.size() - 1];

    if (!ParseLine(line, english, &cur_option, error)) {
        return false;
    }

    return true;
}

bool PhraseConfigParser::Parse(
        const string& text, EnglishExpressionEvaluator* english,
        PhraseConfig* phrase, string* error) const {
    vector<string> lines;
    strings::Split(text, '\n', &lines);

    if (lines.empty()) {
        *error = "[PhraseConfigParser] No lines present in text.";
        return false;
    }

    if (!ParseHeaderLine(lines[0], &phrase->name, &phrase->block_names,
                         error)) {
        return false;
    }

    for (auto i = 1u; i < lines.size() - 1; ++i) {
        auto& line = lines[i];
        if (IsLineADivider(line)) {
            phrase->blocks.resize(phrase->blocks.size() + 1);
        } else {
            if (!ParseAndAppendContentLine(
                    line, english, &phrase->blocks, error)) {
                return false;
            }
        }
    }

    return true;
}
