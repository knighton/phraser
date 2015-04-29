#ifndef CC_ENGLISH_TAG_TAG_EVALUATOR_H_
#define CC_ENGLISH_TAG_TAG_EVALUATOR_H_

#include <string>
#include <vector>

#include "cc/expression/all_at_once_evaluator.h"
#include "cc/tagging/tagger.h"

using std::string;
using std::vector;

class TagEvaluator : public AllAtOnceEvaluator<string> {
  public:
    TagEvaluator();
    ~TagEvaluator();

    // Takes ownership.
    void Init(Tagger* tagger);

    bool AnalyzeTokens(
        const vector<string>& tokens, vector<string>* tags,
        string* error) const;

    bool IsMatch(
        const Expression& expr, const string& token,
        const string& tag) const;


  private:
    bool AreArgsPossible(const vector<string>& args, string* error) const;

    Tagger* tagger_;
};

#endif  // CC_ENGLISH_TAG_TAG_EVALUATOR_H_
