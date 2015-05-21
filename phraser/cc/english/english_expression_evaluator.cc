#include "english_expression_evaluator.h"

#include <string>
#include <unordered_map>

#include "cc/english/number/number_evaluator.h"
#include "cc/english/personal/pers_pro_evaluator.h"
#include "cc/english/personal/pos_pro_evaluator.h"
#include "cc/english/personal/pos_det_evaluator.h"
#include "cc/english/verb/conjugator.h"
#include "cc/english/verb/conjugation_spec.h"
#include "cc/english/verb/conjugation_spec_derivation.h"
#include "cc/english/verb/verb_evaluator.h"

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
        *error = "Allocating the persProEvaluator failed.";
        return false;
    }
    if (!(*pers_pro)->Init(personal_mgr, error)) {
        return false;
    }

    *pos_pro = new PosProEvaluator();
    if (!*pos_pro) {
        *error = "Allocating the PosProEvaluator failed.";
        return false;
    }
    if (!(*pos_pro)->Init(personal_mgr, error)) {
        return false;
    }

    *pos_det = new PosDetEvaluator();
    if (!*pos_det) {
        *error = "Allocating the PosDetEvaluator failed.";
        return false;
    }
    if (!(*pos_det)->Init(personal_mgr, error)) {
        return false;
    }

    return true;
}

NumberEvaluator* MakeNumberEvaluator(string* error) {
    NumberEvaluator* e = new NumberEvaluator();
    if (!e) {
        *error = "Allocating the NumberEvaluator failed.";
        return NULL;
    }

    if (!e->Init(error)) {
        return NULL;
    }

    return e;
}

#define T StringTransform::Create

VerbEvaluator* MakeVerbEvaluator(string* error) {
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

    if (!e->Init(conj, error)) {
        return NULL;
    }

    return e;
}

#undef T

}  // namespace

bool EnglishExpressionEvaluator::Init(string* error) {
    // Precomputable evaluators.
    unordered_map<string, PrecomputableEvaluator*> type2precomputable;
    {
        PersProEvaluator* pers_pro;
        PosProEvaluator* pos_pro;
        PosDetEvaluator* pos_det;
        if (!MakePersonalEvaluators(
                &personal_mgr_, &pers_pro, &pos_pro, &pos_det, error)) {
            return false;
        }

        VerbEvaluator* verb = MakeVerbEvaluator(error);
        if (!verb) {
            return false;
        }

        type2precomputable = {
            {"perspro", pers_pro},
            {"pospro", pos_pro},
            {"posdet", pos_det},

            {"to", verb},
        };
    }

    // Dynamic evaluators.
    unordered_map<string, DynamicEvaluator*> type2dynamic;
    {
        NumberEvaluator* number = MakeNumberEvaluator(error);
        if (!number) {
            return false;
        }

        type2dynamic = {
            {"number", number},
        };
    }

    // All-at-once evaluators.
    unordered_map<string, AllAtOnceEvaluator<string>*> type2all_at_once;

    return InitWithEvaluators(
        type2precomputable, type2dynamic, type2all_at_once);
}
