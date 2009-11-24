lib_LTLIBRARIES = libengine.la
libengineincludedir = $(includedir)/Kyra/engine

#nasty awful horrible but effective hook :P
uninstall-hook:
	rm -rf $(includedir)/Kyra 

#and another
install-data-hook:
	mkdir -p $(DESTDIR)/$(includedir)/Kyra/util
	cp ../util/*.h $(DESTDIR)/$(includedir)/Kyra/util
	cp ../kyra.h $(DESTDIR)/$(includedir)/Kyra
	mkdir -p $(DESTDIR)/$(includedir)/Kyra/gui
	cp ../gui/*.h $(DESTDIR)/$(includedir)/Kyra/gui
	mkdir -p $(DESTDIR)/$(includedir)/Kyra/guiExtended
	cp ../guiExtended/*.h $(DESTDIR)/$(includedir)/Kyra/guiExtended
	mkdir -p $(DESTDIR)/$(includedir)/grinliz
	cp ../../grinliz/*.h $(DESTDIR)/$(includedir)/grinliz
	mkdir -p $(DESTDIR)/$(includedir)/tinyxml
	cp ../../tinyxml/*.h $(DESTDIR)/$(includedir)/tinyxml
 
#and another ... could probably be combined with install-data-hook
install-exec-hook:
	mkdir -p $(DESTDIR)/$(includedir)/Kyra/tinyxml

libengineinclude_HEADERS = action.h \
box.h \
boxresource.h \
canvas.h \
canvasresource.h \
color.h \
dirtyrectangle.h \
encoder.h \
engine.h \
fontresource.h \
image.h \
imagecontainer.h \
imagenode.h \
imagetree.h \
imnode.h \
krmath.h \
kyra.h \
kyrabuild.h \
kyraresource.h \
namefield.h \
ogltexture.h \
painter.h \
parser.h \
pixelblock.h \
rle.h \
sdlutil.h \
sprite.h \
spriteresource.h \
tags.h \
textbox.h \
tile.h \
tileresource.h \
vault.h \
dataresource.h

libengine_la_SOURCES = \
action.cpp \
box.cpp \
boxresource.cpp \
canvas.cpp \
canvasresource.cpp \
color.cpp \
dataresource.cpp \
dirtyrectangle.cpp \
encoder.cpp \
engine.cpp \
fontresource.cpp \
krmath.cpp \
image.cpp \
imagetree.cpp \
imnode.cpp \
kyraresource.cpp \
painter.cpp \
parser.cpp \
pixelblock.cpp \
rle.cpp \
sdlutil.cpp \
sprite.cpp \
spriteresource.cpp \
textbox.cpp \
tile.cpp \
tileresource.cpp \
vault.cpp \
namefield.cpp \
ogltexture.cpp \
../../tinyxml/tinyxml.cpp \
../../tinyxml/tinyxmlparser.cpp \
../../tinyxml/tinystr.cpp \
../../tinyxml/tinyxmlerror.cpp \
../../grinliz/gldebug.cpp \
../../grinliz/glmemorypool.cpp \
../../grinliz/glrandom.cpp \
../../grinliz/glgeometry.cpp \
../../grinliz/glperformance.cpp \
../../grinliz/glutil.cpp \
../../grinliz/glmatrix.cpp \
../../grinliz/glprime.cpp \
../../grinliz/gldynamic.cpp

libengine_la_LIBADD = \
	@SDL_LIBS@
