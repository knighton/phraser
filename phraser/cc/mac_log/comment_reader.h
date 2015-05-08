#ifndef CC_MAC_LOG_COMMENT_READER_H_
#define CC_MAC_LOG_COMMENT_READER_H_

#include <string>
#include <vector>

#include "cc/mac_log/comment.h"

using std::string;
using std::vector;

enum CommentReaderStatus {
    CRS_HAVE_COMMENT,
    CRS_OUT_OF_COMMENTS,
    CRS_ERROR,
};

class CommentReader {
  public:
    CommentReader() : file_index_(0), byte_index_(0) {}

    bool Init(const vector<string>& comment_ff);

    CommentReaderStatus Next(Comment* comment, string* error);

  private:
    vector<string> comment_ff_;
    string bytes_;
    size_t file_index_;
    size_t byte_index_;
};

#endif  // CC_MAC_LOG_COMMENT_READER_H_
