#include "common.h"

EnumStrings<Number> NumberStrings = EnumStrings<Number>("sing plur");

EnumStrings<Person> PersonStrings =
    EnumStrings<Person>("1st 2nd 3rd intr intremph");

EnumStrings<Personhood> PersonhoodStrings =
    EnumStrings<Personhood>("thing person");

EnumStrings<Gender> GenderStrings = EnumStrings<Gender>("male female neuter");
