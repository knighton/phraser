#ifndef PHRASER_CC_BASE_ENUM_STRINGS_H_
#define PHRASER_CC_BASE_ENUM_STRINGS_H_

// Usage:
//
// some_file.h:
//
//     | #include "cc/base/misc/enum_strings.h"
//     |
//     | enum Color {
//     |     COLOR_RED,
//     |     COLOR_GREEN,
//     |     COLOR_BLUE,
//     | };
//     |
//     | extern EnumStrings<Color> ColorStrings;
//
// some_file.cc:
//
//     | #include "some_file.h"
//     |
//     | EnumStrings<Color> ColorStrings = EnumStrings<Color("RED GREEN BLUE");
//
// main.cc:
//
//     | #include <cstdio>
//     |
//     | #include "some_file.h"
//     |
//     | int main() {
//     |     printf("%s = %d = %d\n", ColorStrings.GetString(COLOR_RED).c_str(),
//     |            COLOR_RED, ColorStrings.GetEnumValue("RED"));
//     | }

#include <map>
#include <string>
#include <vector>

using std::map;
using std::string;
using std::vector;

template <typename E>
vector<E> EnumRange(E a, E z_excl);

template <typename E>
class EnumStrings {
  public:
    const vector<E>& enum_values() const { return enum_values_; }
    const map<string, E>& string2enum_value() const {
        return string2enum_value_;
    }
    const vector<string>& strings() const { return strings_; }

    const vector<E>& enum_values_except_last() const {
            return enum_values_except_last_;
    }
    const map<string, E>& string_except_last2enum_value() const {
            return string_except_last2enum_value_;
    }
    const vector<string>& strings_except_last() const {
            return strings_except_last_;
    }

    EnumStrings(const string& s);

    const string& GetString(E e) const;
    bool MaybeGetString(E e, string* s) const;

    E GetEnumValue(const string& s) const;
    bool MaybeGetEnumValue(const string& s, E* e) const;

  private:
    vector<E> enum_values_;
    map<string, E> string2enum_value_;
    vector<string> strings_;

    // Sometimes you want to have a final wildcard enum value.
    vector<E> enum_values_except_last_;
    map<string, E> string_except_last2enum_value_;
    vector<string> strings_except_last_;
};

#include "enum_strings_impl.h"

#endif  // PHRASER_CC_BASE_ENUM_STRINGS_H_
