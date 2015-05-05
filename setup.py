from distutils.core import setup, Extension
import os


SRC_ROOT = 'phraser/'


def find_cc_files(root_dir):
    ff = []
    for root, dirs, files in os.walk(root_dir):
        for name in files:
            if name.endswith('.cc'):
                f = os.path.join(root, name)
                ff.append(f)
    return ff


os.environ['CC'] = 'clang'
os.environ['CXX'] = 'clang++'


phraser = Extension(
    'phraser',
    sources=find_cc_files(SRC_ROOT) + ['phraser/cc/pyext/phraser.cpp'],
    include_dirs=[SRC_ROOT],
    libraries=['boost_regex'],
    library_dirs=['/usr/local/lib/'],
)


setup(
    ext_modules=[phraser],
)
