#include "conjugation_spec.h"

#include "cc/base/combinatorics.h"
#include "cc/english/common.h"

EnumStrings<ConjSpecFieldType> ConjSpecFieldTypeStrings =
    EnumStrings<ConjSpecFieldType>("lemma prespart pastpart pres past");

void ConjugationSpec::Init(
        const string& lemma, const string& pres_part, const string& past_part,
        const vector<string>& pres, const vector<string>& past) {
    lemma_ = lemma;
    pres_part_ = pres_part;
    past_part_ = past_part;
    pres_ = pres;
    past_ = past;
}

const string& ConjugationSpec::GetField(size_t field_index) const {
    if (field_index == 0) {
        return lemma_;
    } else if (field_index == 1) {
        return pres_part_;
    } else if (field_index == 2) {
        return past_part_;
    } else if (field_index < 9) {
        return pres_[field_index - 3];
    } else {
        return past_[field_index - 9];
    }
}

#define FieldValue uint8_t
#define num(name) static_cast<FieldValue>(name)

bool ConjugationSpec::EachTokenWithFeatures(
        vector<TokenWithFeatures>* outs) const {
    outs->clear();

    vector<vector<FieldValue>> ints_with_catchalls = {
        {num(NUM_NUMBERS), num(NUM_PERSONS), num(CSFT_LEMMA)},
        {num(NUM_NUMBERS), num(NUM_PERSONS), num(CSFT_PRES_PART)},
        {num(NUM_NUMBERS), num(NUM_PERSONS), num(CSFT_PAST_PART)},
    };

    for (auto i = 0u; i < pres_.size(); ++i) {
        FieldValue number = num(i / 3);
        FieldValue person = num(i % 3);
        ints_with_catchalls.emplace_back(
            vector<FieldValue>({number, person, num(CSFT_FINITE_PRES)}));
    }

    for (auto i = 0u; i < past_.size(); ++i) {
        FieldValue number = num(i / 3);
        FieldValue person = num(i % 3);
        ints_with_catchalls.emplace_back(
            vector<FieldValue>({number, person, num(CSFT_FINITE_PAST)}));
    }

    vector<FieldValue> num_options_per_field_if_catchall = {
        num(NUM_NUMBERS),
        num(NUM_PERSONS),
        num(NUM_CONJ_SPEC_FIELD_TYPES),
    };

    for (auto i = 0u; i < ints_with_catchalls.size(); ++i) {
        vector<vector<FieldValue>> int_lists;
        if (!combinatorics::ExpandCatchalls(
                ints_with_catchalls[i], num_options_per_field_if_catchall,
                &int_lists)) {
            return false;
        }

        auto& token = GetField(i);
        for (auto& ints : int_lists) {
            outs->resize(outs->size() + 1);
            auto& out = (*outs)[outs->size() - 1];
            out.token = token;

            auto n = static_cast<Number>(ints[0]);
            out.dimension2value["number"] = NumberStrings.GetString(n);

            auto p = static_cast<Person>(ints[1]);
            out.dimension2value["person"] = PersonStrings.GetString(p);

            auto csft = static_cast<ConjSpecFieldType>(ints[2]);
            out.dimension2value["conj_spec_field_type"] =
                ConjSpecFieldTypeStrings.GetString(csft);
        }
    }

    return true;
}

#undef num
#undef FieldValue
