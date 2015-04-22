#ifndef CC_BASE_MISC_STRING_TRANSFORM_H_
#define CC_BASE_MISC_STRING_TRANSFORM_H_

#include <string>

using std::string;

class StringTransform {
  public:
    void Init(const string& append);

    void Apply(const string& in, string* out);

  private:
    string append_;
};

#endif  // CC_BASE_MISC_STRING_TRANSFORM_H_

