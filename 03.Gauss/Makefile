CXX_EXTRA_FLAGS=-I ../external/lazycsv/include -I ../external/eigen $(shell pkgconf --cflags openblas)
LD_EXTRA_OPT=$(pkgconf --libs openblas)

include ../Makefile.inc

release : gauss

gauss  : util.r.o Gauss_main.r.o Gauss_solve.r.o
	$(LD) -fopenmp -o $@ $^ $(LD_EXTRA_OPT)

debug   : util.d.o Gauss_main.d.o Gauss_solve.d.o
	$(LD) -fopenmp -o $@ $^ $(LD_EXTRA_OPT)

utest   : Gauss_test.d.o util.d.o Gauss_solve.d.o
	$(LD) -fopenmp -o $@ $^ $(GTEST_LIBS)

run_utest : utest
	./$^

run_itest : release
	echo "Integration tests..."
	./integration_test.sh

all: depends run_utest run_itest

include Makefile.hdrdep
