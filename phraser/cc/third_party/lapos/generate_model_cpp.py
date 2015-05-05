#!/usr/bin/python


H_SRC = """
#ifndef CC_THIRD_PARTY_LAPOS_MODEL_DATA_H_
#define CC_THIRD_PARTY_LAPOS_MODEL_DATA_H_

#include <vector>

using std::vector;

namespace lapos_model_data {

extern const char* WSJ_02_21_LINES[];

}  // namespace lapos_model_data

#endif  // CC_THIRD_PARTY_LAPOS_MODEL_DATA_H_
"""[1:]


CC_SRC = """
#include "lapos_model_data.h"

namespace lapos_model_data {

const char* WSJ_02_21_LINES[] = {
%s};

}  // namespace lapos_model_data
"""[1:]


def escape(s):
    return s.replace('\\', '\\\\').replace('"', '\\"')


def main():
    model_f = 'model_wsj02-21/model.la'
    lines = open(model_f).read().strip().split('\n')
    lines = map(lambda s: '    "%s",\n' % escape(s), lines)
    lines.append('    nullptr,\n')
    open('lapos_model_data.h', 'wb').write(H_SRC)
    open('lapos_model_data.cc', 'wb').write(CC_SRC % ''.join(lines))


if __name__ == '__main__':
    main()
