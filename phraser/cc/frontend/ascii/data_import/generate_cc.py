#!/usr/bin/python

# Generate Unicode-to-ASCII normalization data as C++ source code.
#
# We have to do this custom because of specific requirements to match the Penn
# Treebank training data.

import codecs
import re


def each_line(file_name, encoding='utf-8'):
    with codecs.open(file_name, encoding=encoding) as f:
        for line in f:
            x = line.find('#')
            if x != -1:
                line = line[:x]
            line = line.rstrip()
            if not line:
                continue
            if len(line) == 1 and ord(line[0]) == 65279:
                continue
            yield line


U_CODE_POINT_RE = re.compile('^U\\+[0-9A-F]{4,5}$')


def parse_u_code_point(s):
    assert U_CODE_POINT_RE.match(s)
    s = s[2:]
    return int(s, 16)


CODE_POINT_RE = re.compile('^[0-9A-F]{4,5}$')


def parse_code_point(s):
    assert CODE_POINT_RE.match(s)
    return int(s, 16)


def load_quotes(f):
    code2code = {}
    for line in each_line(f):
        from_s, to_s = line.split()
        from_c = parse_u_code_point(from_s)
        to_c = parse_u_code_point(to_s)
        assert from_c not in code2code
        code2code[from_c] = to_c
    return code2code


def load_currencies(f):
    code2codes = {}
    for line in each_line(f):
        from_s, to_s = line.split()
        from_c = parse_u_code_point(from_s)
        assert from_c not in code2codes
        to_s = ' ' + to_s + ' '
        code2codes[from_c] = map(ord, to_s)
    return code2codes


def load_ptb_smart_quotes(f):
    code2s = {}
    for line in each_line(f):
        from_s, to_s = line.split()
        from_c = parse_u_code_point(from_s)
        to_s = ' ' + to_s + ' '
        assert from_s not in code2s
        code2s[from_c] = map(ord, to_s)
    return code2s


def load_confusables(f):
    code2codes = {}
    for line in each_line(f):
        from_s, to_s, category = line.split(';')

        category = category.strip()
        if category != 'SA':
            continue

        from_c = parse_code_point(from_s.strip())
        to_cc = map(parse_code_point, to_s.split())

        # If it is not ASCII but maps to ASCII, keep it.
        keep = False
        if from_c < 128:
            continue

        to_cc = filter(lambda c: c < 128, to_cc)
        if not to_cc:
            continue

        assert from_c not in code2codes
        code2codes[from_c] = to_cc
    return code2codes


def load_unicode_code_point_names(f):
    code2name = {}
    for line in each_line(f):
        ss = line.split(';')
        if len(ss) < 2:
            continue
        code_s, name = ss[:2]
        code = int(code_s, 16)
        assert code not in code2name
        code2name[code] = name.title()
    return code2name


def load_nfc(f):
    code2codes = {}
    for line in each_line(f):
        ss = line.replace('>', '=').split('=')
        if len(ss) != 2:
            continue

        from_s, to_s = ss
        from_c = parse_code_point(from_s)
        to_cc = map(parse_code_point, to_s.split())
        assert from_c not in code2codes
        code2codes[from_c] = to_cc
    return code2codes


def load_nfkc(f):
    code2codes = {}
    for line in each_line(f):
        from_s, to_s = line.split('>')
        from_c = parse_code_point(from_s)
        to_cc = map(parse_code_point, to_s.split())
        assert from_c not in code2codes
        code2codes[from_c] = to_cc
    return code2codes


def load_decompositions(nfc_f, nfkc_f):
    nfd_code2codes = load_nfc(nfc_f)
    nfkd_code2codes = load_nfkc(nfkc_f)

    decomp_code2codes = {}
    for code, codes in nfd_code2codes.iteritems():
        decomp_code2codes[code] = codes
        other_codes = nfkd_code2codes.get(code)
        if other_codes:
            assert codes == other_codes
    for code, codes in nfkd_code2codes.iteritems():
        if code in decomp_code2codes:
            continue
        decomp_code2codes[code] = codes
    return decomp_code2codes


H_SOURCE = """
#ifndef CC_FRONTEND_ASCII_PTB_ASCII_NORMALIZER_DATA_H_
#define CC_FRONTEND_ASCII_PTB_ASCII_NORMALIZER_DATA_H_

#include <string>
#include <unordered_map>

using std::string;
using std::unordered_map;

// Automatically generated file -- do not edit!

namespace ptb_ascii_normalizer_data {

// Block containing newline-separated (as space is a valid character) strings
// concatenated together.
extern string ASCII_DATA;

// Unicode -> PTB ASCII (index into ASCII_DATA).
//
// If not present, maps to whitespace (0x20).
//
// How this mapping was created:
// 1. If nonprintable ASCII, normalize to a space.
// 2. Normalize the various open/close quotes to smart quotes.
// 3. Convert currency symbols to dollars and cents (to match PTB training
//    data).
// 4. Convert smart quotes to PTB tokens.
// 5. Decompose the code points according to NFKD.
// 6. De-confuse the resulting code points according to the "SA" confusables
//    table, excluding ones that are already ASCII.
// 7. Drop non-ASCII.
// 8. Condense results that are all spaces to a single space.
// 9. Drop parenthesized characters that don't map to Latin.
extern unordered_map<uint32_t, uint16_t> UNICODE2ASCII;

}  // namespace ptb_ascii_normalizer_data

#endif  // CC_FRONTEND_ASCII_PTB_ASCII_NORMALIZER_DATA_H_
"""[1:]


class TokenizerData(object):
    def __init__(
            self, normalize_quotes_code2code,
            dollarize_code2codes, smart_quotes_code2codes, unicode_code2name,
            decomp_code2codes, confusable_code2codes):
        self.normalize_quotes_code2code = normalize_quotes_code2code
        self.dollarize_code2codes = dollarize_code2codes
        self.smart_quotes_code2codes = smart_quotes_code2codes
        self.unicode_code2name = unicode_code2name
        self.decomp_code2codes = decomp_code2codes
        self.confusable_code2codes = confusable_code2codes

    def get_string(self, original_code):
        # Normalize nonprintable ASCII to spaces.
        if original_code < 0x20 or original_code == 0x7E:
            return ' '

        # Normalize quotes to smart quotes where we can.
        code = self.normalize_quotes_code2code.get(original_code)
        if code is None:
            code = original_code

        # Convert euros to dollars.
        codes = self.dollarize_code2codes.get(code)
        if codes is None:
            codes = [code]

        # Convert smart quotes to PTB ASCII tokens.
        new_codes = []
        for code in codes:
            r = self.smart_quotes_code2codes.get(code)
            if r is None:
                new_codes.append(code)
            else:
                new_codes += r
        codes = new_codes

        # Decompose the characters.
        new_codes = []
        for code in codes:
            r = self.decomp_code2codes.get(code)
            if r is None:
                new_codes.append(code)
            else:
                new_codes += r
        codes = new_codes

        # De-confuse the characters except " quotes.
        new_codes = []
        for code in codes:
            if code < 128:
                new_codes.append(code)
                continue

            r = self.confusable_code2codes.get(code)
            if r is None:
                new_codes.append(code)
            else:
                new_codes += r

        # Drop non-ASCII.
        codes = filter(lambda code: code < 128, codes)

        # Join into an ASCII string.
        s = ''.join(map(chr, codes))

        # Condense spaces.
        is_multispace = True
        for c in s:
            if c != ' ':
                is_multispace = False
                break
        if is_multispace:
            s = ' '

        # Parenthesized characters that don't map to Latin.
        if s == '()':
            return ' '

        return s

    def generate_cpp(self):
        codes = set()
        for code in self.normalize_quotes_code2code:
            codes.add(code)
        for code in self.dollarize_code2codes:
            codes.add(code)
        for code in self.smart_quotes_code2codes:
            codes.add(code)
        for code in self.decomp_code2codes:
            codes.add(code)
        for code in self.confusable_code2codes:
            codes.add(code)

        code2s = {}
        for code in sorted(codes):
            s = self.get_string(code)
            if s.isspace():
                continue
            code2s[code] = s

        ss_set = set()
        for s in code2s.itervalues():
            ss_set.add(s)

        ss_set.discard('')

        ss = sorted(ss_set)

        s2index = {}
        index = 0
        for s in ss:
            s2index[s] = index
            index += len(s) + 1

        pairs = []
        for code in sorted(code2s):
            s = code2s[code]
            if not s:
                continue
            index = s2index[s]
            pairs.append((code, index))

        cc_lines = []

        cc_lines.append('#include "ptb_ascii_normalizer_data.h"')
        cc_lines.append('')

        cc_lines.append('// Automatically generated file -- do not edit!')
        cc_lines.append('')

        cc_lines.append('namespace ptb_ascii_normalizer_data {')
        cc_lines.append('')

        cc_lines.append('string ASCII_DATA = ')
        for i, s in enumerate(ss):
            index = s2index[s]
            s = s.replace('"', '\\"')
            line = '    "%s\\n"  // %d' % (s, index)
            cc_lines.append(line)
        cc_lines.append(';')
        cc_lines.append('')

        cc_lines.append('unordered_map<uint32_t, uint16_t> UNICODE2ASCII = {')
        for code, index in pairs:
            name = self.unicode_code2name.get(code, '(???)')
            s = self.get_string(code).replace('\\', '(backslash)')
            line = '    {0x%05x, %d},  // %s -> %s' % (code, index, name, s)
            cc_lines.append(line)
        cc_lines.append('};')
        cc_lines.append('')

        cc_lines.append('}  // namespace ptb_ascii_normalizer_data')

        cc_lines.append('')
        cc_source = '\n'.join(cc_lines)

        return H_SOURCE, cc_source


def main():
    # Int -> int.
    normalize_quotes_code2code = load_quotes('quotes.txt')

    # Int -> int list.
    dollarize_code2codes = load_currencies('currencies.txt')

    # Int -> int list.
    smart_quotes_code2codes = load_ptb_smart_quotes('ptb_smart_quotes.txt')

    # Int -> string.
    unicode_code2name = load_unicode_code_point_names('unicode_data.txt')

    # Int -> int list.
    decomp_code2codes = load_decompositions('nfc.txt', 'nfkc.txt')

    # Int -> int list.
    confusable_code2codes = load_confusables('confusables.txt')

    tok = TokenizerData(
        normalize_quotes_code2code, dollarize_code2codes,
        smart_quotes_code2codes, unicode_code2name, decomp_code2codes,
        confusable_code2codes)
    h, cc = tok.generate_cpp()
    open('../ptb_ascii_normalizer_data.h', 'wb').write(h)
    open('../ptb_ascii_normalizer_data.cc', 'wb').write(cc)


if __name__ == '__main__':
    main()
