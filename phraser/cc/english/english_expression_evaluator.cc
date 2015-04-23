#include "english_expression_evaluator.h"

#include <string>
#include <unordered_map>

#include "cc/english/number/number_evaluator.h"
#include "cc/english/tag/tag_evaluator.h"
#include "cc/english/verb/conjugator.h"
#include "cc/english/verb/conjugation_spec.h"
#include "cc/english/verb/conjugation_spec_derivation.h"
#include "cc/english/verb/verb_evaluator.h"
#include "cc/english/english_config.h"
#include "cc/tagging/lapos/lapos_tagger.h"

using std::string;
using std::unordered_map;

static NumberEvaluator* MakeNumberEvaluator() {
    NumberEvaluator* e = new NumberEvaluator();
    if (!e) {
        return NULL;
    }

    e->Init();
    return e;
}

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

#define T StringTransform::Create

static VerbEvaluator* MakeVerbEvaluator() {
    Conjugator* conj = new Conjugator();
    if (!conj) {
        return NULL;
    }

    ConjugationSpec be;
    be.Init("be", "being", "been", {"am", "are", "is", "are", "are", "are"},
            {"was", "were", "was", "were", "were", "were"});

    unordered_map<string, ConjugationSpec> lemma2irregular = {
        {"be", be},
    };

    ConjugationSpecDerivation regular_deriv;
    regular_deriv.InitFromTransforms(
        T("ing"),
        T("ed"),
        {T(""), T(""), T("s"), T(""), T(""), T("")},
        {T("ed"), T("ed"), T("ed"), T("ed"), T("ed"), T("ed")}
    );
    conj->Init(lemma2irregular, regular_deriv);

    VerbEvaluator* e = new VerbEvaluator();
    if (!e) {
        return NULL;
    }

    e->Init(conj);
    return e;
}

#undef T

bool EnglishExpressionEvaluator::InitWithData(
        const EnglishConfig& config, const vector<Expression>& expressions,
        const vector<string>& raw_tokens) {
    VerbEvaluator* v = MakeVerbEvaluator();
    unordered_map<string, PrecomputableEvaluator*> type2precomputable = {
        {"to",   v},
        {"verb", v},
    };

    unordered_map<string, DynamicEvaluator*> type2dynamic = {
        {"number", MakeNumberEvaluator()},
    };

    unordered_map<string, AllInputEvaluator<string>*> type2all_input = {
        {"tag", MakeTagEvaluator(config.lapos_model_f)},
    };

    InitWithEvaluatorsAndData(
        type2precomputable, type2dynamic, type2all_input, expressions,
        raw_tokens);
    return true;
}
