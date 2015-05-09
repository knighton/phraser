#include "personal_manager.h"

#include "cc/base/combinatorics.h"
#include "cc/phraser/english/common.h"

namespace {

// Row x column = token.
string PERSONAL_TABLE_STR =
    "          NP_SUBJ NP_OBJ   NP_REFL     POS_SUBJ  POS_OBJ    POS_REFL    POS_DET"
    " I        i       me       myself      mine      mine       myself's    my"
    " YOU1     you     you      yourself    yours     yours      yourself's  your"
    " THOU     thou    thee     thyself     thine     thine      thyself's   thy"
    " HE       he      him      himself     his       his        himself's   his"
    " SHE      she     her      herself     hers      hers       herself's   her"
    " IT       it      it       itself      its       its        itself's    its"
    " THEY1    they    them     themself    theirs    theirs     themself's  their"
    " ONE      one     one      oneself     one's     one's      oneself's   one's"
    " WHO1     who     whom     -           whose     whose      -           whose"
    " WHOEVER1 whoever whomever -           whoever's whomever's -           whoever's"
    " WE       we      us       ourselves   ours      ours       ourselves'  our"
    " YOU2     you     you      yourselves  yours     yours      yourselves' your"
    " YALL     yall    yall     yallsselves yall's    yall's     yallsselves yall's"
    " THEY2    they    them     themselves  theirs    theirs     themselves' their"
    " WHO2     who     whom     -           whose     whose      -           whose"
    " WHOEVER2 whoever whomever -           whoever's whomever's -           whoever's";

struct PersonalColumnInfo {
    string col_name;
    string type;
    string ppcase_if_not_posdet;
};

// Column metadata.
vector<PersonalColumnInfo> COL_INFOS = {
    {"NP_SUBJ",  "perspro", "subj"},
    {"NP_OBJ",   "perspro", "obj"},
    {"NP_REFL",  "perspro", "refl"},
    {"POS_SUBJ", "pospro",  "subj"},
    {"POS_OBJ",  "pospro",  "obj"},
    {"POS_REFL", "pospro",  "refl"},
    {"POS_DET",  "posdet",  ""},  // Possessive determiners don't have ppcase.
};

struct PersonalRowInfo {
    string row_name;
    vector<int> fields;
};

// Row name -> list of feature values that can be wildcards (NUM_X).
vector<PersonalRowInfo> NONEABLE_ROW_INFOS = {
    {"I",        {N_SING, P_FIRST,     NUM_PERSONHOODS, NUM_GENDERS}},
    {"YOU1",     {N_SING, P_SECOND,    NUM_PERSONHOODS, NUM_GENDERS}},
    {"THOU",     {N_SING, P_SECOND,    NUM_PERSONHOODS, NUM_GENDERS}},
    {"HE",       {N_SING, P_THIRD,     PH_YES,          G_MALE}},
    {"SHE",      {N_SING, P_THIRD,     PH_YES,          G_FEMALE}},
    {"IT",       {N_SING, P_THIRD,     PH_NO,           G_NEUTER}},
    {"THEY1",    {N_SING, P_THIRD,     PH_YES,          NUM_GENDERS}},
    {"ONE",      {N_SING, P_THIRD,     NUM_PERSONHOODS, NUM_GENDERS}},
    {"WHO1",     {N_SING, P_INTR,      PH_YES,          NUM_GENDERS}},
    {"WHOEVER1", {N_SING, P_INTR_EMPH, PH_YES,          NUM_GENDERS}},
    {"WE",       {N_PLUR, P_FIRST,     NUM_PERSONHOODS, NUM_GENDERS}},
    {"YOU2",     {N_PLUR, P_SECOND,    NUM_PERSONHOODS, NUM_GENDERS}},
    {"YALL",     {N_PLUR, P_SECOND,    NUM_PERSONHOODS, NUM_GENDERS}},
    {"THEY2",    {N_PLUR, P_THIRD,     NUM_PERSONHOODS, NUM_GENDERS}},
    {"WHO2",     {N_PLUR, P_INTR,      PH_YES,          NUM_GENDERS}},
    {"WHOEVER2", {N_PLUR, P_INTR_EMPH, PH_YES,          NUM_GENDERS}},
};

// Dimension name per 'noneable' in the vectors above.
vector<string> DIMENSION_NAME_PER_FIELD = {
    "number",
    "person",
    "personhood",
    "gender",
};

// Canonical token -> list of equally valid tokens that map to it.
unordered_map<string, vector<string>> CANON2OTHERS = {
    {"himself",    {"hisself"}},
    {"yall",       {"y'all"}},
    {"yall's",     {"y'all's"}},
    {"yallselves", {"y'allselves", "yallsselves", "y'allsselves"}},
    {"you",        {"u"}},
    {"whom",       {"who"}},
    {"whomever",   {"whoever"}},
    {"whomever's", {"whoever's"}},
};

// Parse a personals table.
bool ParsePersonalTable(
        const string& personal_table_str,
        const vector<PersonalColumnInfo>& col_infos,
        const vector<PersonalRowInfo>& noneable_row_infos,
        const vector<vector<string>>& options_per_noneable_field,
        const vector<string>& dimension_name_per_noneable_field,
        const unordered_map<string, vector<string>>& canon2others,
        vector<PersonalTokenInfo>* pers_pro_token_infos,
        vector<PersonalTokenInfo>* pos_pro_token_infos,
        vector<PersonalTokenInfo>* pos_det_token_infos, string* error) {
    pers_pro_token_infos->clear();
    pos_pro_token_infos->clear();
    pos_det_token_infos->clear();

    // Extract the tokens.
    vector<string> table;
    strings::SplitByWhitespace(PERSONAL_TABLE_STR, &table);

    // We must have columns.
    if (col_infos.empty()) {
        *error = "[PersonalManager] Table has no columns.";
        return false;
    }

    // We must have rows.
    if (noneable_row_infos.empty()) {
        *error = "[PersonalManager] Table has no rows.";
        return false;
    }

    // Verify table size (m x n).
    auto expected_size =
        col_infos.size() * noneable_row_infos.size() +
        col_infos.size() +
        noneable_row_infos.size();
    if (table.size() != expected_size) {
        *error = "[PersonalManager] Personals table is the wrong number of "
                 "tokens.";
        return false;
    }

    // Verify column names.
    for (auto i = 0u; i < col_infos.size(); ++i) {
        auto& expect_col_name = col_infos[i].col_name;
        auto& have_col_name = table[i];
        if (expect_col_name != have_col_name) {
            *error = "[PersonalManager] Column name mismatch.  Expected: [" +
                     expect_col_name + "].  Have: [" + have_col_name + "].";
            return false;
        }
    }

    // Verify whether column has a ppcase == whether column is not POS_DET.
    for (auto& info : col_infos) {
        auto& ppcase = info.ppcase_if_not_posdet;
        if ((info.col_name == "POS_DET") != ppcase.empty()) {
            *error = "[PersonalManager] POS_DET column must have no ppcase; "
                     "non-POS_DET column must have ppcase.";
            return false;
        }
    }

    // Verify row names.
    for (auto i = 0u; i < noneable_row_infos.size(); ++i) {
        auto& expect_row_name = noneable_row_infos[i].row_name;
        auto index = col_infos.size() + (col_infos.size() + 1) * i;
        auto& have_row_name = table[index];
        if (expect_row_name != have_row_name) {
            *error = "[PersonalManager] Row name mismatch.  Expected: [" +
                     expect_row_name + "].  Have: [" + have_row_name + "].";
            return false;
        }
    }

    // Verify options per noneable.
    for (auto& info : noneable_row_infos) {
        if (info.fields.size() != options_per_noneable_field.size()) {
            *error = "[PersonalManager] Incorrect number of metadata fields "
                     "for row.";
            return false;
        }
        for (auto i = 0u; i < info.fields.size(); ++i) {
            auto& value = info.fields[i];
            auto num_options = options_per_noneable_field[i].size();
            if (!(0 <= value && value <= num_options)) {
                *error = "[PersonalManager] Impossible metadata field value.";
                return false;
            }
        }
    }

    // Expand noneable row infos to resolved row infos.
    vector<vector<vector<int>>> value_lists_per_row;
    {
        value_lists_per_row.resize(noneable_row_infos.size());

        if (noneable_row_infos.empty()) {
            *error = "[PersonalManager] No row infos.";
            return false;
        }

        vector<int> num_options_per_field;
        num_options_per_field.reserve(noneable_row_infos[0].fields.size());
        for (auto& options : options_per_noneable_field) {
            num_options_per_field.emplace_back(options.size());
        }

        for (auto i = 0u; i < noneable_row_infos.size(); ++i) {
            if (!combinatorics::ExpandCatchalls(
                        noneable_row_infos[i].fields, num_options_per_field,
                        &(value_lists_per_row[i]))) {
                *error = "[PersonalManager] Catchall expansion failed.";
                return false;
            }
        }
    }

    // Map column to results vector pointer.
    vector<vector<PersonalTokenInfo>*> results_per_col = {
        pers_pro_token_infos,  // NP_SUBJ
        pers_pro_token_infos,  // NP_OBJ
        pers_pro_token_infos,  // NP_REFL
        pos_pro_token_infos,   // POS_SUBJ
        pos_pro_token_infos,   // POS_OBJ
        pos_pro_token_infos,   // POS_REFL
        pos_det_token_infos,   // POS_DET
    };

    // Nested for loops below:
    // * 16 rows
    // * 7 columns
    // * 1..3 tokens per index
    // * 1..6 value lists per row,
    // * 4 values per value list

    // For each table row, for each table column,
    vector<const string*> tokens_here;
    PersonalTokenInfo token_info;
    for (auto r = 0u; r < value_lists_per_row.size(); ++r) {
        for (auto c = 0u; c < col_infos.size(); ++c) {
            // Get the canonical token.
            auto index = col_infos.size() + (col_infos.size() + 1) * r + c;
            auto& canonical_token = table[index];

            // Skip empty placeholders ("-").
            if (canonical_token == "-") {
                continue;
            }

            // Get the other tokens that are aliases for the canonical token.
            tokens_here.clear();
            tokens_here.emplace_back(&canonical_token);
            auto it = canon2others.find(canonical_token);
            if (it != canon2others.end()) {
                for (auto& other : it->second) {
                    tokens_here.emplace_back(&other);
                }
            }

            // For each token, for each resolved (no catchalls) row infos,
            for (auto& token : tokens_here) {
                for (auto i = 0u; i < value_lists_per_row[r].size(); ++i) {
                    auto& values = value_lists_per_row[r][i];

                    // Initialize the token info.
                    token_info.token = *token;
                    token_info.dimension2value.clear();

                    // Get info associated with this table column.
                    auto& ppcase = col_infos[c].ppcase_if_not_posdet;
                    if (ppcase.size()) {
                        token_info.dimension2value["ppcase"] = ppcase;
                    }

                    // Get info associated with this table row.
                    for (auto j = 0u; j < values.size(); ++j) {
                        auto& dim_name = dimension_name_per_noneable_field[j];
                        auto& dim_value_int = values[j];
                        auto& dim_value_str =
                            options_per_noneable_field[j][dim_value_int];
                        token_info.dimension2value[dim_name] = dim_value_str;
                    }

                    // Append it to the correct results vector.
                    vector<PersonalTokenInfo>* results = results_per_col[c];
                    results->emplace_back(token_info);
                }
            }
        }
    }

    return true;
}

}  // namespace

bool PersonalManager::Init(string* error) {
    // List of options per 'noneable' in the vectors above.
    vector<vector<string>> OPTIONS_PER_FIELD = {
        NumberStrings.strings(),
        PersonStrings.strings(),
        PersonhoodStrings.strings(),
        GenderStrings.strings(),
    };

    if (!ParsePersonalTable(
            PERSONAL_TABLE_STR, COL_INFOS, NONEABLE_ROW_INFOS,
            OPTIONS_PER_FIELD, DIMENSION_NAME_PER_FIELD, CANON2OTHERS,
            &pers_pro_infos_, &pos_pro_infos_, &pos_det_infos_, error)) {
        return false;
    }

    type2tokeninfos_ = {
        {"perspro", &pers_pro_infos_},
        {"pospro",  &pos_pro_infos_},
        {"posdet",  &pos_det_infos_},
    };

    return true;
}

bool PersonalManager::GetExpressionMatches(
        const Expression& expr, vector<string>* tokens) const {
    auto it = type2tokeninfos_.find(expr.type());
    if (it == type2tokeninfos_.end()) {
        return false;
    }

    auto& infos = *(it->second);
    for (auto& info : infos) {
        if (expr.AcceptsFeatures(info.dimension2value)) {
            tokens->emplace_back(info.token);
        }
    }

    return true;
}
