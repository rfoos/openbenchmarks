AUTOMAKE_OPTIONS=foreign

#SUBDIRS = openmpi

# This is the makefile for NetPipe
# Simply type make with one of the following choices for environments:
#
#      mpi         : will use mpicc to compile
#      mplite      : It will look for the MP_Lite library in $HOME/mplite
#      tcp         : You start the receiver and transmitter manually
#      paragon     : Uses MPI on the Paragon
#      pvm         : Old version doesn't use pvm_spawn
#                    Use 'NPpvm -r' on receiver and 'NPpvm' on transmitter
#      tcgmsg      : Run directly on TCGMSG
#      tcgmsg-mpich: Test TCGMSG layer on top of mpich
#      lapi        : Test the LAPI interface on the IBM SP
#      gm          : Directly measure raw GM on Myrinet
#                    Use 'NPgm -r' on receiver and 'NPgm -t -h ...' on trans
#      shmem       : Directly measure SHMEM on Cray and SGI systems
#      gpshmem     : Measure GPSHMEM on any other system using shmem.c
#
#      strlen      : Single process measurement of strlen.
#      disk        : Hard disk performance.
#      mask64b     : 64 Bit Mask functions 
#      add64b      : 64 Bit Add functions 
#      div64b      : 64 Bit Divide functions 
#      mul32b      : 32 Bit Multiply functions 
#      mul64b      : 64 Bit Multiply functions 
#      idct        : simple idct, (long long computations).
#
# New compiler defines.
#      SINGLE_PROCESS  : Simple kernel structures.
#      START_SIGNATURE : Starting value for signature curve, default 1.
#
#      For more information, see the function printusage() in netpipe.c

bin_PROGRAMS = \
	nptcp npmemcpy npmemmove npstrlen npdisk \
	npmask64b npadd64b npidct npmul32b npmul64b npdiv64b 

SRC          = $(top_srcdir)/src

# For MPI, mpicc will set up the proper include and library paths

MPICC       = mpicc

MP_Lite_home   = $(HOME)/MP_Lite

PVM_HOME   = /usr/share/pvm3
PVM_ARCH   = LINUX

#
# This section of the Makefile is for compiling the binaries
#


nptcp_SOURCES  = $(SRC)/tcp.c $(SRC)/netpipe.c $(SRC)/netpipe.h
nptcp_CPPFLAGS = -DTCP 

npmemcpy_SOURCES  = $(SRC)/memcpy.c $(SRC)/netpipe.c $(SRC)/netpipe.h
npmemcpy_CPPFLAGS = -DMEMCPY -DSINGLE_PROCESS

npmemmove_SOURCES  = $(SRC)/memmove.c $(SRC)/netpipe.c $(SRC)/netpipe.h
npmemmove_CPPFLAGS = -DMEMMOVE  -DSINGLE_PROCESS

npstrlen_SOURCES  = $(SRC)/strlen.c $(SRC)/netpipe.c  $(SRC)/netpipe.h
npstrlen_CPPFLAGS = -DSTRLEN -DSINGLE_PROCESS

npmask64b_SOURCES  = $(SRC)/mask64b.c $(SRC)/netpipe.c $(SRC)/netpipe.h
npmask64b_CPPFLAGS = -DMASK64B -DSINGLE_PROCESS -DSTART_SIGNATURE=128

npadd64b_SOURCES  = $(SRC)/add64b.c $(SRC)/netpipe.c $(SRC)/netpipe.h
npadd64b_CPPFLAGS = -DSINGLE_PROCESS -DSTART_SIGNATURE=128

npmul32b_SOURCES  = $(SRC)/mul32b.c $(SRC)/netpipe.c $(SRC)/netpipe.h
npmul32b_CPPFLAGS = -DSINGLE_PROCESS -DSTART_SIGNATURE=128

npmul64b_SOURCES  = $(SRC)/mul64b.c $(SRC)/netpipe.c $(SRC)/netpipe.h
npmul64b_CPPFLAGS = -DSINGLE_PROCESS -DSTART_SIGNATURE=128

npdiv64b_SOURCES  = $(SRC)/div64b.c $(SRC)/netpipe.c $(SRC)/netpipe.h
npdiv64b_CPPFLAGS = -DSINGLE_PROCESS -DSTART_SIGNATURE=128

npidct_SOURCES  = $(SRC)/idct.c $(SRC)/netpipe.c $(SRC)/netpipe.h
npidct_CPPFLAGS = -DIDCT -DSINGLE_PROCESS -DSTART_SIGNATURE=128

# SSE specific, many changes to make generic.
#mpmemcpy_SOURCES  = $(SRC)/memcpy.c $(SRC)/netpipe.c $(SRC)/MP_memcpy.c
#mpmemcpy_CPPFLAGS = -mmmx -msse -DMEMCPY -DUSE_MP_MEMCPY
#mpmemcpy_CPPFLAGS = -DMEMCPY -DUSE_MP_MEMCPY

npdisk_SOURCES  = $(SRC)/disk.c $(SRC)/netpipe.c $(SRC)/netpipe.h
npdisk_CPPFLAGS = -DDISK

#sync: $(SRC)/mpi.c $(SRC)/netpipe.c $(SRC)/netpipe.h 
#	( cd ~/mplite; make clean; make sync; )
#	$(CC) $(CFLAGS) -DMPI $(SRC)/netpipe.c $(SRC)/mpi.c -o NPmplite \
#         -I$(SRC) -I$(MP_Lite_home) $(MP_Lite_home)/libmplite.a

#mpi: $(SRC)/mpi.c $(SRC)/netpipe.c $(SRC)/netpipe.h 
#	$(MPICC) $(CFLAGS) -DMPI $(SRC)/netpipe.c $(SRC)/mpi.c -o NPmpi -I$(SRC)
#	@ rm -f netpipe.o mpi.o

#mpipro: $(SRC)/mpi.c $(SRC)/netpipe.c $(SRC)/netpipe.h 
#	mpicc $(CFLAGS) -DMPI $(SRC)/netpipe.c $(SRC)/mpi.c \
#            -o NPmpipro -I$(SRC)
#	@ rm -f netpipe.o mpi.o

#mpipro-gm: $(SRC)/mpi.c $(SRC)/netpipe.c $(SRC)/netpipe.h 
#	cc -O -DMPI $(SRC)/netpipe.c $(SRC)/mpi.c \
#            /usr/lib/libmpipro_tg_i386.a \
#            -o NPmpipro-gm -I./src -I/usr/include \
#            -L $HOME/np/packs/gm/binary/lib -lgm -lm -lpthread

#mpich: $(SRC)/mpi.c $(SRC)/netpipe.c $(SRC)/netpipe.h 
#	mpichcc $(CFLAGS) -DMPI $(SRC)/netpipe.c \
#            $(SRC)/mpi.c -o NPmpich -I$(SRC)
#	@ rm -f netpipe.o mpi.o

#mpich-trace: $(SRC)/mpi.c $(SRC)/netpipe.c $(SRC)/netpipe.h 
#	mpichcc $(CFLAGS) -mpitrace -DMPI $(SRC)/netpipe.c \
#            $(SRC)/mpi.c -o NPmpich -I$(SRC)
#	@ rm -f netpipe.o mpi.o

#mpich-log: $(SRC)/mpi.c $(SRC)/netpipe.c $(SRC)/netpipe.h  
#	mpichcc $(CFLAGS) -mpilog -DMPI $(SRC)/netpipe.c \
#            $(SRC)/mpi.c -o NPmpich -I$(SRC)
#	@ rm -f netpipe.o mpi.o

#mpich-gm: $(SRC)/mpi.c $(SRC)/netpipe.c $(SRC)/netpipe.h 
#	mpichgmcc $(CFLAGS) -DMPI $(SRC)/netpipe.c \
#            $(SRC)/mpi.c -o NPmpich-gm -I$(SRC)
#	@ rm -f netpipe.o mpi.o

#gm: $(SRC)/gm.c $(SRC)/netpipe.c $(SRC)/netpipe.h 
#	$(CC) $(CFLAGS) -DGM $(SRC)/netpipe.c $(SRC)/gm.c \
#            -o NPgm -I$(SRC) -I$(GM_INC) -I$(GM_DRI) \
#            $(GM_LIB) -static

#mvich: $(SRC)/mpi.c $(SRC)/netpipe.c $(SRC)/netpipe.h 
#	mvichcc $(CFLAGS) -DMPI $(SRC)/netpipe.c \
#            $(SRC)/mpi.c -o NPmvich -I$(SRC) -lvipl
#	@ rm -f netpipe.o mpi.o

#mvich-gn: $(SRC)/mpi.c $(SRC)/netpipe.c $(SRC)/netpipe.h 
#	mvichcc $(CFLAGS) -DMPI $(SRC)/netpipe.c \
#            $(SRC)/mpi.c -o NPmvich-gn -I$(SRC) -lgnivipl
#	@ rm -f netpipe.o mpi.o

#mplite MP_Lite sigio: $(SRC)/mpi.c $(SRC)/netpipe.c $(SRC)/netpipe.h 
#	( cd $(MP_Lite_home); make; )
#	$(CC) $(CFLAGS) -DMPI $(SRC)/netpipe.c $(SRC)/mpi.c \
#            -o NPmplite -I$(SRC) -I$(MP_Lite_home) $(MP_Lite_home)/libmplite.a

#mplite-mvia: $(SRC)/mpi.c $(SRC)/netpipe.c $(SRC)/netpipe.h 
#	( cd ~/mplite; make mvia; )
#	$(CC) $(CFLAGS) -DMPI $(SRC)/netpipe.c $(SRC)/mpi.c \
#             -o NPmplite-mvia -I$(SRC) -I$(MP_Lite_home) \
#             $(MP_Lite_home)/libmplite.a -lvipl -lpthread

#mplite-gn: $(SRC)/mpi.c $(SRC)/netpipe.c $(SRC)/netpipe.h 
#	( cd ~/mplite; make giganet; )
#	$(CC) $(CFLAGS) -DMPI $(SRC)/netpipe.c $(SRC)/mpi.c \
#            -o NPmplite-gn -I$(SRC) -I$(MP_Lite_home) \
#            $(MP_Lite_home)/libmplite.a -lgnivipl -lpthread

#mplite-ib: $(SRC)/mpi.c $(SRC)/netpipe.c $(SRC)/netpipe.h 
#	( cd $(MP_Lite_home); make ib; )
#	$(CC) $(CFLAGS) -g -DMPI $(SRC)/netpipe.c $(SRC)/mpi.c \
#            -o NPmplite-ib -I$(SRC) -I$(MP_Lite_home) \
#            $(MP_Lite_home)/libmplite.a -L/usr/mellanox/lib \
#            -lmpga -lvapi -lpthread

#pvm: $(SRC)/pvm.c $(SRC)/netpipe.c $(SRC)/netpipe.h 
#	$(CC) $(CFLAGS) -DPVM $(SRC)/netpipe.c $(SRC)/pvm.c \
#           -o NPpvm -I$(SRC) -I$(PVM_HOME)/include \
#           -L $(PVM_HOME)/lib/$(PVM_ARCH)/ -lpvm3 -lgpvm3

#NPtcgmsg_SOURCES  = $(SRC)/tcgmsg.c $(SRC)/netpipe.c
#NPtcgmsg_CPPFLAGS = -DTCGMSG -I$(TCGMSG_INC)
#NPtcgmsg_LDADD =  $(TCGMSG_LIB) 

#tcgmsg-mpich: $(SRC)/tcgmsg.c $(SRC)/netpipe.c $(SRC)/netpipe.h 
#	mpichcc $(CFLAGS) -DTCGMSG $(SRC)/netpipe.c \
#           $(SRC)/tcgmsg.c -o NPtcgmsg.mpich -I$(SRC) -I$(TCGMSG_MPI_INC) \
#           $(TCGMSG_MPI_LIB) $(TCGMSG_ARMCI_LIB)

#lapi: $(SRC)/lapi.c $(SRC)/netpipe.c $(SRC)/netpipe.h 
#	mpcc_r $(CFLAGS) -DLAPI $(SRC)/netpipe.c \
#           $(SRC)/lapi.c -o NPlapi

#t3e: $(SRC)/shmem.c $(SRC)/netpipe.c $(SRC)/netpipe.h 
#	$(CC) $(CFLAGS) -DSHMEM $(SRC)/netpipe.c \
#           $(SRC)/shmem.c -o NPshmem

#shmem: $(SRC)/shmem.c $(SRC)/netpipe.c $(SRC)/netpipe.h 
#	$(CC) $(CFLAGS) -DSHMEM $(SRC)/netpipe.c \
#           $(SRC)/shmem.c -o NPshmem -lsma
# requires IBM High Performance Computing Toolkit
#NPshmem_SOURCES = $(SRC)/shmem.c $(SRC)/netpipe.c
#NPshmem_CPPFLAGS = -DSHMEM -I$(top_srcdir)
#NPshmem_LDFLAGS = -lsma

#gpshmem_SOURCES  = $(SRC)/gpshmem.c $(SRC)/netpipe.c $(SRC)/netpipe.h 
#gpshmem_CPPFLAGS =-DGPSHMEM -DSHMEM
#           $(SRC)/gpshmem.c -I$(GPSHMEM_INC) -o NPgpshmem $(GPSHMEM_LIB) \
#           $(ARMCI_LIB)

#paragon: $(SRC)/mpi.c $(SRC)/netpipe.c $(SRC)/netpipe.h 
#	$(CC) -nx $(CFLAGS) -DMPI $(SRC)/netpipe.c \
#           $(SRC)/mpi.c -o NPparagon -I$(SRC) -lmpi
#	@ echo "On the Paragon, the buffer alignment does not work."
#	@ echo "Run using NPparagon -A 0."

#armci: $(SRC)/armci.c $(SRC)/netpipe.c $(SRC)/netpipe.h 
#	$(MPICC) $(CFLAGS) -DARMCI $(SRC)/netpipe.c \
#           $(SRC)/armci.c -o NParmci -I$(ARMCI_INC) $(ARMCI_LIB) 

#mpi2: $(SRC)/mpi2.c $(SRC)/netpipe.c $(SRC)/netpipe.h 
#	$(MPI2CC) $(CFLAGS) -DMPI -DMPI2 $(SRC)/netpipe.c \
#           $(SRC)/mpi2.c -o NPmpi2 -I$(MPI2_INC) $(MPI2_LIB)

#ib: $(SRC)/ib.c $(SRC)/netpipe.c $(SRC)/netpipe.h 
#	$(CC) $(CFLAGS) $(SRC)/ib.c $(SRC)/netpipe.c -o NPib \
#        -DINFINIBAND -DTCP -I $(VAPI_INC) -L $(VAPI_LIB) \
#        -lmpga -lvapi -lpthread

#atoll: $(SRC)/atoll.c $(SRC)/netpipe.c $(SRC)/netpipe.h
#	$(CC) $(CFLAGS) -DATOLL $(SRC)/netpipe.c \
#        $(SRC)/atoll.c -o NPatoll \
#        -I$(PALMS_PATH)/include -L$(PALMS_PATH)/lib -latoll

if OBJEXT_NOT_O
.c.$(OBJEXT):
#       $(COMPILE) -MT $@ -MD -MP -MF $(DEPDIR)/$*.Tpo -c -o $@ $<
#       mv -f $(DEPDIR)/$*.Tpo $(DEPDIR)/$*.Po
	source='$<' object='$@' libtool=no \
	DEPDIR=$(DEPDIR) $(CCDEPMODE) $(depcomp) \
	$(COMPILE) -c $<
endif

nobase_dist_pkgdata = \
	bin/feplot  bin/geplot  bin/nplaunch

EXTRA_DIST = README
 
