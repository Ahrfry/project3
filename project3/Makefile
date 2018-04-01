#### RVM Library Makefile

CFLAGS  = -Wall -g -I.
LFLAGS  =
CC      = g++
RM      = /bin/rm -rf
AR      = ar rc
RANLIB  = ranlib
OPTIONS= -g 
LIBRARY = librvm.a

LIB_SRC = rvm.cpp

LIB_OBJ = $(patsubst %.cpp,%.o,$(LIB_SRC))

%.o: %.cpp
	$(CC) -c $(CFLAGS) $< -o $@

$(LIBRARY): $(LIB_OBJ)
	$(AR) $(LIBRARY) $(LIB_OBJ)
	$(RANLIB) $(LIBRARY)



TESTCASESDIR=.

all: basic multi abort multi-abort truncate
basic: 
	        $(CC) $(OPTIONS) -o basic testcase/basic.c librvm.a

abort: 
	        $(CC) $(OPTIONS) -o abort testcase/abort.c librvm.a

multi: 
	        $(CC) $(OPTIONS) -o multi testcase/multi.c librvm.a

multi-abort: 
	        $(CC) $(OPTIONS) -o multi-abort testcase/multi-abort.c librvm.a

truncate: 
	        $(CC) $(OPTIONS) -o truncate testcase/truncate.c librvm.a






clean:
	        rm -rf *.o basic multi abort multi-abort truncate


