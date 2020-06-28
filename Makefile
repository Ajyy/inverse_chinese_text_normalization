all:

include ../kaldi.mk

TESTFILES =　itn-test

OBJFILES = itn.o

LIBNAME = kaldi-itn

ADDLIBS =　../base/kaldi-base.a

include ../makefiles/default_rules.mk
