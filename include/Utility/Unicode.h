#ifndef UNICODE_H
#define UNICODE_H

#include <Utility/Standard.h>

enum General_Categories
{
    General_Categories_Lu = 0,
    General_Categories_Ll = 1,
    General_Categories_Lt = 2,
    General_Categories_Lm = 3,
    General_Categories_Lo = 4,
    General_Categories_Mn = 5,
    General_Categories_Mc = 6,
    General_Categories_Me = 7,
    General_Categories_Nd = 8,
    General_Categories_Nl = 9,
    General_Categories_No = 10,
    General_Categories_Pc = 11,
    General_Categories_Pd = 12,
    General_Categories_Ps = 13,
    General_Categories_Pe = 14,
    General_Categories_Pi = 15,
    General_Categories_Pf = 16,
    General_Categories_Po = 17,
    General_Categories_Sm = 18,
    General_Categories_Sc = 19,
    General_Categories_Sk = 20,
    General_Categories_So = 21,
    General_Categories_Zs = 22,
    General_Categories_Zl = 23,
    General_Categories_Zp = 24,
    General_Categories_Cc = 25,
    General_Categories_Cf = 26,
    General_Categories_Cs = 27,
    General_Categories_Co = 28,
    General_Categories_Cn = 29,
};

enum Numeric_Type
{
    Numeric_Type_None = 0,
    Numeric_Type_De = 1,
    Numeric_Type_Di = 2,
    Numeric_Type_Nu = 3
};

typedef struct Unicode
{
    char general_categories;
    char xid_start;
    char xid_continue;
    char numeric_type;
    char numeric_value;
} Unicode;

extern const Unicode UCD[1114112];

#endif
