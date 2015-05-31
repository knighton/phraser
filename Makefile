.PHONY: clean coverage develop env extras package release test virtualenv

OVERRIDE_USE_CLANG = 0

UNAME_S = $(shell uname -s)
ifeq ($(UNAME_S), Darwin)
	# Using clang for binary compatibility with boost.
	# CC = g++-4.8
	USE_CLANG = 1
else
	ifeq ($(OVERRIDE_USE_CLANG), 1)
		USE_CLANG = 1
	else
		USE_CLANG = 0
	endif
endif

ifeq ($(USE_CLANG), 1)
	CC = clang++
else
	CC = g++-4.7
endif

PYENV = . env/bin/activate;
PYTHON = $(PYENV) python
EXTRAS_REQS := $(wildcard requirements-*.txt)
DISTRIBUTE = sdist bdist_wheel

SRC_ROOT = phraser
BIN_DIR = bin
BUILD_ROOT = $(shell $(PYTHON) ./scripts/get_build_dir.py)
BUILD_DIR = $(BUILD_ROOT)/$(SRC_ROOT)
TEST_FILE = tests/data/50k_comments.txt

COMMON_FLAGS = \
	-std=c++11 \
	-O3 \
	-I$(SRC_ROOT) \
	-Wall \
	-Wextra \
	-Wno-c++98-compat-pedantic \
	-Wno-covered-switch-default \
	-Wno-padded \
	-Wno-unknown-pragmas \
	-Wno-old-style-cast \
	-Wno-unused-variable \
	-Wno-unused-parameter \
	-Wno-unused-function \

CLANG_FLAGS = \
	-Werror \
	-Wpedantic \
	-Weverything \
	-fcolor-diagnostics \
	-ferror-limit=5 \
	-Wno-weak-vtables \
	-Wno-global-constructors \
	-Wno-implicit-fallthrough \
	-Wno-exit-time-destructors \
	-Wno-shorten-64-to-32 \

ifeq ($(USE_CLANG), 1)
	CC_FLAGS = $(COMMON_FLAGS) $(CLANG_FLAGS)
else
	CC_FLAGS = $(COMMON_FLAGS)
endif

LD_FLAGS = -lboost_regex

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

test: extras build_ext
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
	rm -rf dist build $(BIN_DIR)
	find . -path ./env -prune -o -type f -regex '.*\.pyc' -or -regex '.*\.so' -exec rm {} \;

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

.PHONY: memcheck_cc memcheck_python

memcheck_cc: compare_against_impermium
	time valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes \
		$(COMPARE_BIN) $(TEST_FILE) 2> valgrind_stderr.txt

memcheck_python: build_ext
	valgrind --tool=memcheck --dsymutil=yes \
		--leak-check=full --show-leak-kinds=all --track-origins=yes \
		--suppressions=valgrind-python.supp \
		env/bin/python scripts/testmem.py 2> valgrind_python_stderr.txt

develop:
	@echo "Installing for " `which pip`
	-pip uninstall --yes $(PYMODULE)
	pip install -e .

.PHONY: build_ext
build_ext phraser/phraserext.so: env
	$(PYTHON) setup.py build_ext --inplace

env virtualenv: env/bin/activate
env/bin/activate: requirements.txt setup.py
	test -f $@ || virtualenv --no-site-packages env
	$(PYENV) pip install -U pip wheel
	$(PYENV) pip install -e . -r $<
	touch $@
