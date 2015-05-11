#ifndef CC_FRONTEND_SBD_SENTENCE_SPLITTER_H_
#define CC_FRONTEND_SBD_SENTENCE_SPLITTER_H_

#include <boost/regex.hpp>
#include <string>
#include <unordered_set>

using boost::regex;
using std::string;
using std::unordered_set;

class SentenceSplitter {
  public:
    void Init();
    void Split(const string& in, string* out) const;

  private:
    regex ellipsis_re_;
    regex url_re_;
    unordered_set<string> abbrevs_;
};

#endif  // CC_FRONTEND_SBD_SENTENCE_SPLITTER_H_
