CC=gcc
LD=gcc
CFLAGS=-I/usr/include -O3
LDFLAGS=-L/usr/lib/x86_64-linux-gnu -lgsl -lgslcblas -lm

.SUFFIXES:
.SUFFIXES: .c .o .h .svg .dat

.PHONY: all clean

all: qruov_tau.h

qruov_tau.h: find_tau
	./find_tau_all.sh > qruov_tau.h

find_tau: find_tau.o tau.o
	${LD} $@.o tau.o ${LDFLAGS} -o $@

clean:
	rm -f find_tau qruov_tau.h find_tau.o tau.o
