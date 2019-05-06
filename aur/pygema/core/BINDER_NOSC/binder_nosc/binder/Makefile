.SUFFIXES : .o .cpp .c

OBJS =\
 mypick.o\
 arrayposition.o\
 arrayindex.o\
 constants.o\
 position.o\
 stacknode.o\
 stack.o\
 event.o\
 binder.o\
 input.o\
 cartesian.o\
 uTravelTime.o\
 velmod.o\
 latencycheck.o\
 debug.o

LIB = libbinder.a
INCLUDES     = -I../include
#DEFINES      = -DMY_DEBUG
DEFINES      = 
CCFLAGS      = -ggdb -Wall
#CCFLAGS      = -O2
LDFLAGS      = 
CXXFLAGS     = $(CCFLAGS) $(DEFINES) $(INCLUDES)
CFLAGS       = $(CCFLAGS) $(DEFINES) $(INCLUDES)
AR           = ar -rs

CCPLUS = g++
CC     = gcc

.cpp.o: ; $(CCPLUS) -c $(CXXFLAGS) $*.cpp
.c.o: ; $(CC) -c $(CFLAGS) $*.c

all: $(LIB)

$(LIB):	$(OBJS) 
	$(RM) $@
	$(AR) $@ $(OBJS)
	cp $(LIB) ../lib/$(LIB)

clean:
	$(RM) *.o *~ $(LIB) Makefile.bak Makefile.BAK

depend:
	makedepend $(INCLUDES) *.cpp

# DO NOT DELETE

arrayindex.o: arrayindex.h arrayposition.h
arrayposition.o: arrayposition.h
binder.o: binder.h mypick.h position.h /usr/include/values.h
binder.o: /usr/include/features.h /usr/include/sys/cdefs.h
binder.o: /usr/include/gnu/stubs.h /usr/include/limits.h
binder.o: /usr/lib/gcc/x86_64-redhat-linux/3.4.6/include/limits.h
binder.o: /usr/lib/gcc/x86_64-redhat-linux/3.4.6/include/float.h cartesian.h
binder.o: stack.h arrayindex.h arrayposition.h stacknode.h event.h
binder.o: /usr/include/time.h
binder.o: /usr/lib/gcc/x86_64-redhat-linux/3.4.6/include/stddef.h
binder.o: /usr/include/bits/time.h /usr/include/bits/types.h
binder.o: /usr/include/bits/wordsize.h /usr/include/bits/typesizes.h
binder.o: mystation.h debug.h livloc.h velmod.h uTravelTime.h uConst.h
cartesian.o: cartesian.h position.h /usr/include/values.h
cartesian.o: /usr/include/features.h /usr/include/sys/cdefs.h
cartesian.o: /usr/include/gnu/stubs.h /usr/include/limits.h
cartesian.o: /usr/lib/gcc/x86_64-redhat-linux/3.4.6/include/limits.h
cartesian.o: /usr/lib/gcc/x86_64-redhat-linux/3.4.6/include/float.h
constants.o: /usr/include/values.h /usr/include/features.h
constants.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
constants.o: /usr/include/limits.h
constants.o: /usr/lib/gcc/x86_64-redhat-linux/3.4.6/include/limits.h
constants.o: /usr/lib/gcc/x86_64-redhat-linux/3.4.6/include/float.h
constants.o: constants.h
debug.o: debug.h mypick.h
event.o: utl_time.h event.h /usr/include/time.h /usr/include/features.h
event.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
event.o: /usr/lib/gcc/x86_64-redhat-linux/3.4.6/include/stddef.h
event.o: /usr/include/bits/time.h /usr/include/bits/types.h
event.o: /usr/include/bits/wordsize.h /usr/include/bits/typesizes.h mypick.h
event.o: position.h /usr/include/values.h /usr/include/limits.h
event.o: /usr/lib/gcc/x86_64-redhat-linux/3.4.6/include/limits.h
event.o: /usr/lib/gcc/x86_64-redhat-linux/3.4.6/include/float.h cartesian.h
event.o: debug.h livloc.h velmod.h uTravelTime.h uConst.h ../include/hypo71.h
event.o: ../include/aehnll.h
input.o: input.h mystation.h
latencycheck.o: latencycheck.h mypick.h
mypick.o: mypick.h
position.o: position.h /usr/include/values.h /usr/include/features.h
position.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
position.o: /usr/include/limits.h
position.o: /usr/lib/gcc/x86_64-redhat-linux/3.4.6/include/limits.h
position.o: /usr/lib/gcc/x86_64-redhat-linux/3.4.6/include/float.h
stack.o: /usr/include/time.h /usr/include/features.h /usr/include/sys/cdefs.h
stack.o: /usr/include/gnu/stubs.h
stack.o: /usr/lib/gcc/x86_64-redhat-linux/3.4.6/include/stddef.h
stack.o: /usr/include/bits/time.h /usr/include/bits/types.h
stack.o: /usr/include/bits/wordsize.h /usr/include/bits/typesizes.h stack.h
stack.o: mypick.h position.h /usr/include/values.h /usr/include/limits.h
stack.o: /usr/lib/gcc/x86_64-redhat-linux/3.4.6/include/limits.h
stack.o: /usr/lib/gcc/x86_64-redhat-linux/3.4.6/include/float.h cartesian.h
stack.o: arrayindex.h arrayposition.h stacknode.h event.h
stacknode.o: stacknode.h mypick.h arrayposition.h position.h
stacknode.o: /usr/include/values.h /usr/include/features.h
stacknode.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
stacknode.o: /usr/include/limits.h
stacknode.o: /usr/lib/gcc/x86_64-redhat-linux/3.4.6/include/limits.h
stacknode.o: /usr/lib/gcc/x86_64-redhat-linux/3.4.6/include/float.h
stacknode.o: cartesian.h event.h /usr/include/time.h
stacknode.o: /usr/lib/gcc/x86_64-redhat-linux/3.4.6/include/stddef.h
stacknode.o: /usr/include/bits/time.h /usr/include/bits/types.h
stacknode.o: /usr/include/bits/wordsize.h /usr/include/bits/typesizes.h
uTravelTime.o: uConst.h uTravelTime.h
velmod.o: velmod.h uTravelTime.h uConst.h
