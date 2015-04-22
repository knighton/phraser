CC = clang++

SRC_ROOT_DIR = abuse/
BUILD_DIR = build/

FLAGS_BASE = \
	-std=c++11 \
	-fcolor-diagnostics \
	-O3 \
	-ferror-limit=5 \
	-lboost_regex \
	-I$(SRC_ROOT_DIR)

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

all:
	$(CC) `find -type f -name "*.cc"` -o $(BUILD_DIR)/abuse $(FLAGS)
