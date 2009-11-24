SUBDIRS = engine gui util guiExtended tinyxml encoder spriteed
bin_SCRIPTS = kyra-config 

EXTRA_DIST = ltconfig kyra.m4 kyra.h

# M4 macro file for inclusion with autoconf
m4datadir = $(datadir)/aclocal
m4data_DATA = kyra.m4

lib_LTLIBRARIES = libkyra.la
#nasty awful horrible but effective hook :P
uninstall-hook:
	rm -rf $(includedir)/Kyra 

libkyra_la_SOURCES = 

#and another
install-data-hook:
	mkdir -p $(includedir)/Kyra/util
	cp util/*.h $(includedir)/Kyra/util
	cp kyra.h $(includedir)/Kyra
	mkdir -p $(includedir)/Kyra/gui
	cp gui/*.h $(includedir)/Kyra/gui
	mkdir -p $(includedir)/Kyra/guiExtended
	cp guiExtended/*.h $(includedir)/Kyra/guiExtended
 
#and another ... could probably be combined with install-data-hook
install-exec-hook:

libkyra_la_LIBADD = \
	util/libutil.la \
	gui/libgui.la \
	guiExtended/libguiExtended.la \
	engine/libengine.la \
	@SDL_LIBS@
