#include "utf8.h"

#include <cassert>

/**
 * UTF8_ERROR_VALUE_1 and UTF8_ERROR_VALUE_2 are special error values for UTF-8,
 * which need 1 or 2 bytes in UTF-8:
 * \code
 * U+0015 = NAK = Negative Acknowledge, C0 control character
 * U+009f = highest C1 control character
 * \endcode
 *
 * These are used by UTF8_..._SAFE macros so that they can return an error value
 * that needs the same number of code units (bytes) as were seen by
 * a macro. They should be tested with UTF_IS_ERROR() or UTF_IS_VALID().
 *
 * @deprecated ICU 2.4. Obsolete, see utf_old.h.
 */
#define UTF8_ERROR_VALUE_1 0x15

/**
 * See documentation on UTF8_ERROR_VALUE_1 for details.
 *
 * @deprecated ICU 2.4. Obsolete, see utf_old.h.
 */
#define UTF8_ERROR_VALUE_2 0x9f

/**
 * Error value for all UTFs. This code point value will be set by macros with error
 * checking if an error is detected.
 *
 * @deprecated ICU 2.4. Obsolete, see utf_old.h.
 */
#define UTF_ERROR_VALUE 0xffff

/**
 * Is a given 32-bit code an error value
 * as returned by one of the macros for any UTF?
 *
 * @deprecated ICU 2.4. Obsolete, see utf_old.h.
 */
// #define UTF_IS_ERROR(c) \
//     (((c)&0xfffe)==0xfffe || (c)==UTF8_ERROR_VALUE_1 || (c)==UTF8_ERROR_VALUE_2)

#define U_SENTINEL (-1)

static const UChar
utf8_minLegal[4]={ 0, 0x80, 0x800, 0x10000 };

static const UChar
utf8_errorValue[6]={
    UTF8_ERROR_VALUE_1, UTF8_ERROR_VALUE_2, UTF_ERROR_VALUE, 0x10ffff,
    0x3ffffff, 0x7fffffff
};

static UChar
errorValue(size_t count, int8_t strict) {
    if(strict>=0) {
        return utf8_errorValue[count];
    } else if(strict==-3) {
        return 0xfffd;
    } else {
        return U_SENTINEL;
    }
}

/*
 * Handle the non-inline part of the U8_NEXT() and U8_NEXT_FFFD() macros
 * and their obsolete sibling UTF8_NEXT_CHAR_SAFE().
 *
 * The "strict" parameter controls the error behavior:
 * <0  "Safe" behavior of U8_NEXT():
 *     -1: All illegal byte sequences yield U_SENTINEL=-1.
 *     -2: Same as -1, except for lenient treatment of surrogate code points as legal.
 *         Some implementations use this for roundtripping of
 *         Unicode 16-bit strings that are not well-formed UTF-16, that is, they
 *         contain unpaired surrogates.
 *     -3: All illegal byte sequences yield U+FFFD.
 *  0  Obsolete "safe" behavior of UTF8_NEXT_CHAR_SAFE(..., FALSE):
 *     All illegal byte sequences yield a positive code point such that this
 *     result code point would be encoded with the same number of bytes as
 *     the illegal sequence.
 * >0  Obsolete "strict" behavior of UTF8_NEXT_CHAR_SAFE(..., TRUE):
 *     Same as the obsolete "safe" behavior, but non-characters are also treated
 *     like illegal sequences.
 *
 * Note that a UBool is the same as an int8_t.
 */
UChar utf8_nextCharSafeBody(const uint8_t *s, size_t *pi, size_t length,
                            UChar c, int8_t strict) {
    size_t i=*pi;
    uint8_t count=U8_COUNT_TRAIL_BYTES(c);
    assert(count <= 5); /* U8_COUNT_TRAIL_BYTES returns value 0...5 */
    if(i+count<=length) {
        uint8_t trail;

        U8_MASK_LEAD_BYTE(c, count);
        /* do not read beyond the first non-trail byte */
        switch(count) {
        /* each branch falls through to the next one */
        case 0:
            /* count==0 for illegally leading trail bytes and the illegal bytes 0xfe and 0xff */
        case 5:
        case 4:
            /* count>=4 is always illegal: no more than 3 trail bytes in Unicode's UTF-8 */
            break;
        case 3:
            trail=s[i++]-0x80;
            c=(c<<6)|trail;
            /* c>=0x110 would result in code point>0x10ffff, outside Unicode */
            if(c>=0x110 || trail>0x3f) { break; }
        [[clang::fallthrough]]; case 2:
            trail=s[i++]-0x80;
            c=(c<<6)|trail;
            /*
             * test for a surrogate d800..dfff unless we are lenient:
             * before the last (c<<6), a surrogate is c=360..37f
             */
            if(((c&0xffe0)==0x360 && strict!=-2) || trail>0x3f) { break; }
        [[clang::fallthrough]]; case 1:
            trail=s[i++]-0x80;
            c=(c<<6)|trail;
            if(trail>0x3f) { break; }
            /* correct sequence - all trail bytes have (b7..b6)==(10) */
            if(c>=utf8_minLegal[count] &&
                    /* strict: forbid non-characters like U+fffe */
                    (strict<=0 || !U_IS_UNICODE_NONCHAR(c))) {
                *pi=i;
                return c;
            }
        /* no default branch to optimize switch()  - all values are covered */
        }
    } else {
        /* too few bytes left */
        count=static_cast<uint8_t>(length-i);
    }

    /* error handling */
    i=*pi;
    while(count>0 && U8_IS_TRAIL(s[i])) {
        ++i;
        --count;
    }
    c=errorValue(i-*pi, strict);
    *pi=i;
    return c;
}

bool ReadLine(const char* s, size_t* i, size_t length, vector<UChar>* line) {
    line->clear();
    while (*i < length) {
        UChar c;
        U8_NEXT(s, *i, length, c);
        line->emplace_back(c);
        if (c == '\n') {
            break;
        }
    }

    return *i < length;
}
