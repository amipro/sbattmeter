## Process this file with automake to produce Makefile.in

## Created by Anjuta

uidir = $(datadir)/sbattmeter/ui
ui_DATA = 

AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	$(SBATTMETER_CFLAGS)

AM_CFLAGS =\
	 -Wall\
	 -g

bin_PROGRAMS = sbattmeter

sbattmeter_SOURCES = \
	main.c

sbattmeter_LDFLAGS = \
	-Wl,--export-dynamic

sbattmeter_LDADD = $(SBATTMETER_LIBS)

EXTRA_DIST = $(ui_DATA)

# Remove ui directory on uninstall
uninstall-local:
	-rm -r $(uidir)
	-rm -r $(datadir)/sbattmeter
