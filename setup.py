from setuptools import setup, Extension, find_packages
from setuptools.dist import Distribution
from pkg_resources import resource_string
import os
import platform


class BinaryDistribution(Distribution):
    """
    Subclass the setuptools Distribution to flip the purity flag to false.
    See http://lucumr.pocoo.org/2014/1/27/python-on-wheels/
    """
    def is_pure(self):
        # TODO: verify whether this is still necessary in Python v2.7
        return False


SRC_ROOT = 'phraser/'


# 1. Base flags.
# 2. Max out warnings.
# 3. Disable some warnings.
# 4. Disable more warnings for LAPOS.
COMMON_BASE_FLAGS = ("""
    -std=c++11
    -O3
    -I%s
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


CLANG_BASE_FLAGS = """
    -Wall
    -Wextra
    -Wpedantic
    -Werror
    -Weverything
""".split()


CLANG_DISABLE_FLAGS = """
    -Wno-padded
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


UBUNTU_VERSION_F = '/etc/lsb-release'
RELEASE_FIELD_NAME = 'DISTRIB_RELEASE'


def get_ubuntu_release():
    with open(UBUNTU_VERSION_F) as f:
        lines = f.readlines()
    kk_vv = map(lambda s: s.split('='), lines)
    k2v = dict(kk_vv)
    release = k2v[RELEASE_FIELD_NAME]
    nn = map(int, release.split('.'))
    assert len(nn) == 2
    return nn


def is_ubuntu_old():
    try:
        major, minor = get_ubuntu_release()
    except:
        return False  # Guess it's recent.

    return major < 13  # Compat required.


if platform.system() == 'Darwin':
    os.environ['CC'] = 'clang++'
    os.environ['CXX'] = 'clang++'
else:
    if is_ubuntu_old():
        os.environ['CC'] = 'g++-4.7'
        os.environ['CXX'] = 'g++-4.7'
    else:
        os.environ['CC'] = 'g++'
        os.environ['CXX'] = 'g++'


if os.environ.get('CXX') == 'clang++':
    FLAGS = COMMON_BASE_FLAGS + CLANG_BASE_FLAGS + COMMON_LAPOS_FLAGS + \
        CLANG_DISABLE_FLAGS + CLANG_LAPOS_FLAGS
else:
    FLAGS = COMMON_BASE_FLAGS + COMMON_LAPOS_FLAGS


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
    version='0.1.6',
    author='James Knighton',
    author_email='iamknighton@gmail.com',
    description='Detects phrases in English text',
    license='MIT',
    packages=find_packages(exclude=['tests', 'scripts']),
    ext_modules=[phraser],
    long_description=resource_string(__name__, 'README.rst'),
    distclass=BinaryDistribution,
)
