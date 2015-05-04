#ifndef CC_COMMENT_COMMENT_H_
#define CC_COMMENT_COMMENT_H_

#include <string>
#include <vector>

#include "cc/misc/unicode.h"

using std::string;
using std::vector;

class Comment {
  public:
    const ustring& text() const { return text_; }
    const vector<string>& tags() const { return tags_; }

    void Init(const ustring& text, const vector<string>& tags);

  private:
    ustring text_;
    vector<string> tags_;
};

#endif  // CC_COMMENT_COMMENT_H_
