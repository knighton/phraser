#ifndef CC_PHRASER_ENGLISH_COMMON_H_
#define CC_PHRASER_ENGLISH_COMMON_H_

#include "cc/misc/enum_strings.h"

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
    P_INTR,
    P_INTR_EMPH,
    NUM_PERSONS,
};

extern EnumStrings<Person> PersonStrings;

enum Personhood {
    PH_NO,
    PH_YES,
    NUM_PERSONHOODS,
};

extern EnumStrings<Personhood> PersonhoodStrings;

enum Gender {
    G_MALE,
    G_FEMALE,
    G_NEUTER,
    NUM_GENDERS,
};

extern EnumStrings<Gender> GenderStrings;

#endif  // CC_PHRASER_ENGLISH_COMMON_H_
