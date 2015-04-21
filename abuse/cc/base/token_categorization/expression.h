#ifndef CC_EN_TOKEN_CATEGORIZATION_EXPRESSION_H_
#define CC_EN_TOKEN_CATEGORIZATION_EXPRESSION_H_

#include <string>
#include <vector>
#include <unordered_map>

using std::string;
using std::vector;
using std::unordered_map;

class Expression {
  public:
    const string& type() const { return type_; }
    const vector<string>& args() const { return args_; }
    const unordered_map<string, vector<string>>& dimension2filters() const {
        return dimension2filters_;
    }

    void Init(const string& type, const vector<string>& args,
              const unordered_map<string, vector<string>>& dimension2filters);

    void ToCanonicalString(string* s) const;

  private:
    string type_;
    vector<string> args_;
    unordered_map<string, vector<string>> dimension2filters_;
};

#endif  // CC_EN_TOKEN_CATEGORIZATION_EXPRESSION_H_
