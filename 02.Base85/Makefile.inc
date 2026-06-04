include ../Makefile.inc

release : base85

base85  : base85.r.o base85ed.r.o
	$(LD) -o $@ $^

debug   : base85.d.o base85ed.d.o
	$(LD) -o $@ $^

utest   : base85ed_test.d.o base85ed.d.o
	$(LD)  -o $@ $^ $(GTEST_LIBS)

run_utest : utest
	./$^

run_itest : release
	echo "Integration tests..."
	./integration_test.sh

all: depends run_utest run_itest

include Makefile.hdrdep
