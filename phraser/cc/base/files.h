#ifndef CC_BASE_FILES_H_
#define CC_BASE_FILES_H_

#include <fstream>
#include <string>
#include <vector>

using std::ifstream;
using std::string;
using std::vector;

namespace files {

// Returns true iff it got a line.
bool EachCommentableLine(ifstream* in, string* line);

// Whether the path is a file.
bool IsFile(const string& file_name);

// Read the entire file to a string.
bool FileToString(const string& file_name, string* text);

// Write the entire string to a file.
bool StringToFile(const string& text, const string& file_name);

// List a directory.
bool ListDir(const string& dir_name, vector<string>* names);

}  // namespace files

#endif  // CC_BASE_FILES_H_
