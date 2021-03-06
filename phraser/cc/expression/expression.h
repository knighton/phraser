#ifndef CC_EXPRESSION_EXPRESSION_H_
#define CC_EXPRESSION_EXPRESSION_H_

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "cc/base/json.h"

using std::string;
using std::vector;
using std::unordered_map;
using std::unordered_set;

// Thing a user writes that matches tokens.
class Expression {
  public:
    const string& type() const { return type_; }
    const vector<string>& args() const { return args_; }
    const unordered_map<string, unordered_set<string>>&
            dimension2values() const {
        return dimension2values_;
    }

    void Init(const string& type, const vector<string>& args,
              const unordered_map<string, unordered_set<string>>&
                  dimension2values);

    // Dump to JSON.
    json::Object* ToJSON() const;

    // Do we match against these token features?
    bool AcceptsFeatures(const unordered_map<string, string>& dim2value) const;

    // Dump to a string, in a canonically ordered, human-readable form.
    void ToCanonicalString(string* s) const;

  private:
    // The name of the Evaluator for me (eg. "tag").
    string type_;

    // The list of arguments (eg, "(to go)" -> [go]).
    vector<string> args_;

    // Dimension -> list of values of that dimension.
    //
    // One value from each listed dimension must match for there to be a match.
    unordered_map<string, unordered_set<string>> dimension2values_;
};

#endif  // CC_EXPRESSION_EXPRESSION_H_
