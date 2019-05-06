.SUFFIXES : .o .cpp .c

OBJS =\
 julday.o\
 caldat.o\
 utl_hypd.o\
 utl_time.o\
 hypo71_core.o\
 hypo71.o

LIB = libhypo71.a
INCLUDES     = -I../include
DEFINES      =
CCFLAGS      = -ggdb -Wall
#CCFLAGS      = -O2
LDFLAGS      =
CXXFLAGS     = $(CCFLAGS) $(DEFINES) $(INCLUDES)
CFLAGS       = $(CCFLAGS) $(DEFINES) $(INCLUDES)

AR = ar -rs
CCPLUS = g++
CC     = gcc

.cpp.o: ; $(CCPLUS) -c $(CXXFLAGS) $*.cpp
.c.o: ; $(CC) -c $(CFLAGS) $*.c

all: $(LIB)

$(LIB): $(OBJS) 
	$(RM) $@
	$(AR) $@ $(OBJS)
	cp $(LIB) ../lib/$(LIB)

clean:
	$(RM) *.o *~ $(LIB) Makefile.bak Makefile.BAK

depend:
	makedepend $(INCLUDES) *.cpp

# DO NOT DELETE

hypo71_core.o: /usr/include/memory.h /usr/include/features.h
hypo71_core.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
hypo71_core.o: /usr/include/string.h
hypo71_core.o: /usr/lib/gcc/x86_64-redhat-linux/3.4.6/include/stddef.h
hypo71_core.o: /usr/include/unistd.h /usr/include/bits/posix_opt.h
hypo71_core.o: /usr/include/bits/types.h /usr/include/bits/wordsize.h
hypo71_core.o: /usr/include/bits/typesizes.h /usr/include/bits/confname.h
hypo71_core.o: ../include/pdas.h ../include/lsd.h loma.h utl_time.h
hypo71_core.o: hypo71_core.h ../include/hypo71.h constants.h
hypo71.o: /usr/include/time.h /usr/include/features.h
hypo71.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
hypo71.o: /usr/lib/gcc/x86_64-redhat-linux/3.4.6/include/stddef.h
hypo71.o: /usr/include/bits/time.h /usr/include/bits/types.h
hypo71.o: /usr/include/bits/wordsize.h /usr/include/bits/typesizes.h
hypo71.o: /usr/include/unistd.h /usr/include/bits/posix_opt.h
hypo71.o: /usr/include/bits/confname.h ../include/pdas.h ../include/lsd.h
hypo71.o: loma.h utl_time.h hypo71_core.h ../include/hypo71.h
