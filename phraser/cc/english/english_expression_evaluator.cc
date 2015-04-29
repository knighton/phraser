#include "english_expression_evaluator.h"

#include <string>
#include <unordered_map>

#include "cc/english/number/number_evaluator.h"
#include "cc/english/personal/pers_pro_evaluator.h"
#include "cc/english/personal/pos_pro_evaluator.h"
#include "cc/english/personal/pos_det_evaluator.h"
#include "cc/english/tag/tag_evaluator.h"
#include "cc/english/verb/conjugator.h"
#include "cc/english/verb/conjugation_spec.h"
#include "cc/english/verb/conjugation_spec_derivation.h"
#include "cc/english/verb/verb_evaluator.h"
#include "cc/english/english_config.h"
#include "cc/tagging/lapos/lapos_tagger.h"

using std::string;
using std::unordered_map;

namespace {

bool MakePersonalEvaluators(
        PersonalManager* personal_mgr, PersProEvaluator** pers_pro,
        PosProEvaluator** pos_pro, PosDetEvaluator** pos_det, string* error) {
    if (!personal_mgr->Init(error)) {
        return false;
    }

    *pers_pro = new PersProEvaluator();
    if (!*pers_pro) {
        return false;
    }
    if (!(*pers_pro)->Init(personal_mgr)) {
        return false;
    }

    *pos_pro = new PosProEvaluator();
    if (!*pos_pro) {
        return false;
    }
    if (!(*pos_pro)->Init(personal_mgr)) {
        return false;
    }

    *pos_det = new PosDetEvaluator();
    if (!*pos_det) {
        return false;
    }
    if (!(*pos_det)->Init(personal_mgr)) {
        return false;
    }

    return true;
}

NumberEvaluator* MakeNumberEvaluator() {
    NumberEvaluator* e = new NumberEvaluator();
    if (!e) {
        return NULL;
    }

    e->Init();
    return e;
}

TagEvaluator* MakeTagEvaluator(const string& lapos_model_f) {
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

VerbEvaluator* MakeVerbEvaluator() {
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

}  // namespace

bool EnglishExpressionEvaluator::InitWithConfig(
        const EnglishConfig& config, string* error) {
    PersProEvaluator* pers_pro;
    PosProEvaluator* pos_pro;
    PosDetEvaluator* pos_det;
    if (!MakePersonalEvaluators(&personal_mgr_, &pers_pro, &pos_pro, &pos_det,
                                error)) {
        return false;
    }

    unordered_map<string, PrecomputableEvaluator*> type2precomputable = {
        // Personals.
        {"perspro", pers_pro},
        {"pospro", pos_pro},
        {"posdet", pos_det},

        // Verbs.
        {"to", MakeVerbEvaluator()},
    };

    unordered_map<string, DynamicEvaluator*> type2dynamic = {
        {"number", MakeNumberEvaluator()},
    };

    unordered_map<string, AllAtOnceEvaluator<string>*> type2all_at_once = {
        {"tag", MakeTagEvaluator(config.lapos_model_f)},
    };

    return InitWithEvaluators(type2precomputable, type2dynamic,
                              type2all_at_once);
}
