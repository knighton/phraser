#include "tag_evaluator.h"

TagEvaluator::TagEvaluator() : tagger_(NULL) {
}

TagEvaluator::~TagEvaluator() {
    if (tagger_) {
        delete tagger_;
        tagger_ = NULL;
    }
}

bool TagEvaluator::Init(Tagger* tagger, string* error) {
    type_ = "tag";
    dimension2possible_values_.clear();
    tagger_ = tagger;
    return PostInit(error);
}

bool TagEvaluator::AnalyzeTokens(const vector<string>& tokens,
                                 vector<string>* tags, string* error) const {
    return tagger_->Tag(tokens, tags, error);
}

bool TagEvaluator::IsMatch(
        const Expression& expr, const string& token,
        const string& tag) const {
    auto& want_tag = expr.args()[0];
    return tag == want_tag;
}

bool TagEvaluator::AreArgsPossible(
        const vector<string>& args, string* error) const {
    // TODO: check against an actual list of Penn tags.
    if (args.size() != 1) {
        *error = "[TagEvaluator] Must have 1 arg: the Penn tag to filter by.";
        return false;
    }

    return true;
}
