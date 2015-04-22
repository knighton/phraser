#include "english_token_categorizer.h"

#include "cc/en/special_tokens/tag/tag_evaluator.h"
#include "cc/en/tagging/lapos_tagger.h"

static TagEvaluator* MakeTagEvaluator(const string& lapos_model_f) {
    LaposTagger* tagger = new LaposTagger();
    if (!tagger) {
        return NULL;
    }

    if (!tagger->Init(lapos_model_f)) {
        return NULL;
    }

    TagEvaluator* e = new TagEvaluator();
    if (!e) {
        return NULL;
    }

    e->Init(tagger);

    return e;
}

bool EnglishTokenCategorizer::InitWithData(
        const string& lapos_model_f, const vector<Expression>& expressions,
        const vector<string>& raw_tokens) {
    unordered_map<string, PrecomputableEvaluator*> type2precomputable;
    unordered_map<string, DynamicEvaluator*> type2dynamic;
    unordered_map<string, AllInputEvaluator<string>*> type2all_input;

    type2all_input = {
        {"tag", MakeTagEvaluator(lapos_model_f)},
    };

    InitWithEvaluatorsAndData(
        type2precomputable, type2dynamic, type2all_input, expressions,
        raw_tokens);
    return true;
}
