from setuptools import setup, Extension, find_packages
from pkg_resources import resource_string
import os
import platform


SRC_ROOT = 'phraser/'


# 1. Base flags.
# 2. Max out warnings.
# 3. Disable some warnings.
# 4. Disable more warnings for LAPOS.
COMMON_FLAGS = ("""
    -std=c++11
    -O3
    -I%s

    -Wpedantic
    -Wall
    -Wextra

    -Wno-padded
""" % SRC_ROOT).split()


COMMON_LAPOS_FLAGS = """
    -Wno-sign-conversion
    -Wno-old-style-cast
    -Wno-sign-compare
    -Wno-float-equal
    -Wno-unused-variable
    -Wno-unused-parameter
    -Wno-unused-function
""".split()


CLANG_FLAGS = """
    -Werror
    -Weverything
    -fcolor-diagnostics
    -ferror-limit=5
    -Wno-c++98-compat-pedantic
    -Wno-covered-switch-default
    -Wno-weak-vtables
    -Wno-global-constructors
    -Wno-implicit-fallthrough
""".split()


CLANG_LAPOS_FLAGS = """
    -Wno-exit-time-destructors
    -Wno-shorten-64-to-32
""".split()


# C++ source naming convention:
# * Main files end with .cpp
# * Everything else ends with .cc
def find_cc_files(root_dir):
    ff = []
    for root, dirs, files in os.walk(root_dir):
        for name in files:
            if name.endswith('.cc'):
                f = os.path.join(root, name)
                ff.append(f)
    return ff


if platform.system() == 'Darwin':
    os.environ['CC'] = 'clang++'
    os.environ['CXX'] = 'clang++'
else:
    os.environ['CC'] = 'g++'
    os.environ['CXX'] = 'g++'


if os.environ.get('CXX', None) == 'clang++':
    FLAGS = COMMON_FLAGS + COMMON_LAPOS_FLAGS + CLANG_FLAGS + CLANG_LAPOS_FLAGS
else:
    FLAGS = COMMON_FLAGS + COMMON_LAPOS_FLAGS


phraser = Extension(
    'phraser.phraserext',
    sources=find_cc_files(SRC_ROOT) + ['phraser/cc/pyext/phraserext.cpp'],
    extra_compile_args=FLAGS,
    include_dirs=[SRC_ROOT, '.'],
    libraries=['boost_regex'],
    library_dirs=['/usr/local/lib/'],
)

setup(
    name='phraser',
    version='0.1.3',
    author='James Knighton',
    author_email='iamknighton@gmail.com',
    description='Detects phrases in English text',
    license='MIT',
    packages=find_packages(exclude=['tests', 'scripts']),
    ext_modules=[phraser],
    long_description=resource_string(__name__, 'README.rst'),
)
