## Automake commands.
# @author Rick Foos
# @file

AUTOMAKE_OPTIONS = foreign

bin_PROGRAMS = postmark
bin_SCRIPTS = postmark-run.sh
data_DATA = postmark-cmds.txt

postmark_SOURCES = postmark-1_5.c
	
EXTRA_DIST = autogen.sh $(bin_SCRIPTS) $(data_DATA)
