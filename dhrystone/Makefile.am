## Automake definitions
# @author Rick Foos
# @file

bin_PROGRAMS = dhrystone
bin_SCRIPTS = dhrystone-run.sh

# Standard Dhrystone flags
AM_CPPFLAGS = -O2 -DHZ=60 -DREG=register -DTIME -DNDEBUG 


dhrystone_SOURCES = \
		dhry_1.c \
		dhry_2.c \
		dhry.h

if OBJEXT_NOT_O
.c.$(OBJEXT):
#       $(COMPILE) -MT $@ -MD -MP -MF $(DEPDIR)/$*.Tpo -c -o $@ $<
#       mv -f $(DEPDIR)/$*.Tpo $(DEPDIR)/$*.Po
	source='$<' object='$@' libtool=no \
	DEPDIR=$(DEPDIR) $(CCDEPMODE) $(depcomp) \
	$(COMPILE) -c $<
endif

EXTRA_DIST = autogen.sh $(bin_SCRIPTS)

