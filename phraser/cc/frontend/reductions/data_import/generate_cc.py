#!/usr/bin/python

H_SRC = """
#ifndef CC_FRONTEND_REDUCTIONS_REDUCTION_DATA_H_
#define CC_FRONTEND_REDUCTIONS_REDUCTION_DATA_H_

#include <string>
#include <vector>

using std::string;
using std::vector;

// Automatically generated file -- do not edit!

struct Reduction {
    string reduced;      // "wanna"

    string first;        // "wan"
    string second;       // "na"

    string norm_first;   // "want"
    string norm_second;  // "to"
};

namespace reduction_data = {

extern vector<Reduction> reductions;

}  // namespace

#endif  // CC_FRONTEND_REDUCTIONS_REDUCTION_DATA_H_
"""[1:]


CC_SRC = """
#include "reduction_data.h"

vector<Reduction> reductions = {
%s
};

"""[1:]


def each_line():
    with open('reductions.txt', 'rb') as f:
        for line in f:
            x = line.find('#')
            if x != -1:
                line = line[:x]
            ss = line.split()
            if ss:
                yield ss


def escape(s):
    return '"%s"' % s.replace('"', '\\"')


def make_reductions_s():
    lines = []
    for ss in each_line():
        print '!', ss
        assert len(ss) == 5
        line = '    {%s}' % ', '.join(map(escape, ss))
        lines.append(line)
    return '\n'.join(lines)


def main():
    open('../reduction_data.h', 'wb').write(H_SRC)

    cc_src = CC_SRC % make_reductions_s()
    open('../reduction_data.cc', 'wb').write(cc_src)


if __name__ == '__main__':
    main()
