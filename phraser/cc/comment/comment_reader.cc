#include "comment_reader.h"

#include <string>
#include <vector>

#include "cc/comment/comment.h"
#include "cc/base/files.h"
#include "cc/base/utf8.h"

using std::string;
using std::vector;

bool CommentReader::Init(const vector<string>& comment_ff) {
    comment_ff_ = comment_ff;
    bytes_.clear();
    file_index_ = 0;
    byte_index_ = 0;
    return true;
}

static bool ParseLine(const ustring& line, Comment* comment, string* error) {
    size_t count = 0;
    size_t x = 0;
    uchar c;
    bool found = false;
    while (x < line.size() && (c = line[x++]) != ' ') {
        found = true;
        if (!('0' <= c && c <= '9')) {
            *error = "Not a digit.";
            return false;
        }
        count += c - '0';
    }
    if (!found) {
        *error = "Number of tags is missing.";
        return false;
    }

    vector<string> tags;
    for (auto i = 0; i < count; ++i) {
        while (x < line.size() && (c = line[x]) == ' ') {
            ++x;
        }

        string tag;
        while (x < line.size() && (c = line[x++]) != ' ') {
            if (!('a' <= c && c <= 'z') && c != '_') {
                *error = "Not a letter.";
                return false;
            }
            tag += static_cast<char>(c);
        }
        tags.emplace_back(tag);
    }
    if (tags.size() != count) {
        *error = "Did not read all tags.";
        return false;
    }

    while (x < line.size() && (c = line[x]) == ' ') {
        ++x;
    }

    ustring text;
    text.reserve(line.size() - x);
    while (x < line.size()) {
        text.emplace_back(line[x]);
        ++x;
    }

    comment->Init(text, tags);
    return true;
}

CommentReaderStatus CommentReader::Next(
        bool dump_file_names, Comment* comment, string* error) {
    // If byte index is zero, load the file.
    if (!byte_index_) {
        if (file_index_ < comment_ff_.size()) {
            auto& f = comment_ff_[file_index_];
            if (dump_file_names) {
                printf("About to start extracting comments from [%s].\n",
                       f.data());
            }
            if (!files::FileToString(f, &bytes_)) {
                *error = "Could not read file: [" + f + "]";
                return CRS_ERROR;
            }
        } else {
            return CRS_OUT_OF_COMMENTS;
        }
    }

    // Try to load the next line.  If nothing, reset to the next file.
    ustring line;
    if (!utf8::NextLine(bytes_.data(), bytes_.size(), &byte_index_, &line)) {
        ++file_index_;
        byte_index_ = 0;
        return Next(dump_file_names, comment, error);
    }

    if (!ParseLine(line, comment, error)) {
        return CRS_ERROR;
    }

    return CRS_HAVE_COMMENT;
}
