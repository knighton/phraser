#ifndef CC_BASE_TOKEN_CATEGORIZATION_EVALUATOR_H_
#define CC_BASE_TOKEN_CATEGORIZATION_EVALUATOR_H_

#include <string>
#include <unordered_map>
#include <unordered_set>

using std::string;
using std::unordered_map;
using std::unordered_set;

class Evaluator {
  public:
    const string& type() const { return type_; }
    const unordered_map<string, unordered_set<string>>&
            dimension2filters() const {
        return dimension2values_; }
    }

    virtual ~Evaluator() = 0;

    // Is the expression possible?
    //
    // Reasons for false that are common to all Evaluators:
    // * Its type does not match my type
    // * It has a filter dimension I have never heard of
    //
    // Reasons for false that are per Evaluator:
    // * Its args are invalid (override AreArgsPossible()).
    bool IsExpressionPossible(const Expression& expr) const;

    // Check the expression's args.
    virtual bool AreArgsPossible(const vector<string>& args) const = 0;

  private:
    string type_;
    unordered_map<string, unordered_set<string>> dimension2values_;
};

#endif  // CC_BASE_TOKEN_CATEGORIZATION_EVALUATOR_H_
