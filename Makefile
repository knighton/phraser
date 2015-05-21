.PHONY: clean coverage develop env extras package release test virtualenv

CC = clang++

SRC_ROOT = phraser
BIN_DIR = bin
BUILD_ROOT = build/temp
BUILD_DIR = $(BUILD_ROOT)/$(SRC_ROOT)
TEST_FILE = tests/data/50k_comments.txt

FLAGS_BASE = \
    -std=c++11 \
    -fcolor-diagnostics \
    -O3 \
    -ferror-limit=5 \
    -I$(SRC_ROOT)

FLAGS_WARN = \
    -Wpedantic \
    -Wall \
    -Weverything \
    -Wextra \
    -Werror \

FLAGS_WARN_DISABLE = \
    -Wno-c++98-compat-pedantic \
    -Wno-covered-switch-default \
    -Wno-exit-time-destructors \
    -Wno-global-constructors \
    -Wno-padded \
    -Wno-weak-vtables \

FLAGS_WARN_DISABLE_LAPOS = \
    -Wno-shorten-64-to-32 \
    -Wno-sign-conversion \
    -Wno-old-style-cast \
    -Wno-sign-compare \
    -Wno-float-equal \
    -Wno-unused-variable \
    -Wno-unused-parameter \
    -Wno-unused-function \

CC_FLAGS = $(FLAGS_BASE) $(FLAGS_WARN) $(FLAGS_WARN_DISABLE) \
        $(FLAGS_WARN_DISABLE_LAPOS)

LD_FLAGS = -lboost_regex

PYENV = . env/bin/activate;
PYTHON = $(PYENV) python
EXTRAS_REQS := $(wildcard requirements-*.txt)
DISTRIBUTE = sdist bdist_wheel

package: env
	$(PYTHON) setup.py $(DISTRIBUTE)

release: env
	$(PYTHON) setup.py $(DISTRIBUTE) upload -r livefyre

# if in local dev on Mac, `make coverage` will run tests and open
# coverage report in the browser
ifeq ($(shell uname -s), Darwin)
coverage: test
	open cover/index.html
endif

test: extras
	$(PYENV) nosetests $(NOSEARGS)
	$(PYENV) py.test README.rst

extras: env/make.extras
env/make.extras: $(EXTRAS_REQS) | env
	rm -rf env/build
	$(PYENV) for req in $?; do pip install -r $$req; done
	touch $@

nuke: clean
	rm -rf *.egg *.egg-info env bin cover coverage.xml nosetests.xml

clean:
	python setup.py clean
	rm -rf dist $(BUILD_DIR) $(BIN_DIR)
	find . -path ./env -prune -o -type f -name "*.pyc" -exec rm {} \;
	find . -path ./env -prune -o -type f -name "*.so" -exec rm {} \;

CC_SOURCES = $(shell find $(SRC_ROOT) -name "*.cc")
O_INTERMEDIATES := $(patsubst $(SRC_ROOT)/%,$(BUILD_DIR)/%,$(CC_SOURCES:.cc=.o))
COMPARE_MAIN = $(SRC_ROOT)/tools/compare_against_impermium.cpp
COMPARE_BIN = $(BIN_DIR)/compare_against_impermium

$(BUILD_DIR)/%.o: $(SRC_ROOT)/%.cc
	mkdir -p $(dir $@)
	$(CC) -c $< -o $@ $(CC_FLAGS)

compare_against_impermium $(COMPARE_BIN): $(O_INTERMEDIATES)
	mkdir -p $(BIN_DIR)
	$(CC) $(CC_FLAGS) $(O_INTERMEDIATES) $(COMPARE_MAIN) -o $(COMPARE_BIN) $(LD_FLAGS)

memcheck: compare_against_impermium
	time valgrind --leak-check=full --track-origins=yes \
		$(COMPARE_BIN) $(TEST_FILE)
		2> valgrind_stderr.txt

develop:
	@echo "Installing for " `which pip`
	pip uninstall $(PYMODULE) || true
	python setup.py develop

.PHONY: build_ext
build_ext phraser/phraserext.so: env
	$(PYTHON) setup.py build_ext --inplace

env virtualenv: env/bin/activate
env/bin/activate: requirements.txt setup.py
	test -f $@ || virtualenv --no-site-packages env
	$(PYENV) pip install -U pip wheel
	$(PYENV) pip install -e . -r $<
	touch $@
