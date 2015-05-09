#include "uncontractor.h"

void Uncontractor::Init() {
    bigram_exceptions_ = {
        {"wo", "n't", "will", "not"},
        {"let", "'s", "let", "us"},
    };

    suffix2uncontracted_ = {
        {"n't", {"not"}},
        {"'d",  {"did", "had", "would"}},
        {"'ll", {"will"}},
        {"'m",  {"am"}},
        {"'re", {"are"}},
        {"'s",  {"is", "has"}},
        {"'ve", {"have"}},
    };
}

static Token* NewToken(vector<Token>* v) {
    size_t z = v->size();
    v->resize(z + 1);
    return &(*v)[z];
}

void Uncontractor::UncontractLoneUnigram(
        const WordAndTag& wt, vector<Token>* tokens) const {
    // The word is a contraction suffix.
    auto it = suffix2uncontracted_.find(wt.word);
    if (it != suffix2uncontracted_.end()) {
        auto* token = NewToken(tokens);
        token->possible_words = it->second;
        token->tag = wt.tag;
        return;
    }

    // Not a contraction suffix.
    auto* token = NewToken(tokens);
    token->possible_words.emplace_back(wt.word);
    token->tag = wt.tag;
}

size_t Uncontractor::UncontractBigram(
        const WordAndTag& base, const WordAndTag& suffix,
        vector<Token>* tokens) const {
    // Exceptions (won't, let's).
    for (auto& e : bigram_exceptions_) {
        if (base.word == e.base && suffix.word == e.suffix) {
            auto* token = NewToken(tokens);
            token->possible_words.emplace_back(e.to_first);
            token->tag = base.tag;
            token = NewToken(tokens);
            token->possible_words.emplace_back(e.to_second);
            token->tag = suffix.tag;
            return 2;
        }
    }

    // Normal contractions.
    auto it = suffix2uncontracted_.find(suffix.word);
    if (it != suffix2uncontracted_.end()) {
        auto* token = NewToken(tokens);
        token->possible_words.emplace_back(base.word);
        token->tag = base.tag;
        token = NewToken(tokens);
        token->possible_words = it->second;
        token->tag = suffix.tag;
        return 2;
    }

    // Not a contraction.
    auto* token = NewToken(tokens);
    token->possible_words.emplace_back(base.word);
    token->tag = base.tag;
    return 1;
}

void Uncontractor::Uncontract(
        const vector<WordAndTag>& in, vector<Token>* out) const {
    if (in.size() == 1) {
        UncontractLoneUnigram(in[0], out);
        return;
    }

    size_t i = 0;
    while (i < in.size() - 1) {
        auto& base = in[i];
        auto& suffix = in[i + 1];
        i += UncontractBigram(base, suffix, out);
    }
}
