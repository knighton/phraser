import os
import time


COMPILER = 'clang++'


INCLUDE_ROOT_DIR = 'abuse/'


TARGET = 'build/abuse'


BASE_FLAGS = """
    -std=c++11
    -fcolor-diagnostics
    -O3
    -ferror-limit=5
    -lboost_regex
""".split()


WARN_FLAGS = """
    -Wpedantic
    -Wall
    -Weverything
    -Wextra
    -Werror
""".split()


DISABLE_WARN_FLAGS = """
    -Wno-c++98-compat-pedantic
    -Wno-covered-switch-default
    -Wno-exit-time-destructors
    -Wno-global-constructors
    -Wno-padded
    -Wno-weak-vtables
""".split()

WARNINGS_TO_FIX_LATER = """
    -Wno-shorten-64-to-32
    -Wno-sign-conversion
    -Wno-old-style-cast
    -Wno-sign-compare
    -Wno-float-equal

    -Wno-unused-variable
    -Wno-unused-parameter
    -Wno-unused-function
""".split()


def find_cc_files(root_dir):
    ff = []
    for root, dirs, files in os.walk(root_dir):
        for name in files:
             if name.endswith('.cc'):
                 f = os.path.join(root, name)
                 ff.append(f)
    return ff


def main():
    print '\n' * 10
    flags = BASE_FLAGS + WARN_FLAGS + DISABLE_WARN_FLAGS + WARNINGS_TO_FIX_LATER
    ff = find_cc_files('.')
    os.system('mkdir -p %s' % os.path.dirname(TARGET))
    t0 = time.time()
    os.system('%s %s %s -o %s -I%s' %
             (COMPILER, ' '.join(ff), ' '.join(flags), TARGET,
              INCLUDE_ROOT_DIR))
    t1 = time.time()
    t = t1 - t0
    print '%.3f sec' % t


if __name__ == '__main__':
    main()
