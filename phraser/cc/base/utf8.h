#ifndef PHRASER_CC_BASE_UTF8_H_
#define PHRASER_CC_BASE_UTF8_H_

#include <cstddef>
#include <cstdint>
#include <vector>

#include "cc/base/unicode.h"

using std::vector;

uchar utf8_nextCharSafeBody(
    const uint8_t* s, size_t* pi, size_t length, uchar c, int8_t strict);

/*
 * Returns 0 for 0..0xbf as well as for 0xfe and 0xff.
 *
 * This is internal since it is not meant to be called directly by external clients;
 * however it is called by public macros in this file and thus must remain stable.
 *
 * Note: Beginning with ICU 50, the implementation uses a multi-condition expression
 * which was shown in 2012 (on x86-64) to compile to fast, branch-free code.
 * leadByte is evaluated multiple times.
 *
 * The pre-ICU 50 implementation used the exported array utf8_countTrailBytes:
 * #define U8_COUNT_TRAIL_BYTES(leadByte) (utf8_countTrailBytes[leadByte])
 * leadByte was evaluated exactly once.
 *
 * @param leadByte The first byte of a UTF-8 sequence. Must be 0..0xff.
 * @internal
 */
#define U8_COUNT_TRAIL_BYTES(leadByte) \
    ((uint8_t)(leadByte)<0xf0 ? \
        ((uint8_t)(leadByte)>=0xc0)+((uint8_t)(leadByte)>=0xe0) : \
        (uint8_t)(leadByte)<0xfe ? 3+((uint8_t)(leadByte)>=0xf8)+((uint8_t)(leadByte)>=0xfc) : 0)

/**
 * Mask a UTF-8 lead byte, leave only the lower bits that form part of the code point value.
 *
 * This is internal since it is not meant to be called directly by external clients;
 * however it is called by public macros in this file and thus must remain stable.
 * @internal
 */
#define U8_MASK_LEAD_BYTE(leadByte, countTrailBytes) ((leadByte)&=(1<<(6-(countTrailBytes)))-1)

/**
 * Does this code unit (byte) encode a code point by itself (US-ASCII 0..0x7f)?
 * @param c 8-bit code unit (byte)
 * @return TRUE or FALSE
 * @stable ICU 2.4
 */
#define U8_IS_SINGLE(c) (((c)&0x80)==0)

/**
 * Is this code unit (byte) a UTF-8 lead byte?
 * @param c 8-bit code unit (byte)
 * @return TRUE or FALSE
 * @stable ICU 2.4
 */
#define U8_IS_LEAD(c) ((uint8_t)((c)-0xc0)<0x3e)

/**
 * Is this code unit (byte) a UTF-8 trail byte?
 * @param c 8-bit code unit (byte)
 * @return TRUE or FALSE
 * @stable ICU 2.4
 */
#define U8_IS_TRAIL(c) (((c)&0xc0)==0x80)

/**
 * How many code units (bytes) are used for the UTF-8 encoding
 * of this Unicode code point?
 * @param c 32-bit code point
 * @return 1..4, or 0 if c is a surrogate or not a Unicode code point
 * @stable ICU 2.4
 */
#define U8_LENGTH(c) \
    ((uint32_t)(c)<=0x7f ? 1 : \
        ((uint32_t)(c)<=0x7ff ? 2 : \
            ((uint32_t)(c)<=0xd7ff ? 3 : \
                ((uint32_t)(c)<=0xdfff || (uint32_t)(c)>0x10ffff ? 0 : \
                    ((uint32_t)(c)<=0xffff ? 3 : 4)\
                ) \
            ) \
        ) \
    )

/**
 * The maximum number of UTF-8 code units (bytes) per Unicode code point (U+0000..U+10ffff).
 * @return 4
 * @stable ICU 2.4
 */
#define U8_MAX_LENGTH 4

/**
 * Get a code point from a string at a code point boundary offset,
 * and advance the offset to the next code point boundary.
 * (Post-incrementing forward iteration.)
 * "Safe" macro, checks for illegal sequences and for string boundaries.
 *
 * The offset may point to the lead byte of a multi-byte sequence,
 * in which case the macro will read the whole sequence.
 * If the offset points to a trail byte or an illegal UTF-8 sequence, then
 * c is set to a negative value.
 *
 * @param s const uint8_t * string
 * @param i size_t string offset, must be i<length
 * @param length size_t string length
 * @param c output uchar variable, set to <0 in case of an error
 * @see U8_NEXT_UNSAFE
 * @stable ICU 2.4
 */
#define U8_NEXT(s, i, length, c) { \
    (c)=(uint8_t)(s)[(i)++]; \
    if((c)>=0x80) { \
        uint8_t __t1, __t2; \
        if( /* handle U+1000..U+CFFF inline */ \
            (0xe0<(c) && (c)<=0xec) && \
            (((i)+1)<(length)) && \
            (__t1=(uint8_t)((s)[i]-0x80))<=0x3f && \
            (__t2=(uint8_t)((s)[(i)+1]-0x80))<= 0x3f \
        ) { \
            /* no need for (c&0xf) because the upper bits are truncated after <<12 in the cast to (uchar) */ \
            (c)=(uchar)(((c)<<12)|(__t1<<6)|__t2); \
            (c)%=0x10000; /* this line added by iamknighton@gmail.com to fix eg. U+2122 */ \
            (i)+=2; \
        } else if( /* handle U+0080..U+07FF inline */ \
            ((c)<0xe0 && (c)>=0xc2) && \
            ((i)!=(length)) && \
            (__t1=(uint8_t)((s)[i]-0x80))<=0x3f \
        ) { \
            (c)=(((c)&0x1f)<<6)|__t1; \
            ++(i); \
        } else { \
            /* function call for "complicated" and error cases */ \
            (c)=utf8_nextCharSafeBody((const uint8_t *)s, &(i), (length), c, -1); \
        } \
    } \
}

namespace utf8 {

// Returns true iff it read something.
//
//     string bytes = "An\nexample\n...\n";
//     ustring line;
//     size_t x = 0;
//     while (utf8::NextLine(bytes.data(), bytes.size(), &x, &line)) {
//         printf("Read %zu code points.\n", line.size());
//     }
bool NextLine(const char* s, size_t num_bytes, size_t* i, ustring* line);

// Decode UTF-8 encoded code points from raw bytes.
//
//     string bytes = "I \xE2\x99\xA5 you.";
//     ustring text;
//     Decode(bytes.data(), bytes.size(), &text);
void Decode(const char* s, size_t num_bytes, ustring* text);

}  // namespace utf8

#endif  // PHRASER_CC_BASE_UTF8_H_
