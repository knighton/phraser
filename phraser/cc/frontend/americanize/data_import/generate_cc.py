#!/usr/bin/python

# Generate tokenization-related normalization data as C++ source code.
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


def load_americanize(f):
    token2token = {}
    for line in each_line(f):
        a, b = line.split()
        a = str(a)
        b = str(b)
        assert a not in token2token
        token2token[a] = b
    return token2token


H_SRC = """
#ifndef CC_FRONTEND_AMERICANIZE_AMERICANIZER_DATA_H_
#define CC_FRONTEND_AMERICANIZE_AMERICANIZER_DATA_H_

#include <string>
#include <unordered_map>

using std::string;
using std::unordered_map;

// Automatically generated file -- do not edit!

namespace americanizer_data {

extern string S2Z_LIST;
extern string TOKEN_PAIRS;

}  // namespace americanizer_data

#endif  // CC_FRONTEND_AMERICANIZE_AMERICANIZER_DATA_H_
"""[1:]


def dump_cc(americanize_s2z):
    cc_lines = []

    cc_lines.append('#include "americanizer_data.h"')
    cc_lines.append('')

    cc_lines.append('// Automatically generated file -- do not edit!')
    cc_lines.append('')

    cc_lines.append('namespace americanizer_data {')
    cc_lines.append('')

    token2token = {}
    for s, z in americanize_s2z.iteritems():
        assert s not in token2token
        token2token[s] = z

    s2z = set()
    pairs = []
    for from_s, to_s in token2token.iteritems():
        assert ' ' not in from_s
        assert ' ' not in to_s
        if from_s.replace('s', 'z') == to_s:
            s2z.add(from_s)
        else:
            pairs.append((from_s, to_s))

    cc_lines.append('string S2Z_LIST =')
    for s in sorted(s2z):
        cc_lines.append('    "%s "' % s.replace('"', '\"'))
    cc_lines.append(';')
    cc_lines.append('')

    cc_lines.append('string TOKEN_PAIRS =')
    for from_s, to_s in sorted(pairs):
        cc_lines.append(
            '    "%s %s "' %
                (from_s.replace('"', '\\"'), to_s.replace('"', '\\"')))
    cc_lines.append(';')
    cc_lines.append('')

    cc_lines.append('}  // namespace americanizer_data')

    cc_source = ''.join(map(lambda s: s + '\n', cc_lines))

    return cc_source


def main():
    americanize_s2z = load_americanize('americanize.txt')
    cc_src = dump_cc(americanize_s2z)
    open('../americanizer_data.h', 'wb').write(H_SRC)
    open('../americanizer_data.cc', 'wb').write(cc_src)


if __name__ == '__main__':
    main()
