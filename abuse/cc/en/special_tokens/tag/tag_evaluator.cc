#include "tag_evaluator.h"

TagEvaluator::TagEvaluator() : tagger_(NULL) {
}

TagEvaluator::~TagEvaluator() {
    if (tagger_) {
        delete tagger_;
        tagger_ = NULL;
    }
}

void TagEvaluator::Init(Tagger* tagger) {
    tagger_ = tagger;
}

bool TagEvaluator::AnalyzeTokens(const vector<string>& tokens,
                                 vector<string>* tags) const {
    return tagger_->Tag(tokens, tags);
}

bool TagEvaluator::IsMatch(
        const Expression& expr, const string& token,
        const string& tag) const {
    auto& want_tag = expr.args()[0];
    return tag == want_tag;
}

bool TagEvaluator::AreArgsPossible(const vector<string>& args) const {
    // TODO: check against an actual list of Penn tags.
    return args.size() == 0;
}
