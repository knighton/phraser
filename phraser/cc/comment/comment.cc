#include "comment.h"

void Comment::Init(const ustring& text, const vector<string>& tags) {
    text_ = text;
    tags_ = tags;
}
