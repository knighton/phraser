#ifndef CC_PHRASER_ENGLISH_TAG_TAG_EVALUATOR_H_
#define CC_PHRASER_ENGLISH_TAG_TAG_EVALUATOR_H_

#include <string>
#include <vector>

#include "cc/phraser/expression/all_at_once_evaluator.h"

using std::string;
using std::vector;

class TagEvaluator : public AllAtOnceEvaluator<string> {
  public:
    TagEvaluator();
    ~TagEvaluator();

    // Takes ownership.
    bool Init(/*Tagger* tagger,*/ string* error);

    bool AnalyzeTokens(
        const vector<string>& tokens, vector<string>* tags,
        string* error) const;

    bool IsMatch(
        const Expression& expr, const string& token,
        const string& tag) const;


  private:
    bool AreArgsPossible(const vector<string>& args, string* error) const;

    // Tagger* tagger_;
};

#endif  // CC_PHRASER_ENGLISH_TAG_TAG_EVALUATOR_H_
