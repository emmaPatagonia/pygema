.SUFFIXES : .o .cpp .c

OBJS =\
 cartesian.o\
 velmod.o\
 position.o\
 livloc.o

LIB = liblivloc.a
INCLUDES     = 
DEFINES      =
CCFLAGS      = -ggdb -Wall
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

cartesian.o: cartesian.h position.h /usr/include/values.h
cartesian.o: /usr/include/features.h /usr/include/sys/cdefs.h
cartesian.o: /usr/include/gnu/stubs.h /usr/include/limits.h
cartesian.o: /usr/lib/gcc/x86_64-redhat-linux/3.4.6/include/limits.h
cartesian.o: /usr/lib/gcc/x86_64-redhat-linux/3.4.6/include/float.h
livloc.o: livloc.h position.h /usr/include/values.h /usr/include/features.h
livloc.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
livloc.o: /usr/include/limits.h
livloc.o: /usr/lib/gcc/x86_64-redhat-linux/3.4.6/include/limits.h
livloc.o: /usr/lib/gcc/x86_64-redhat-linux/3.4.6/include/float.h velmod.h
livloc.o: uTravelTime.h uConst.h cartesian.h TNT/tnt.h TNT/tnt_version.h
livloc.o: TNT/tnt_math_utils.h TNT/tnt_array1d.h TNT/tnt_i_refvec.h
livloc.o: TNT/tnt_array2d.h TNT/tnt_array3d.h TNT/tnt_array1d_utils.h
livloc.o: TNT/tnt_array2d_utils.h TNT/tnt_array3d_utils.h
livloc.o: TNT/tnt_fortran_array1d.h TNT/tnt_fortran_array2d.h
livloc.o: TNT/tnt_fortran_array3d.h TNT/tnt_fortran_array1d_utils.h
livloc.o: TNT/tnt_fortran_array2d_utils.h TNT/tnt_fortran_array3d_utils.h
livloc.o: TNT/tnt_sparse_matrix_csr.h TNT/tnt_stopwatch.h /usr/include/time.h
livloc.o: /usr/lib/gcc/x86_64-redhat-linux/3.4.6/include/stddef.h
livloc.o: /usr/include/bits/time.h /usr/include/bits/types.h
livloc.o: /usr/include/bits/wordsize.h /usr/include/bits/typesizes.h
livloc.o: TNT/tnt_subscript.h TNT/tnt_vec.h TNT/tnt_cmat.h TNT/jama_svd.h
position.o: position.h /usr/include/values.h /usr/include/features.h
position.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
position.o: /usr/include/limits.h
position.o: /usr/lib/gcc/x86_64-redhat-linux/3.4.6/include/limits.h
position.o: /usr/lib/gcc/x86_64-redhat-linux/3.4.6/include/float.h
velmod.o: velmod.h uTravelTime.h uConst.h
