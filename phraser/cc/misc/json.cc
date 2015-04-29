#include "json.h"

namespace json {

Object::Object(const string& s) {
    type_ = JOT_STR;
    auto data = new string(s);
    data_ = reinterpret_cast<void*>(data);
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

Object::Object(const map<string, Object*>& d) {
    type_ = JOT_DICT;
    auto data = new map<string, Object*>();
    for (auto& it : d) {
        (*data)[it.first] = it.second;
    }
    data_ = reinterpret_cast<void*>(data);
}

Object::~Object() {
    switch (type_) {
    case JOT_BOOL:
        delete reinterpret_cast<bool*>(data_);
        break;
    case JOT_INT:
        delete reinterpret_cast<int64_t*>(data_);
        break;
    case JOT_STR:
        delete reinterpret_cast<string*>(data_);
        break;
    case JOT_LIST:
        {
            auto* data = reinterpret_cast<vector<Object*>*>(data_);
            for (auto& obj : *data) {
                delete obj;
            }
            delete data;
            break;
        }
    case JOT_DICT:
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

void Object::InitFromStrings(const vector<string>& v) {
    type_ = JOT_LIST;
    auto data = new vector<Object*>();
    data->reserve(v.size());
    for (auto& s : v) {
        data->emplace_back(new Object(s));
    }
    data_ = reinterpret_cast<void*>(data);
}

}  // namespace json
