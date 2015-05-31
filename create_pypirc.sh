#!/bin/bash

cat <<- EOF > ~/.pypirc
[distutils]
index-servers =
    livefyre

[livefyre]
repository:https://pypi.livefyre.com
username:$LF_PYPI_USERNAME
password:$LF_PYPI_PASSWORD
EOF
