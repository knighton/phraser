#ifndef LAPOS_STRING_STORE_H_
#define LAPOS_STRING_STORE_H_

#include <vector>
#include <string>
#include <iostream>

// This is an implementation of the string hashing method described
// in the following paper.
//
// N. Askitis and J. Zobel, Cache-conscious Collision Resolution in
// String Hash Tables, Proc. of String Processing and Information
// Retrieval Symp., Springer-Verlag, pp. 92--104, 2006.

class StringStore {
  public:
    // enum { LOAD_FACTOR = 1 };  // expand the hash table when num_terms ==
    // table_size.
    enum { LOAD_FACTOR = 2 };
    //  enum { LOAD_FACTOR = 4 };  // a bit slower, but more memory efficient.

    StringStore(const int n = 1);

    ~StringStore();

    int Put(const std::string& s);

    int Id(const std::string& s) const;

    void Clear(const int n = 1);

    size_t Size() const {
        return _num_terms;
    }

    struct const_iterator {
        const StringStore* obj;
        int idx;
        const unsigned char* base;

        const_iterator(const StringStore* o, int i, const unsigned char* b);

        const_iterator& operator++(int);

        std::string getStr() const;

        int getId() const;
    };

    const_iterator begin() const;
    const_iterator end() const;

  private:
    size_t _num_terms;
    size_t _min_idx;
    size_t _max_idx;
    std::vector<unsigned char*> _v;

    enum {
        EXTRA_DATA_SIZE = 5
    };  // length of the string (1 byte) + int (4 bytes)

    size_t hash_func(const std::string& s) const;

    void StoreStrInt(const std::string& s, const unsigned int val,
                     unsigned char* base);

    int GetInt(const unsigned char* base) const;

    const unsigned char* ScanStr(const unsigned char* base,
                                 const std::string& s) const;

    int Insert(const std::string& s, const int val);

    void Expand();
};

bool operator!=(const StringStore::const_iterator& x,
                const StringStore::const_iterator& y);

#endif  // LAPOS_STRING_STORE_H_
