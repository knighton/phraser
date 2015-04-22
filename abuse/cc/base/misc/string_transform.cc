#include "string_transform.h"

StringTransform StringTransform::Create(const string& append) {
    StringTransform t;
    t.Init(append);
    return t;
}

void StringTransform::Init(const string& append) {
    append_ = append;
}

void StringTransform::Apply(const string& in, string* out) const {
    *out = in + append_;
}
