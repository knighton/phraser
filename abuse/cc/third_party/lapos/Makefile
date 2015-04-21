BASE_FLAGS=-O3 -DNDEBUG -std=c++11 -fcolor-diagnostics -std=c++11
WARN_FLAGS=-Wpedantic -Wall -Weverything -Wextra -Werror
DISABLE_WARNINGS=\
	-Wno-c++98-compat-pedantic \
	-Wno-covered-switch-default \
	-Wno-exit-time-destructors \
	-Wno-global-constructors \
	-Wno-padded \
	-Wno-float-equal \
	-Wno-unused-variable \
	-Wno-unused-parameter \
	-Wno-unused-function \

WARNINGS_FIX_LATER=\
	-Wno-shorten-64-to-32 \
	-Wno-sign-conversion \
	-Wno-old-style-cast \
	-Wno-sign-compare \

CFLAGS=$(BASE_FLAGS) $(WARN_FLAGS) $(DISABLE_WARNINGS) $(WARNINGS_FIX_LATER)
CPP = clang++
LOBJS = learn.cc crf.o crfpos.o lookahead.o string_store.o
MOBJS = main.cc lookahead.o tokenize.o crf.o crfpos.o string_store.o

all: lapos-learn lapos lapos-eval

lapos-learn: $(LOBJS)
	$(CPP) -o lapos-learn $(CFLAGS) $(LOBJS)
lapos: $(MOBJS)
	$(CPP) -o lapos $(CFLAGS) $(MOBJS)
lapos-eval: eval.cc
	$(CPP) -o lapos-eval $(CFLAGS) eval.cc
clean:
	/bin/rm -r -f $(OBJS) a.out tmp lapos lapos-learn lapos-eval *.o *~
.cc.o:
	$(CPP) -c $(CFLAGS) $<
