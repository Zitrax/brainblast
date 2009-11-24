#****************************************************************************
#
# Makefile for Kyra.
# Lee Thomason and David Leimbach
# www.grinninglizard.com
# www.sourceforge.net/projects/kyra
#
# This is a GNU make (gmake) makefile
#****************************************************************************

include ../makefilebuild

OUTPUT := krdemo

all: ${OUTPUT}


#****************************************************************************
# Source files
#****************************************************************************

SRCS :=	bemgame.cpp \
		demos.cpp \
		space.cpp \
		spritetest.cpp \
		tiletest.cpp \
		gamefactory.cpp \
        scale.cpp \
        sinwave.cpp \
		text.cpp \
		title.cpp \
		shooter.cpp

ifeq (YES, ${PROFILE})
   SRCS   := ${SRCS} \
		../engine/action.cpp \
		../engine/box.cpp \
		../engine/boxresource.cpp \
		../engine/canvas.cpp \
		../engine/canvasresource.cpp \
		../engine/color.cpp \
		../util/gldebug.cpp \
		../engine/dirtyrectangle.cpp \
		../engine/encoder.cpp \
		../engine/engine.cpp \
		../engine/fontresource.cpp \
		../engine/image.cpp \
		../engine/imagetree.cpp \
		../engine/imnode.cpp \
		../engine/kyraresource.cpp \
		../engine/ogltexture.cpp \
		../engine/painter.cpp \
		../engine/parser.cpp \
		../engine/pixelblock.cpp \
		../engine/rle.cpp \
		../engine/sdlutil.cpp \
		../engine/sprite.cpp \
		../engine/spriteresource.cpp \
		../engine/textbox.cpp \
		../engine/tile.cpp \
		../engine/tileresource.cpp \
		../engine/vault.cpp \
        ../engine/namefield.cpp \
		../engine/krmath.cpp \
		../util/glrandom.cpp \
		../util/glmap.cpp \
        ../util/glisomath.cpp \
		../util/glstring.cpp \
        ../util/glmemorypool.cpp \
		../util/glprime.cpp \
		../util/glperformance.cpp \
		../tinyxml/tinyxml.cpp \
		../tinyxml/tinyxmlparser.cpp \
		../tinyxml/tinyxmlerror.cpp \
		../gui/console.cpp \
		../gui/widget.cpp
endif

# Add on the sources for libraries
SRCS := ${SRCS}

OBJS := $(addsuffix .o,$(basename ${SRCS}))

#****************************************************************************
# Output
#****************************************************************************

${OUTPUT}: ${OBJS} ../engine/libkyra.a
	${LD} -o $@ ${LDFLAGS} ${OBJS} ../engine/libkyra.a ${LIBS} ${EXTRA_LIBS}
	
#****************************************************************************
# common rules
#****************************************************************************

# Rules for compiling source files to object files
%.o : %.cpp
	${CXX} -c ${CXXFLAGS} ${INCS} $< -o $@

%.o : %.c
	${CC} -c ${CFLAGS} ${INCS} $< -o $@

clean:
	-rm -f core ${OBJS} ${OUTPUT}

# Dependencies. Badly over included, but there were just too many problems
# using makedepend and sharing the makefiles.

include ../makefiledefines
${OBJS} :  $(addprefix ../engine/, ${ENGINE_HEADERS}) $(addprefix ../util/, ${UTIL_HEADERS}) $(addprefix ../gui/, ${GUI_HEADERS}) \
           demos.h standardtest.h bem.h consolefont.h font.h space.h
