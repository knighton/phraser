#include "phrase_config_parser.h"

#include "cc/misc/strings.h"

// Parse the top line of a phrases config.
//
// Format: "<name> = <list of comma-separated block names>"
//
// Example: "threat = subject, aux verb, main verb"
static bool ParseHeaderLine(
        const string& line, string* name, vector<string>* block_names) {
    vector<string> pair;
    strings::Split(line, '=', &pair);

    if (pair.size() != 2) {
        return false;
    }

    *name = pair[0];
    strings::Trim(name);

    strings::Split(pair[1], ',', block_names);
    for (auto& s : *block_names) {
        strings::Trim(&s);
        if (s.empty()) {
            return false;
        }
    }

    if (block_names->empty()) {
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
        vector<TokenGroupID>* group_ids) {
    bool is_inside_expr = false;
    string tmp;
    for (auto& c : line) {
        if (is_inside_expr) {
            if (c == ')') {
                TokenGroupID group_id;
                if (!vocab->AddExpression(tmp, &group_id)) {
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
                        return false;
                    }
                    group_ids->emplace_back(group_id);
                    tmp.clear();
                }
            } else if (c == '(') {
                if (tmp.size()) {
                    return false;
                }
                is_inside_expr = true;
            } else if (c == ')') {
                return false;
            } else {
                tmp += c;
            }
        }
    }
    return true;
}

static bool ParseAndAppendContentLine(
        const string& line, EnglishExpressionEvaluator* english,
        vector<vector<vector<TokenGroupID>>>* blocks) {
    if (blocks->empty()) {
        return false;
    }

    auto& cur_block = (*blocks)[blocks->size() - 1];

    cur_block.resize(cur_block.size() + 1);
    auto& cur_option = cur_block[cur_block.size() - 1];

    if (!ParseLine(line, english, &cur_option)) {
        return false;
    }

    return true;
}

bool PhraseConfigParser::Parse(
        const string& text, EnglishExpressionEvaluator* english,
        PhraseConfig* phrase) const {
    vector<string> lines;
    strings::Split(text, '\n', &lines);

    if (lines.empty()) {
        return false;
    }

    string name;
    vector<string> block_names;
    if (!ParseHeaderLine(lines[0], &name, &block_names)) {
        return false;
    }

    vector<vector<vector<TokenGroupID>>> blocks;
    for (auto i = 1u; i < lines.size(); ++i) {
        auto& line = lines[i];
        if (IsLineADivider(line)) {
            blocks.resize(blocks.size() + 1);
        } else {
            if (!ParseAndAppendContentLine(line, english, &blocks)) {
                return false;
            }
        }
    }

    return true;
}
