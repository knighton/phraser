#include "json.h"

#include "cc/misc/strings.h"

namespace json {

Object::Object() {
    type_ = JOT_NULL;
    data_ = NULL;
}

Object::Object(int64_t n) {
    type_ = JOT_INT;
    auto data = new int64_t(n);
    data_ = reinterpret_cast<void*>(data);
}

Object::Object(const string& s) {
    type_ = JOT_STR;
    auto data = new string(s);
    data_ = reinterpret_cast<void*>(data);
}

Object::Object(const vector<uint64_t>& vec) {
    vector<int64_t> v;
    v.reserve(vec.size());
    for (auto& it : vec) {
        auto n = static_cast<int64_t>(it);
        v.emplace_back(n);
    }
    InitFromInts(v);
}

Object::Object(const set<uint64_t>& set) {
    vector<int64_t> v;
    v.reserve(set.size());
    for (auto& it : set) {
        auto n = static_cast<int64_t>(it);
        v.emplace_back(n);
    }
    InitFromInts(v);
}

Object::Object(const unordered_set<uint64_t>& set) {
    vector<int64_t> v;
    v.reserve(set.size());
    for (auto& it : set) {
        auto n = static_cast<int64_t>(it);
        v.emplace_back(n);
    }
    InitFromInts(v);
}

Object::Object(const vector<size_t>& vec) {
    vector<int64_t> v;
    v.reserve(vec.size());
    for (auto& it : vec) {
        auto n = static_cast<int64_t>(it);
        v.emplace_back(n);
    }
    InitFromInts(v);
}

Object::Object(const set<size_t>& set) {
    vector<int64_t> v;
    v.reserve(set.size());
    for (auto& it : set) {
        auto n = static_cast<int64_t>(it);
        v.emplace_back(n);
    }
    InitFromInts(v);
}

Object::Object(const unordered_set<size_t>& set) {
    vector<int64_t> v;
    v.reserve(set.size());
    for (auto& it : set) {
        auto n = static_cast<int64_t>(it);
        v.emplace_back(n);
    }
    InitFromInts(v);
}

Object::Object(const vector<uint32_t>& vec) {
    vector<int64_t> v;
    v.reserve(vec.size());
    for (auto& it : vec) {
        auto n = static_cast<int64_t>(it);
        v.emplace_back(n);
    }
    InitFromInts(v);
}

Object::Object(const set<uint32_t>& set) {
    vector<int64_t> v;
    v.reserve(set.size());
    for (auto& it : set) {
        auto n = static_cast<int64_t>(it);
        v.emplace_back(n);
    }
    InitFromInts(v);
}

Object::Object(const unordered_set<uint32_t>& set) {
    vector<int64_t> v;
    v.reserve(set.size());
    for (auto& it : set) {
        auto n = static_cast<int64_t>(it);
        v.emplace_back(n);
    }
    InitFromInts(v);
}

Object::Object(const vector<uint16_t>& vec) {
    vector<int64_t> v;
    v.reserve(vec.size());
    for (auto& it : vec) {
        auto n = static_cast<int64_t>(it);
        v.emplace_back(n);
    }
    InitFromInts(v);
}

Object::Object(const set<uint16_t>& set) {
    vector<int64_t> v;
    v.reserve(set.size());
    for (auto& it : set) {
        auto n = static_cast<int64_t>(it);
        v.emplace_back(n);
    }
    InitFromInts(v);
}

Object::Object(const unordered_set<uint16_t>& set) {
    vector<int64_t> v;
    v.reserve(set.size());
    for (auto& it : set) {
        auto n = static_cast<int64_t>(it);
        v.emplace_back(n);
    }
    InitFromInts(v);
}

Object::Object(const vector<string>& v) {
    InitFromStrings(v);
}

Object::Object(const set<string>& set) {
    vector<string> v;
    v.reserve(set.size());
    for (auto& it : set) {
        v.emplace_back(it);
    }
    InitFromStrings(v);
}

Object::Object(const unordered_set<string>& set) {
    vector<string> v;
    v.reserve(set.size());
    for (auto& it : set) {
        v.emplace_back(it);
    }
    InitFromStrings(v);
}

Object::Object(const vector<Object*>& v) {
    type_ = JOT_ARRAY;
    auto data = new vector<Object*>(v);
    data_ = reinterpret_cast<void*>(data);
}

Object::Object(const map<string, Object*>& d) {
    type_ = JOT_OBJ;
    auto data = new map<string, Object*>();
    for (auto& it : d) {
        (*data)[it.first] = it.second;
    }
    data_ = reinterpret_cast<void*>(data);
}

Object::~Object() {
    switch (type_) {
    case JOT_NULL:
        break;
    case JOT_BOOL:
        delete reinterpret_cast<bool*>(data_);
        break;
    case JOT_INT:
        delete reinterpret_cast<int64_t*>(data_);
        break;
    case JOT_STR:
        delete reinterpret_cast<string*>(data_);
        break;
    case JOT_ARRAY:
        {
            auto* data = reinterpret_cast<vector<Object*>*>(data_);
            for (auto& obj : *data) {
                delete obj;
            }
            delete data;
            break;
        }
    case JOT_OBJ:
        {
            auto* data = reinterpret_cast<map<string, Object*>*>(data_);
            for (auto& it : *data) {
                delete it.second;
            }
            delete data;
            break;
        }
    }
}

static void Quote(const string& in, string* out) {
    out->clear();
    out->reserve(in.size() + 2);
    *out += "\"";
    for (auto& c : in) {
        if (c == '"') {
            *out += "\\\"";
        } else {
            *out += c;
        }
    }
    *out += "\"";
}

void Object::AppendToString(
        string* out, size_t spaces_per_indent, size_t offset_spaces,
        bool leading_indent) const {
    string indent0(offset_spaces, ' ');
    string indent1(offset_spaces + spaces_per_indent, ' ');

    if (leading_indent) {
        *out += indent0;
    }

    switch (type_) {
    case JOT_NULL: {
        *out += "null";
        break;
    }
    case JOT_BOOL: {
        auto& b = *reinterpret_cast<bool*>(data_);
        *out += (b ? "true" : "false");
        break;
    }
    case JOT_INT: {
        auto& n = *reinterpret_cast<int64_t*>(data_);
        *out += strings::StringPrintf("%ld", n);
        break;
    }
    case JOT_STR: {
        auto& orig_s = *reinterpret_cast<string*>(data_);
        string quoted_s;
        Quote(orig_s, &quoted_s);
        *out += quoted_s;
        break;
    }
    case JOT_ARRAY: {
        auto& v = *reinterpret_cast<vector<Object*>*>(data_);
        *out += "[";
        if (v.size()) {
            *out += "\n";
        }
        for (auto i = 0u; i < v.size(); ++i) {
            auto& obj = v[i];
            obj->AppendToString(
                out, spaces_per_indent, offset_spaces + spaces_per_indent,
                true);
            if (i != v.size() - 1) {
                *out += ",";
            }
            *out += "\n";
        }
        if (v.size()) {
            *out += indent0;
        }
        *out += "]";
        break;
    }
    case JOT_OBJ: {
        auto& d = *reinterpret_cast<map<string, Object*>*>(data_);
        *out += "{";
        if (d.size()) {
            *out += "\n";
        }
        size_t count = 0;
        for (auto& it : d) {
            auto& orig_s = it.first;
            string quoted_s;
            Quote(orig_s, &quoted_s);
            *out += indent1 + quoted_s + ": ";
            auto& obj = it.second;
            obj->AppendToString(
                out, spaces_per_indent, offset_spaces + spaces_per_indent,
                false);
            if (count != d.size() - 1) {
                *out += ",";
            }
            *out += "\n";
            ++count;
        }
        if (d.size()) {
            *out += indent0;
        }
        *out += "}";
        break;
    }
    }
}

void Object::InitFromInts(const vector<int64_t>& v) {
    type_ = JOT_ARRAY;
    auto data = new vector<Object*>();
    data->reserve(v.size());
    for (auto& n : v) {
        data->emplace_back(new Object(n));
    }
    data_ = reinterpret_cast<void*>(data);
}

void Object::InitFromStrings(const vector<string>& v) {
    type_ = JOT_ARRAY;
    auto data = new vector<Object*>();
    data->reserve(v.size());
    for (auto& s : v) {
        data->emplace_back(new Object(s));
    }
    data_ = reinterpret_cast<void*>(data);
}

}  // namespace json
