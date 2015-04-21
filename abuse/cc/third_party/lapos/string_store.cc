#include "string_store.h"

#include <cassert>
#include <vector>
#include <string>
#include <iostream>

StringStore::StringStore(const int n) {
    Clear(n);
}

StringStore::~StringStore() {
    for (size_t i = 0; i < _v.size(); i++)
        free(_v[i]);
}

int StringStore::Put(const std::string& s) {
    assert(s.size() > 0 && s.size() < 256);
    return Insert(s, _num_terms);
}

int StringStore::Id(const std::string& s) const {
    assert(s.size() > 0 && s.size() < 256);
    const size_t idx = hash_func(s) % _v.size();
    const unsigned char* base = _v[idx];
    if (base != NULL) {
        base = ScanStr(base, s);
        if (*base)
            return GetInt(base);
    }
    return -1;
}

void StringStore::Clear(const int n) {
    for (size_t i = 0; i < _v.size(); i++)
        free(_v[i]);
    _v.resize(n);
    for (size_t i = 0; i < _v.size(); i++)
        _v[i] = NULL;
    _num_terms = 0;
    _min_idx = _v.size();
    _max_idx = 0;
}

StringStore::const_iterator::const_iterator(const StringStore* o, int i,
                                            const unsigned char* b)
        : obj(o), idx(i), base(b) {
}

StringStore::const_iterator& StringStore::const_iterator::operator++(int) {
    base += *base + EXTRA_DATA_SIZE;
    if (*base)
        return *this;
    for (int i = idx + 1; i <= (int)(obj->_max_idx); i++) {
        if (obj->_v[i]) {
            idx = i;
            base = obj->_v[i];
            return *this;
        }
    }
    idx = -1;
    base = NULL;
    return *this;
}

std::string StringStore::const_iterator::getStr() const {
    return std::string((char*)(base + 1), (*base));
}

int StringStore::const_iterator::getId() const {
    return obj->GetInt(base);
}

StringStore::const_iterator StringStore::begin() const {
    return const_iterator(this, _min_idx, _v[_min_idx]);
}

StringStore::const_iterator StringStore::end() const {
    return const_iterator(this, -1, NULL);
}

size_t StringStore::hash_func(const std::string& s) const {
    unsigned int h = 5;
    for (size_t i = 0; i < s.size(); i++) {
        h ^= ((h << 5) + (h >> 2) + s[i]);
    }
    return h;
}

void StringStore::StoreStrInt(const std::string& s, const unsigned int val,
                              unsigned char* base) {
    *(base++) = (unsigned char)s.size();
    for (size_t i = 0; i < s.size(); i++)
        *(base++) = s[i];
    *(base++) = (unsigned char)(val);
    *(base++) = (unsigned char)(val >> 8);
    *(base++) = (unsigned char)(val >> 16);
    *(base++) = (unsigned char)(val >> 24);
    *base = 0;
}

int StringStore::GetInt(const unsigned char* base) const {
    assert(base != NULL);
    base += *base + 4;
    unsigned int val = 0;
    val |= *(base--);
    val = val << 8;
    val |= *(base--);
    val = val << 8;
    val |= *(base--);
    val = val << 8;
    val |= *base;
    return val;
}

const unsigned char* StringStore::ScanStr(const unsigned char* base,
                                          const std::string& s) const {
    assert(base != NULL);
    while (*base != 0) {
        bool is_same = false;
        if (*base == s.size()) {
            is_same = true;
            for (size_t i = 0; i < s.size(); i++) {
                if (base[i + 1] != s[i]) {
                    is_same = false;
                    break;
                }
            }
        }
        if (is_same)
            return base;
        base += *base + EXTRA_DATA_SIZE;
    }
    return base;
}

int StringStore::Insert(const std::string& s, const int val) {
    assert(s.size() < 256);
    const size_t idx = hash_func(s) % _v.size();
    const unsigned char* base = _v[idx];
    unsigned int current_size = 0;
    if (base != NULL) {
        base = ScanStr(base, s);
        if (*base)
            return GetInt(base);  /////////// doesn't store the value
        current_size = base - _v[idx];
    }
    const size_t new_size = current_size + s.size() + EXTRA_DATA_SIZE + 1;
    //    const static size_t blocksize = 8;
    // const size_t new_size = (((current_size + s.size() + EXTRA_DATA_SIZE
    // + 1) / blocksize) + 1) * blocksize;
    unsigned char* p = (unsigned char*)realloc(_v[idx], new_size);
    if (p == NULL) {
        std::cerr << "error: realloc() failed." << std::endl;
        exit(1);
    }
    _v[idx] = p;
    StoreStrInt(s, val, p + current_size);
    _num_terms++;
    _min_idx = std::min(_min_idx, idx);
    _max_idx = std::max(_max_idx, idx);

    //    if (_num_terms > _v.size() * 900 / 1024) this->Expand();
    if (_num_terms > _v.size() * LOAD_FACTOR)
        this->Expand();

    return _num_terms - 1;
}

void StringStore::Expand() {
    StringStore c(_v.size() * 2);
    //    StringStore c(_v.size() * 3 / 2 + 1);

    for (size_t i = _min_idx; i <= _max_idx; i++) {
        if (_v[i] == NULL)
            continue;
        const unsigned char* base = _v[i];
        while (*base) {
            c.Insert(std::string((char*)(base + 1), (*base)), GetInt(base));
            base += *base + EXTRA_DATA_SIZE;
        }
        free(_v[i]);
    }

    _v = c._v;
    _min_idx = c._min_idx;
    _max_idx = c._max_idx;

    c._v.resize(0);
}

bool operator!=(const StringStore::const_iterator& x,
                const StringStore::const_iterator& y) {
    if (x.idx == y.idx && x.base == y.base)
        return false;
    return true;
}
