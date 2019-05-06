.SUFFIXES : .o .cpp .c

OBJS =\
 main.o

EXEC = binder_nosc_AR
LIBS = -L./lib -lbinder -lnll -lhypo71 -llivloc
INCLUDES     = -I./include -I$(SC_HOME)/trunk/include
DEFINES      = 
CCFLAGS      = -ggdb -Wall
LDFLAGS      = 
CXXFLAGS     = $(CCFLAGS) $(DEFINES) $(INCLUDES)
CFLAGS       = $(CCFLAGS) $(DEFINES) $(INCLUDES)

CCPLUS = g++
CC     = gcc

.cpp.o: ; $(CCPLUS) -c $(CXXFLAGS) $*.cpp
.c.o: ; $(CC) -c $(CFLAGS) $*.c

all: $(EXEC)

$(EXEC): $(OBJS) 
	$(CCPLUS) $(LDFLAGS) -o $@ $(INCLUDES) $(OBJS) $(LIBS)

clean:
	$(RM) *.o *~ $(EXEC) Makefile.bak Makefile.BAK

depend:
	makedepend $(INCLUDES) *.cpp

# DO NOT DELETE

main.o: ./include/hypo71.h ./include/aehnll.h binder/input.h
main.o: binder/mystation.h binder/uTravelTime.h binder/uConst.h
main.o: binder/binder.h binder/mypick.h binder/position.h
main.o: /usr/include/values.h /usr/include/features.h
main.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
main.o: /usr/include/limits.h
main.o: /usr/lib/gcc/x86_64-redhat-linux/3.4.6/include/limits.h
main.o: /usr/lib/gcc/x86_64-redhat-linux/3.4.6/include/float.h
main.o: binder/cartesian.h binder/stack.h binder/arrayindex.h
main.o: binder/arrayposition.h binder/stacknode.h binder/event.h
main.o: /usr/include/time.h
main.o: /usr/lib/gcc/x86_64-redhat-linux/3.4.6/include/stddef.h
main.o: /usr/include/bits/time.h /usr/include/bits/types.h
main.o: /usr/include/bits/wordsize.h /usr/include/bits/typesizes.h
main.o: binder/velmod.h binder/uTravelTime.h binder/constants.h
main.o: binder/latencycheck.h binder/mystation.h binder/livloc.h
main.o: binder/velmod.h
