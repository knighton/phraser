#!/usr/bin/python


INDENT = ' ' * 4


H_SRC = """
#ifndef CC_FRONTEND_REDUCTIONS_REDUCTION_DATA_H_
#define CC_FRONTEND_REDUCTIONS_REDUCTION_DATA_H_

#include <string>
#include <unordered_map>
#include <vector>

using std::string;
using std::unordered_map;
using std::vector;

// Automatically generated file -- do not edit!

namespace reduction_data = {

extern unordered_map<string, vector<vector<string>>> REDUCED2NORMAL;

}  // namespace

#endif  // CC_FRONTEND_REDUCTIONS_REDUCTION_DATA_H_
"""[1:]


CC_SRC = """
#include "reduction_data.h"

unordered_map<string, vector<vector<string>>> REDUCED2NORMAL = {
%s
};

"""[1:]


def each_line():
    with open('reductions.txt', 'rb') as f:
        for line in f:
            x = line.find('#')
            if x != -1:
                line = line[:x]
            s = line.strip()
            if not s:
                continue
            yield s


def escape(s):
    return '"%s"' % s.replace('"', '\\"')


def parse_line(line):
    line = ' '.join(line.split())
    reduced_s, normal_s = line.split(' -> ')
    reduced_ss = reduced_s.split(', ')
    normal_options = normal_s.split(', ')
    rrr = []
    for s in normal_options:
        ss = s.split()
        rr = []
        for s in ss:
            tt = s.split('/')
            if len(tt) == 1:
                word, tag = tt[0], ''
            elif len(tt) == 2:
                word, tag = tt
            else:
                assert False
            rr += [word, tag]
        rrr.append(rr)
    return reduced_ss, rrr


def dump_list(ss):
    rr = []
    rr.append('{')
    if ss:
        rr.append(escape(ss[0]))
    for s in ss[1:]:
        rr.append(', ')
        rr.append(escape(s))
    rr.append('}')
    return rr


def dump_list_of_lists(sss):
    rr = []
    rr.append('{')
    if sss:
        rr += dump_list(sss[0])
    for ss in sss[1:]:
        rr.append(', ')
        rr += dump_list(ss)
    rr.append('}')
    return rr


def dump_line(reduced, sss):
    rr = []
    rr.append(INDENT)
    rr.append('{')
    rr.append(escape(reduced))
    rr.append(', ')
    rr += dump_list_of_lists(sss)
    rr.append('}')
    return ''.join(rr)


def make_reductions_s():
    lines = []
    for line in each_line():
        reduceds, sss = parse_line(line)
        for reduced in reduceds:
            line = dump_line(reduced, sss)
            lines.append(line)
    lines.sort()
    return '\n'.join(lines)


def main():
    open('../reduction_data.h', 'wb').write(H_SRC)

    cc_src = CC_SRC % make_reductions_s()
    open('../reduction_data.cc', 'wb').write(cc_src)


if __name__ == '__main__':
    main()
