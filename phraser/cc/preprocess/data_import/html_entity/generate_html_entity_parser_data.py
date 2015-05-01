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


U_CODE_POINT_RE = re.compile('^U\\+[0-9A-F]{4,5}$')


def parse_u_code_point(s):
    assert U_CODE_POINT_RE.match(s)
    s = s[2:]
    return int(s, 16)


HTML_ENTITY_RE = re.compile('^&[A-Za-z0-9]+;$')


def load_html_entities(f):
    name2code = {}
    for line in each_line(f):
        ss = line.split()
        code = parse_u_code_point(ss[0])
        for s in ss[1:]:
            assert HTML_ENTITY_RE.match(s)
            name = s[1:-1]
            name = str(name)
            assert name not in name2code
            name2code[name] = code
    return name2code


H_SOURCE = """
#ifndef CC_PREPROCESS_HTML_ENTITY_PARSER_DATA_H_
#define CC_PREPROCESS_HTML_ENTITY_PARSER_DATA_H_

#include <string>
#include <unordered_map>

using std::string;
using std::unordered_map;

// Automatically generated file -- do not edit!

namespace html_entity_parser_data {

// HTML named entity -> Unicode code point.
extern unordered_map<string, uint32_t> HTML2UNICODE;

}  // namespace html_entity_parser_data

#endif  // CC_PREPROCESS_HTML_ENTITY_PARSER_DATA_H_
"""[1:]


CC_SOURCE = """
#include "html_entity_parser_data.h"

// Automatically generated file -- do not edit!

namespace html_entity_parser_data {

unordered_map<string, uint32_t> HTML2UNICODE = {
%s
};

}  // namespace html_entity_parser_data
"""[1:]


INDENT = ' ' * 4


def quote(s):
    return '"%s"' % s.replace('"', '\"')


def generate_cc(name2code):
    lines = []
    for name in sorted(name2code):
        code = name2code[name]
        line = '%s{%s, 0x%x},' % (INDENT, quote(name), code)
        lines.append(line)
    return CC_SOURCE % '\n'.join(lines)


def main():
    h = H_SOURCE

    name2code = load_html_entities('html_entities.txt')
    cc = generate_cc(name2code)

    open('../../html_entity_parser_data.h', 'wb').write(h)
    open('../../html_entity_parser_data.cc', 'wb').write(cc)


if __name__ == '__main__':
    main()
