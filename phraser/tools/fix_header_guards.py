#!/usr/bin/python
#
# Fix each .h header guard (useful after moving files around).

import os


DOIT = True


def each_header(root_dir):
    for root, dirs, files in os.walk(root_dir):
        for name in files:
            if name.endswith('.h'):
                f = os.path.join(root, name)
                yield f


def header_guard_from_file_name(f):
    if f.startswith('./'):
        f = f[2:]
    return f.replace('/', '_').replace('.h', '_H_').upper()


def fix_header_guards(root_dir):
    for fn in each_header(root_dir):
        new_header = header_guard_from_file_name(fn)
        text = open(fn).read()
        ss = text.split()
        try:
            assert ss[0] == '#ifndef'
        except:
            print 'WTF:', fn
            assert False
        old_header = ss[1]
        if old_header != new_header:
            if DOIT:
                open(fn, 'wb').write(text.replace(old_header, new_header))
            else:
                print 'Would change a header:'
                print '  file:', fn
                print '  old: ', old_header
                print '  new: ', new_header


def main():
    fix_header_guards('.')


if __name__ == '__main__':
    main()
