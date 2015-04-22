#include "string_transform.h"

void StringTransform::Init(const string& append) {
    append_ = append;
}

void StringTransform::Apply(const string& in, string* out) {
    *out = in + append_;
}
