#!/usr/bin/python

import codecs


INDENT = ' ' * 4


H_SRC = """
#ifndef CC_FRONTEND_DESTUTTER_DESTUTTERER_DATA_H_
#define CC_FRONTEND_DESTUTTER_DESTUTTERER_DATA_H_

#include <cstddef>
#include <cstdint>
#include <cstdlib>

namespace destutterer_data {

extern uint32_t DIGITS[];

extern size_t DIGITS_SIZE;

}  // namespace destutterer_data

#endif  // CC_FRONTEND_DESTUTTER_DESTUTTERER_DATA_H_
"""[1:]


CC_SRC = """
#include "destutterer_data.h"

namespace destutterer_data {

uint32_t DIGITS[] = {
%s
};

size_t DIGITS_SIZE = sizeof(DIGITS) / sizeof(DIGITS[0]);

}  // namespace destutterer_data
"""[1:]


def parse_code_point(s):
    assert s.startswith('U+')
    return int(s[2:], 16)


def get(file_name):
    code2name = {}
    with codecs.open(file_name, encoding='utf-8') as f:
        for line in f:
            ss = line.split()
            code = parse_code_point(ss[0])
            name = ' '.join(ss[1:-1])
            value = ss[-1]
            assert code not in code2name
            code2name[code] = name
    return code2name


def dump_h(file_name):
    with open(file_name, 'wb') as f:
        f.write(H_SRC)


def dump_cc(code2name, file_name):
    with open(file_name, 'wb') as f:
        lines = []
        for code in sorted(code2name):
            name = code2name[code]
            lines.append('%s0x%x,  // %s' % (INDENT, code, name))
        f.write(CC_SRC % '\n'.join(lines))


def main():
    dump_h('../destutterer_data.h')

    code2name = get('digits.txt')
    dump_cc(code2name, '../destutterer_data.cc')


if __name__ == '__main__':
    main()
