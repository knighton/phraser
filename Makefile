.PHONY: clean coverage develop env extras package release test virtualenv

CC = clang++

SRC_ROOT = phraser/
BIN_DIR = bin/
EXT_DIR = phraser/ext/

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
	rm -rf $(BIN_DIR)
	rm -rf $(EXT_DIR)
	rm -f $(EXT_DIR)/*.so
	python setup.py clean
	rm -rf dist build
	find . -path ./env -prune -o -type f -name "*.pyc" -exec rm {} \;

compare_against_impermium:
	mkdir -p $(BIN_DIR)
	$(CC) `find -type f -name "*.cc"` $(SRC_ROOT)/tools/compare_against_impermium.cpp -o $(BIN_DIR)/compare_against_impermium $(FLAGS)

develop:
	@echo "Installing for " `which pip`
	pip uninstall $(PYMODULE) || true
	python setup.py develop

env virtualenv: env/bin/activate
env/bin/activate: requirements.txt setup.py
	test -f $@ || virtualenv --no-site-packages env
	$(PYENV) pip install -U pip wheel
	$(PYENV) pip install -e . -r $<
	mkdir -p $(EXT_DIR)
	$(PYTHON) setup.py build_ext
	touch $@
