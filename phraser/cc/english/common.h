#ifndef CC_EN_SPECIAL_TOKENS_COMMON_H_
#define CC_EN_SPECIAL_TOKENS_COMMON_H_

#include "cc/base/misc/enum_strings.h"

enum Number {
    N_SING,
    N_PLUR,
    NUM_NUMBERS,
};

extern EnumStrings<Number> NumberStrings;

enum Person {
    P_FIRST,
    P_SECOND,
    P_THIRD,
    NUM_PERSONS,
};

extern EnumStrings<Person> PersonStrings;

#endif  // CC_EN_SPECIAL_TOKENS_COMMON_H_
