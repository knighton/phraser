#ifndef CC_BASE_JSON_H_
#define CC_BASE_JSON_H_

#include <map>
#include <set>
#include <string>
#include <unordered_set>
#include <vector>

using std::map;
using std::set;
using std::string;
using std::unordered_set;
using std::vector;

namespace json {

enum JSONObjectType {
    JOT_NULL,
    JOT_BOOL,
    JOT_INT,  // int64_t.
    JOT_STR,
    JOT_ARRAY,
    JOT_OBJ,
};

class Object {
  public:
    Object();  // NULL constructor.

    Object(int64_t n);

    Object(const string& s);

    Object(const vector<uint16_t>& v);
    Object(const set<uint16_t>& set);
    Object(const unordered_set<uint16_t>& set);

    Object(const vector<uint32_t>& v);
    Object(const set<uint32_t>& set);
    Object(const unordered_set<uint32_t>& set);

    Object(const vector<uint64_t>& v);
    Object(const set<uint64_t>& set);
    Object(const unordered_set<uint64_t>& set);

    Object(const vector<string>& v);
    Object(const set<string>& set);
    Object(const unordered_set<string>& set);

    Object(const vector<Object*>& v);

    Object(const map<string, Object*>& d);

    ~Object();

    void AppendToString(string* s, size_t spaces_per_indent=4,
                        size_t offset_spaces=0, bool leading_indent=true) const;

  private:
    void InitFromInts(const vector<int64_t>& v);

    void InitFromStrings(const vector<string>& v);

    JSONObjectType type_;
    void* data_;
};

}  // namespace json

#endif  // CC_BASE_JSON_H_
