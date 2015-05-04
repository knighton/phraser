#!/bin/sh
#
# wc the non-data, non-third party source files.

find . -type f -name "*.py" | sort | grep -v third_party | grep -v _data | xargs wc
echo
find . -type f -name "*.h" | sort | grep -v third_party | grep -v _data | xargs wc
echo
find . -type f -name "*.cc" | sort | grep -v third_party | grep -v _data | xargs wc
