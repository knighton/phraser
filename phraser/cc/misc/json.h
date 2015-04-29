#ifndef CC_MISC_JSON_H_
#define CC_MISC_JSON_H_

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

    Object(const vector<uint32_t>& v);
    Object(const set<uint32_t>& set);
    Object(const unordered_set<uint32_t>& set);

    Object(const vector<string>& v);
    Object(const set<string>& set);
    Object(const unordered_set<string>& set);

    Object(const vector<Object*>& v);

    Object(const map<string, Object*>& d);

    ~Object();

  private:
    void InitFromInts(const vector<int64_t>& v);

    void InitFromStrings(const vector<string>& v);

    JSONObjectType type_;
    void* data_;
};

}  // namespace json

#endif  // CC_MISC_JSON_H_
