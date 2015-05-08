CC = clang++

SRC_ROOT = phraser/
BIN_DIR = bin/
EXT_DIR = phraser/ext/

FLAGS_BASE = \
	-std=c++11 \
	-fcolor-diagnostics \
	-O3 \
	-ferror-limit=5 \
	-I$(SRC_ROOT) \
	-lboost_regex \

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

FLAGS = $(FLAGS_BASE) $(FLAGS_WARN) $(FLAGS_WARN_DISABLE) \
		$(FLAGS_WARN_DISABLE_LAPOS)

a:
	@echo
	@echo
	@echo
	@echo
	@echo
	@echo
	@echo
	@echo
	mkdir -p $(BIN_DIR)
	$(CC) `find -type f -name "*.cc"` $(SRC_ROOT)/tools/a.cpp -o $(BIN_DIR)/a $(FLAGS)

clean:
	rm -rf $(BIN_DIR)
	rm -rf $(EXT_DIR)

compare_against_impermium:
	mkdir -p $(BIN_DIR)
	$(CC) `find -type f -name "*.cc"` $(SRC_ROOT)/tools/compare_against_impermium.cpp -o $(BIN_DIR)/compare_against_impermium $(FLAGS)

all:
	rm -rf $(EXT_DIR)
	mkdir -p $(EXT_DIR)
	touch $(EXT_DIR)/__init__.py
	python setup.py build_ext
	rm -rf build/
