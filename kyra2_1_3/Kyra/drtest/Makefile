#****************************************************************************
#
# Makefile for Kyra.
# Lee Thomason
# www.grinninglizard.com
#
# This is a GNU make (gmake) makefile
#****************************************************************************

# DEBUG can be set to YES to include debugging info, or NO otherwise
DEBUG          := YES

# PROFILE can be set to YES to include profiling info, or NO otherwise
PROFILE        := NO

#****************************************************************************

CC     := gcc
CXX    := g++
LD     := g++
AR     := ar rc
RANLIB := ranlib

DEBUG_CFLAGS     := -Wall -Wno-unknown-pragmas -Wno-format -g -DDEBUG
RELEASE_CFLAGS   := -Wall -Wno-unknown-pragmas -Wno-format

LIBS		 :=   -lSDL -lSDL_image

DEBUG_CXXFLAGS   := ${DEBUG_CFLAGS}
RELEASE_CXXFLAGS := ${RELEASE_CFLAGS}

DEBUG_LDFLAGS    := -g
RELEASE_LDFLAGS  :=

ifeq (YES, ${DEBUG})
   CFLAGS       := ${DEBUG_CFLAGS}
   CXXFLAGS     := ${DEBUG_CXXFLAGS}
   LDFLAGS      := ${DEBUG_LDFLAGS}
else
   CFLAGS       := ${RELEASE_CFLAGS}
   CXXFLAGS     := ${RELEASE_CXXFLAGS}
   LDFLAGS      := ${RELEASE_LDFLAGS}
endif

ifeq (YES, ${PROFILE})
   CFLAGS   := ${CFLAGS} -pg
   CXXFLAGS := ${CXXFLAGS} -pg
   LDFLAGS  := ${LDFLAGS} -pg
else
   ifeq (NO, ${DEBUG})
       CFLAGS   := ${CFLAGS} -O2
       CXXFLAGS := ${CXXFLAGS} -O2
   endif
endif

#****************************************************************************
# Preprocessor directives
#****************************************************************************

ifeq (YES, ${PROFILE})
  DEFS :=
else
  DEFS :=
endif

#****************************************************************************
# Include paths
#****************************************************************************

INCS := -I"/usr/include/SDL"


#****************************************************************************
# Makefile code common to all platforms
#****************************************************************************

CFLAGS   := ${CFLAGS}   ${DEFS}
CXXFLAGS := ${CXXFLAGS} ${DEFS}

#****************************************************************************
# Targets of the build
#****************************************************************************

OUTPUT := drtest

all: ${OUTPUT}


#****************************************************************************
# Source files
#****************************************************************************

SRCS :=	drtest.cpp \
		../engine/action.cpp \
		../engine/canvas.cpp \
		../engine/canvasresource.cpp \
		../engine/color.cpp \
		../engine/debug.cpp \
		../engine/dirtyrectangle.cpp \
		../engine/encoder.cpp \
		../engine/engine.cpp \
		../engine/image.cpp \
		../engine/imagetree.cpp \
		../engine/imnode.cpp \
		../engine/kyraresource.cpp \
		../engine/painter.cpp \
		../engine/parser.cpp \
		../engine/pixelblock.cpp \
		../engine/rle.cpp \
		../engine/sdlutil.cpp \
		../engine/sprite.cpp \
		../engine/spriteresource.cpp \
		../engine/tile.cpp \
		../engine/tileresource.cpp \
		../engine/userimage.cpp \
		../engine/vault.cpp \
		../util/glintmap.cpp \
		../util/glmath.cpp \
		../util/glrandom.cpp \
		../util/glstring.cpp \
		../util/glstringmap.cpp \
		../tinyxml/tinyxml.cpp \
		../tinyxml/tinyxmlparser.cpp
				
# Add on the sources for libraries
SRCS := ${SRCS}

OBJS := $(addsuffix .o,$(basename ${SRCS}))

#****************************************************************************
# Output
#****************************************************************************

${OUTPUT}: ${OBJS}
	${LD} -o $@ ${LDFLAGS} ${OBJS} ${LIBS} ${EXTRA_LIBS}

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

depend:
	makedepend ${INCS} ${SRCS}
# DO NOT DELETE

drtest.o: /usr/include/stdlib.h /usr/include/features.h
drtest.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
drtest.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stddef.h
drtest.o: /usr/include/sys/types.h /usr/include/bits/types.h
drtest.o: /usr/include/time.h /usr/include/endian.h
drtest.o: /usr/include/bits/endian.h /usr/include/sys/select.h
drtest.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
drtest.o: /usr/include/sys/sysmacros.h /usr/include/alloca.h
drtest.o: /usr/include/stdio.h
drtest.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stdarg.h
drtest.o: /usr/include/libio.h /usr/include/_G_config.h
drtest.o: /usr/include/bits/stdio_lim.h /usr/include/SDL/SDL.h
drtest.o: /usr/include/SDL/SDL_main.h /usr/include/SDL/SDL_types.h
drtest.o: /usr/include/SDL/SDL_error.h /usr/include/SDL/begin_code.h
drtest.o: /usr/include/SDL/close_code.h /usr/include/SDL/SDL_rwops.h
drtest.o: /usr/include/SDL/SDL_timer.h /usr/include/SDL/SDL_audio.h
drtest.o: /usr/include/SDL/SDL_byteorder.h /usr/include/SDL/SDL_cdrom.h
drtest.o: /usr/include/SDL/SDL_events.h /usr/include/SDL/SDL_active.h
drtest.o: /usr/include/SDL/SDL_keyboard.h /usr/include/SDL/SDL_keysym.h
drtest.o: /usr/include/SDL/SDL_mouse.h /usr/include/SDL/SDL_video.h
drtest.o: /usr/include/SDL/SDL_mutex.h /usr/include/SDL/SDL_quit.h
drtest.o: /usr/include/SDL/SDL_version.h ../engine/dirtyrectangle.h
drtest.o: ../util/glmath.h ../util/kyrautil.h ../util/dynarray.h
drtest.o: /usr/include/limits.h /usr/include/bits/posix1_lim.h
drtest.o: /usr/include/bits/local_lim.h /usr/include/linux/limits.h
drtest.o: /usr/include/bits/posix2_lim.h ../engine/debug.h ../engine/types.h
drtest.o: ../engine/painter.h ../engine/types.h ../engine/color.h
../engine/action.o: /usr/include/SDL/SDL_endian.h /usr/include/stdio.h
../engine/action.o: /usr/include/features.h /usr/include/sys/cdefs.h
../engine/action.o: /usr/include/gnu/stubs.h
../engine/action.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stddef.h
../engine/action.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stdarg.h
../engine/action.o: /usr/include/bits/types.h /usr/include/libio.h
../engine/action.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
../engine/action.o: /usr/include/SDL/SDL_types.h /usr/include/SDL/SDL_rwops.h
../engine/action.o: /usr/include/SDL/begin_code.h
../engine/action.o: /usr/include/SDL/close_code.h
../engine/action.o: /usr/include/SDL/SDL_byteorder.h ../engine/action.h
../engine/action.o: /usr/include/SDL/SDL.h /usr/include/SDL/SDL_main.h
../engine/action.o: /usr/include/SDL/SDL_error.h /usr/include/SDL/SDL_timer.h
../engine/action.o: /usr/include/SDL/SDL_audio.h /usr/include/SDL/SDL_cdrom.h
../engine/action.o: /usr/include/SDL/SDL_events.h
../engine/action.o: /usr/include/SDL/SDL_active.h
../engine/action.o: /usr/include/SDL/SDL_keyboard.h
../engine/action.o: /usr/include/SDL/SDL_keysym.h
../engine/action.o: /usr/include/SDL/SDL_mouse.h /usr/include/SDL/SDL_video.h
../engine/action.o: /usr/include/SDL/SDL_mutex.h /usr/include/SDL/SDL_quit.h
../engine/action.o: /usr/include/SDL/SDL_version.h ../engine/rle.h
../engine/action.o: ../engine/types.h ../engine/debug.h ../util/glmath.h
../engine/action.o: ../util/kyrautil.h ../util/dynarray.h
../engine/action.o: /usr/include/limits.h /usr/include/bits/posix1_lim.h
../engine/action.o: /usr/include/bits/local_lim.h /usr/include/linux/limits.h
../engine/action.o: /usr/include/bits/posix2_lim.h ../engine/debug.h
../engine/action.o: ../engine/types.h ../engine/painter.h ../engine/color.h
../engine/action.o: ../util/dynarray.h ../engine/sdlutil.h ../util/glstring.h
../engine/canvas.o: ../engine/canvas.h ../engine/image.h ../engine/imnode.h
../engine/canvas.o: ../util/insidelist.h ../engine/debug.h
../engine/canvas.o: /usr/include/SDL/SDL.h /usr/include/SDL/SDL_main.h
../engine/canvas.o: /usr/include/SDL/SDL_types.h /usr/include/SDL/SDL_error.h
../engine/canvas.o: /usr/include/SDL/begin_code.h
../engine/canvas.o: /usr/include/SDL/close_code.h
../engine/canvas.o: /usr/include/SDL/SDL_rwops.h /usr/include/stdio.h
../engine/canvas.o: /usr/include/features.h /usr/include/sys/cdefs.h
../engine/canvas.o: /usr/include/gnu/stubs.h
../engine/canvas.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stddef.h
../engine/canvas.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stdarg.h
../engine/canvas.o: /usr/include/bits/types.h /usr/include/libio.h
../engine/canvas.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
../engine/canvas.o: /usr/include/SDL/SDL_timer.h /usr/include/SDL/SDL_audio.h
../engine/canvas.o: /usr/include/SDL/SDL_byteorder.h
../engine/canvas.o: /usr/include/SDL/SDL_cdrom.h
../engine/canvas.o: /usr/include/SDL/SDL_events.h
../engine/canvas.o: /usr/include/SDL/SDL_active.h
../engine/canvas.o: /usr/include/SDL/SDL_keyboard.h
../engine/canvas.o: /usr/include/SDL/SDL_keysym.h
../engine/canvas.o: /usr/include/SDL/SDL_mouse.h /usr/include/SDL/SDL_video.h
../engine/canvas.o: /usr/include/SDL/SDL_mutex.h /usr/include/SDL/SDL_quit.h
../engine/canvas.o: /usr/include/SDL/SDL_version.h ../util/glmath.h
../engine/canvas.o: ../util/kyrautil.h ../util/dynarray.h
../engine/canvas.o: /usr/include/limits.h /usr/include/bits/posix1_lim.h
../engine/canvas.o: /usr/include/bits/local_lim.h /usr/include/linux/limits.h
../engine/canvas.o: /usr/include/bits/posix2_lim.h ../engine/types.h
../engine/canvas.o: ../engine/canvasresource.h ../engine/kyraresource.h
../engine/canvas.o: ../util/glstring.h ../engine/types.h ../util/kyrautil.h
../engine/canvas.o: ../util/circlelist.h ../engine/pixelblock.h
../engine/canvas.o: ../engine/debug.h ../engine/painter.h ../engine/color.h
../engine/canvas.o: ../engine/engine.h ../engine/vault.h ../util/kyralist.h
../engine/canvas.o: ../util/dynarray.h ../engine/rle.h ../engine/imagetree.h
../engine/canvas.o: ../util/glintmap.h ../util/glstringmap.h
../engine/canvas.o: ../engine/imagecontainer.h ../engine/imagenode.h
../engine/canvas.o: ../engine/sprite.h ../engine/spriteresource.h
../engine/canvas.o: ../engine/action.h ../engine/dirtyrectangle.h
../engine/canvasresource.o: ../engine/canvasresource.h
../engine/canvasresource.o: ../engine/kyraresource.h ../util/glstring.h
../engine/canvasresource.o: ../engine/types.h ../util/dynarray.h
../engine/canvasresource.o: /usr/include/limits.h /usr/include/features.h
../engine/canvasresource.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
../engine/canvasresource.o: /usr/include/bits/posix1_lim.h
../engine/canvasresource.o: /usr/include/bits/local_lim.h
../engine/canvasresource.o: /usr/include/linux/limits.h
../engine/canvasresource.o: /usr/include/bits/posix2_lim.h ../util/kyrautil.h
../engine/canvasresource.o: ../engine/debug.h ../engine/types.h
../engine/canvasresource.o: ../util/kyrautil.h ../util/glmath.h
../engine/canvasresource.o: ../util/circlelist.h ../engine/pixelblock.h
../engine/canvasresource.o: ../engine/debug.h /usr/include/SDL/SDL.h
../engine/canvasresource.o: /usr/include/SDL/SDL_main.h
../engine/canvasresource.o: /usr/include/SDL/SDL_types.h
../engine/canvasresource.o: /usr/include/SDL/SDL_error.h
../engine/canvasresource.o: /usr/include/SDL/begin_code.h
../engine/canvasresource.o: /usr/include/SDL/close_code.h
../engine/canvasresource.o: /usr/include/SDL/SDL_rwops.h /usr/include/stdio.h
../engine/canvasresource.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stddef.h
../engine/canvasresource.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stdarg.h
../engine/canvasresource.o: /usr/include/bits/types.h /usr/include/libio.h
../engine/canvasresource.o: /usr/include/_G_config.h
../engine/canvasresource.o: /usr/include/bits/stdio_lim.h
../engine/canvasresource.o: /usr/include/SDL/SDL_timer.h
../engine/canvasresource.o: /usr/include/SDL/SDL_audio.h
../engine/canvasresource.o: /usr/include/SDL/SDL_byteorder.h
../engine/canvasresource.o: /usr/include/SDL/SDL_cdrom.h
../engine/canvasresource.o: /usr/include/SDL/SDL_events.h
../engine/canvasresource.o: /usr/include/SDL/SDL_active.h
../engine/canvasresource.o: /usr/include/SDL/SDL_keyboard.h
../engine/canvasresource.o: /usr/include/SDL/SDL_keysym.h
../engine/canvasresource.o: /usr/include/SDL/SDL_mouse.h
../engine/canvasresource.o: /usr/include/SDL/SDL_video.h
../engine/canvasresource.o: /usr/include/SDL/SDL_mutex.h
../engine/canvasresource.o: /usr/include/SDL/SDL_quit.h
../engine/canvasresource.o: /usr/include/SDL/SDL_version.h
../engine/canvasresource.o: ../engine/painter.h ../engine/color.h
../engine/canvasresource.o: ../engine/canvas.h ../engine/image.h
../engine/canvasresource.o: ../engine/imnode.h ../util/insidelist.h
../engine/color.o: ../engine/color.h ../engine/types.h ../engine/debug.h
../engine/color.o: /usr/include/stdlib.h /usr/include/features.h
../engine/color.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
../engine/color.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stddef.h
../engine/color.o: /usr/include/sys/types.h /usr/include/bits/types.h
../engine/color.o: /usr/include/time.h /usr/include/endian.h
../engine/color.o: /usr/include/bits/endian.h /usr/include/sys/select.h
../engine/color.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
../engine/color.o: /usr/include/sys/sysmacros.h /usr/include/alloca.h
../engine/debug.o: ../engine/debug.h
../engine/dirtyrectangle.o: ../engine/dirtyrectangle.h ../util/glmath.h
../engine/dirtyrectangle.o: ../util/kyrautil.h ../util/dynarray.h
../engine/dirtyrectangle.o: /usr/include/limits.h /usr/include/features.h
../engine/dirtyrectangle.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
../engine/dirtyrectangle.o: /usr/include/bits/posix1_lim.h
../engine/dirtyrectangle.o: /usr/include/bits/local_lim.h
../engine/dirtyrectangle.o: /usr/include/linux/limits.h
../engine/dirtyrectangle.o: /usr/include/bits/posix2_lim.h ../engine/debug.h
../engine/dirtyrectangle.o: ../engine/types.h ../engine/painter.h
../engine/dirtyrectangle.o: /usr/include/SDL/SDL.h
../engine/dirtyrectangle.o: /usr/include/SDL/SDL_main.h
../engine/dirtyrectangle.o: /usr/include/SDL/SDL_types.h
../engine/dirtyrectangle.o: /usr/include/SDL/SDL_error.h
../engine/dirtyrectangle.o: /usr/include/SDL/begin_code.h
../engine/dirtyrectangle.o: /usr/include/SDL/close_code.h
../engine/dirtyrectangle.o: /usr/include/SDL/SDL_rwops.h /usr/include/stdio.h
../engine/dirtyrectangle.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stddef.h
../engine/dirtyrectangle.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stdarg.h
../engine/dirtyrectangle.o: /usr/include/bits/types.h /usr/include/libio.h
../engine/dirtyrectangle.o: /usr/include/_G_config.h
../engine/dirtyrectangle.o: /usr/include/bits/stdio_lim.h
../engine/dirtyrectangle.o: /usr/include/SDL/SDL_timer.h
../engine/dirtyrectangle.o: /usr/include/SDL/SDL_audio.h
../engine/dirtyrectangle.o: /usr/include/SDL/SDL_byteorder.h
../engine/dirtyrectangle.o: /usr/include/SDL/SDL_cdrom.h
../engine/dirtyrectangle.o: /usr/include/SDL/SDL_events.h
../engine/dirtyrectangle.o: /usr/include/SDL/SDL_active.h
../engine/dirtyrectangle.o: /usr/include/SDL/SDL_keyboard.h
../engine/dirtyrectangle.o: /usr/include/SDL/SDL_keysym.h
../engine/dirtyrectangle.o: /usr/include/SDL/SDL_mouse.h
../engine/dirtyrectangle.o: /usr/include/SDL/SDL_video.h
../engine/dirtyrectangle.o: /usr/include/SDL/SDL_mutex.h
../engine/dirtyrectangle.o: /usr/include/SDL/SDL_quit.h
../engine/dirtyrectangle.o: /usr/include/SDL/SDL_version.h ../engine/types.h
../engine/dirtyrectangle.o: ../engine/color.h ../util/kyralist.h
../engine/encoder.o: ../engine/encoder.h /usr/include/SDL/SDL.h
../engine/encoder.o: /usr/include/SDL/SDL_main.h /usr/include/SDL/SDL_types.h
../engine/encoder.o: /usr/include/SDL/SDL_error.h
../engine/encoder.o: /usr/include/SDL/begin_code.h
../engine/encoder.o: /usr/include/SDL/close_code.h
../engine/encoder.o: /usr/include/SDL/SDL_rwops.h /usr/include/stdio.h
../engine/encoder.o: /usr/include/features.h /usr/include/sys/cdefs.h
../engine/encoder.o: /usr/include/gnu/stubs.h
../engine/encoder.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stddef.h
../engine/encoder.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stdarg.h
../engine/encoder.o: /usr/include/bits/types.h /usr/include/libio.h
../engine/encoder.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
../engine/encoder.o: /usr/include/SDL/SDL_timer.h
../engine/encoder.o: /usr/include/SDL/SDL_audio.h
../engine/encoder.o: /usr/include/SDL/SDL_byteorder.h
../engine/encoder.o: /usr/include/SDL/SDL_cdrom.h
../engine/encoder.o: /usr/include/SDL/SDL_events.h
../engine/encoder.o: /usr/include/SDL/SDL_active.h
../engine/encoder.o: /usr/include/SDL/SDL_keyboard.h
../engine/encoder.o: /usr/include/SDL/SDL_keysym.h
../engine/encoder.o: /usr/include/SDL/SDL_mouse.h
../engine/encoder.o: /usr/include/SDL/SDL_video.h
../engine/encoder.o: /usr/include/SDL/SDL_mutex.h /usr/include/SDL/SDL_quit.h
../engine/encoder.o: /usr/include/SDL/SDL_version.h ../tinyxml/tinyxml.h
../engine/encoder.o: /usr/include/assert.h ../engine/types.h
../engine/encoder.o: ../engine/color.h ../engine/types.h ../util/kyralist.h
../engine/encoder.o: ../engine/debug.h /usr/include/SDL/SDL_image.h
../engine/encoder.o: ../engine/painter.h ../engine/color.h ../util/glmath.h
../engine/encoder.o: ../util/kyrautil.h ../util/dynarray.h
../engine/encoder.o: /usr/include/limits.h /usr/include/bits/posix1_lim.h
../engine/encoder.o: /usr/include/bits/local_lim.h
../engine/encoder.o: /usr/include/linux/limits.h
../engine/encoder.o: /usr/include/bits/posix2_lim.h ../engine/parser.h
../engine/encoder.o: ../util/dynarray.h /usr/include/SDL/SDL_endian.h
../engine/encoder.o: ../engine/rle.h ../engine/debug.h ../engine/painter.h
../engine/encoder.o: ../engine/sdlutil.h ../util/glstring.h
../engine/encoder.o: ../engine/pixelblock.h ../engine/canvasresource.h
../engine/encoder.o: ../engine/kyraresource.h ../util/kyrautil.h
../engine/encoder.o: ../util/circlelist.h ../engine/pixelblock.h
../engine/engine.o: ../engine/engine.h /usr/include/SDL/SDL.h
../engine/engine.o: /usr/include/SDL/SDL_main.h /usr/include/SDL/SDL_types.h
../engine/engine.o: /usr/include/SDL/SDL_error.h
../engine/engine.o: /usr/include/SDL/begin_code.h
../engine/engine.o: /usr/include/SDL/close_code.h
../engine/engine.o: /usr/include/SDL/SDL_rwops.h /usr/include/stdio.h
../engine/engine.o: /usr/include/features.h /usr/include/sys/cdefs.h
../engine/engine.o: /usr/include/gnu/stubs.h
../engine/engine.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stddef.h
../engine/engine.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stdarg.h
../engine/engine.o: /usr/include/bits/types.h /usr/include/libio.h
../engine/engine.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
../engine/engine.o: /usr/include/SDL/SDL_timer.h /usr/include/SDL/SDL_audio.h
../engine/engine.o: /usr/include/SDL/SDL_byteorder.h
../engine/engine.o: /usr/include/SDL/SDL_cdrom.h
../engine/engine.o: /usr/include/SDL/SDL_events.h
../engine/engine.o: /usr/include/SDL/SDL_active.h
../engine/engine.o: /usr/include/SDL/SDL_keyboard.h
../engine/engine.o: /usr/include/SDL/SDL_keysym.h
../engine/engine.o: /usr/include/SDL/SDL_mouse.h /usr/include/SDL/SDL_video.h
../engine/engine.o: /usr/include/SDL/SDL_mutex.h /usr/include/SDL/SDL_quit.h
../engine/engine.o: /usr/include/SDL/SDL_version.h ../engine/vault.h
../engine/engine.o: ../util/kyrautil.h ../util/kyralist.h ../engine/debug.h
../engine/engine.o: ../util/dynarray.h ../util/glstring.h ../engine/types.h
../engine/engine.o: ../util/dynarray.h /usr/include/limits.h
../engine/engine.o: /usr/include/bits/posix1_lim.h
../engine/engine.o: /usr/include/bits/local_lim.h /usr/include/linux/limits.h
../engine/engine.o: /usr/include/bits/posix2_lim.h ../util/kyrautil.h
../engine/engine.o: ../engine/rle.h ../engine/types.h ../engine/debug.h
../engine/engine.o: ../util/glmath.h ../engine/painter.h ../engine/color.h
../engine/engine.o: ../engine/imagetree.h ../util/glintmap.h
../engine/engine.o: ../util/glstringmap.h ../engine/imagecontainer.h
../engine/engine.o: ../engine/image.h ../engine/imnode.h ../util/insidelist.h
../engine/engine.o: ../engine/imagenode.h ../engine/sprite.h
../engine/engine.o: ../engine/spriteresource.h ../engine/kyraresource.h
../engine/engine.o: ../engine/action.h ../engine/dirtyrectangle.h
../engine/image.o: ../engine/image.h ../engine/imnode.h ../util/insidelist.h
../engine/image.o: ../engine/debug.h /usr/include/SDL/SDL.h
../engine/image.o: /usr/include/SDL/SDL_main.h /usr/include/SDL/SDL_types.h
../engine/image.o: /usr/include/SDL/SDL_error.h /usr/include/SDL/begin_code.h
../engine/image.o: /usr/include/SDL/close_code.h /usr/include/SDL/SDL_rwops.h
../engine/image.o: /usr/include/stdio.h /usr/include/features.h
../engine/image.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
../engine/image.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stddef.h
../engine/image.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stdarg.h
../engine/image.o: /usr/include/bits/types.h /usr/include/libio.h
../engine/image.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
../engine/image.o: /usr/include/SDL/SDL_timer.h /usr/include/SDL/SDL_audio.h
../engine/image.o: /usr/include/SDL/SDL_byteorder.h
../engine/image.o: /usr/include/SDL/SDL_cdrom.h /usr/include/SDL/SDL_events.h
../engine/image.o: /usr/include/SDL/SDL_active.h
../engine/image.o: /usr/include/SDL/SDL_keyboard.h
../engine/image.o: /usr/include/SDL/SDL_keysym.h /usr/include/SDL/SDL_mouse.h
../engine/image.o: /usr/include/SDL/SDL_video.h /usr/include/SDL/SDL_mutex.h
../engine/image.o: /usr/include/SDL/SDL_quit.h /usr/include/SDL/SDL_version.h
../engine/image.o: ../util/glmath.h ../util/kyrautil.h ../util/dynarray.h
../engine/image.o: /usr/include/limits.h /usr/include/bits/posix1_lim.h
../engine/image.o: /usr/include/bits/local_lim.h /usr/include/linux/limits.h
../engine/image.o: /usr/include/bits/posix2_lim.h ../engine/types.h
../engine/image.o: ../engine/engine.h ../engine/vault.h ../util/kyrautil.h
../engine/image.o: ../util/kyralist.h ../util/dynarray.h ../util/glstring.h
../engine/image.o: ../engine/rle.h ../engine/types.h ../engine/debug.h
../engine/image.o: ../engine/painter.h ../engine/color.h
../engine/image.o: ../engine/imagetree.h ../util/glintmap.h
../engine/image.o: ../util/glstringmap.h ../engine/imagecontainer.h
../engine/image.o: ../engine/imagenode.h ../engine/sprite.h
../engine/image.o: ../engine/spriteresource.h ../engine/kyraresource.h
../engine/image.o: ../engine/action.h ../engine/dirtyrectangle.h
../engine/imagetree.o: ../engine/imagetree.h ../util/kyralist.h
../engine/imagetree.o: ../engine/debug.h /usr/include/SDL/SDL.h
../engine/imagetree.o: /usr/include/SDL/SDL_main.h
../engine/imagetree.o: /usr/include/SDL/SDL_types.h
../engine/imagetree.o: /usr/include/SDL/SDL_error.h
../engine/imagetree.o: /usr/include/SDL/begin_code.h
../engine/imagetree.o: /usr/include/SDL/close_code.h
../engine/imagetree.o: /usr/include/SDL/SDL_rwops.h /usr/include/stdio.h
../engine/imagetree.o: /usr/include/features.h /usr/include/sys/cdefs.h
../engine/imagetree.o: /usr/include/gnu/stubs.h
../engine/imagetree.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stddef.h
../engine/imagetree.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stdarg.h
../engine/imagetree.o: /usr/include/bits/types.h /usr/include/libio.h
../engine/imagetree.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
../engine/imagetree.o: /usr/include/SDL/SDL_timer.h
../engine/imagetree.o: /usr/include/SDL/SDL_audio.h
../engine/imagetree.o: /usr/include/SDL/SDL_byteorder.h
../engine/imagetree.o: /usr/include/SDL/SDL_cdrom.h
../engine/imagetree.o: /usr/include/SDL/SDL_events.h
../engine/imagetree.o: /usr/include/SDL/SDL_active.h
../engine/imagetree.o: /usr/include/SDL/SDL_keyboard.h
../engine/imagetree.o: /usr/include/SDL/SDL_keysym.h
../engine/imagetree.o: /usr/include/SDL/SDL_mouse.h
../engine/imagetree.o: /usr/include/SDL/SDL_video.h
../engine/imagetree.o: /usr/include/SDL/SDL_mutex.h
../engine/imagetree.o: /usr/include/SDL/SDL_quit.h
../engine/imagetree.o: /usr/include/SDL/SDL_version.h ../util/glmath.h
../engine/imagetree.o: ../util/kyrautil.h ../util/dynarray.h
../engine/imagetree.o: /usr/include/limits.h /usr/include/bits/posix1_lim.h
../engine/imagetree.o: /usr/include/bits/local_lim.h
../engine/imagetree.o: /usr/include/linux/limits.h
../engine/imagetree.o: /usr/include/bits/posix2_lim.h ../engine/types.h
../engine/imagetree.o: ../util/glintmap.h ../util/glstringmap.h
../engine/imagetree.o: ../engine/imagecontainer.h ../engine/image.h
../engine/imagetree.o: ../engine/imnode.h ../util/insidelist.h
../engine/imagetree.o: ../engine/imagenode.h ../engine/sprite.h
../engine/imagetree.o: ../engine/spriteresource.h ../util/glstring.h
../engine/imagetree.o: ../engine/kyraresource.h ../engine/types.h
../engine/imagetree.o: ../util/kyrautil.h ../engine/action.h ../engine/rle.h
../engine/imagetree.o: ../engine/debug.h ../engine/painter.h
../engine/imagetree.o: ../engine/color.h ../util/dynarray.h
../engine/imagetree.o: ../engine/engine.h ../engine/vault.h
../engine/imagetree.o: ../engine/dirtyrectangle.h
../engine/imnode.o: ../engine/imnode.h ../util/insidelist.h ../engine/debug.h
../engine/imnode.o: /usr/include/SDL/SDL.h /usr/include/SDL/SDL_main.h
../engine/imnode.o: /usr/include/SDL/SDL_types.h /usr/include/SDL/SDL_error.h
../engine/imnode.o: /usr/include/SDL/begin_code.h
../engine/imnode.o: /usr/include/SDL/close_code.h
../engine/imnode.o: /usr/include/SDL/SDL_rwops.h /usr/include/stdio.h
../engine/imnode.o: /usr/include/features.h /usr/include/sys/cdefs.h
../engine/imnode.o: /usr/include/gnu/stubs.h
../engine/imnode.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stddef.h
../engine/imnode.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stdarg.h
../engine/imnode.o: /usr/include/bits/types.h /usr/include/libio.h
../engine/imnode.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
../engine/imnode.o: /usr/include/SDL/SDL_timer.h /usr/include/SDL/SDL_audio.h
../engine/imnode.o: /usr/include/SDL/SDL_byteorder.h
../engine/imnode.o: /usr/include/SDL/SDL_cdrom.h
../engine/imnode.o: /usr/include/SDL/SDL_events.h
../engine/imnode.o: /usr/include/SDL/SDL_active.h
../engine/imnode.o: /usr/include/SDL/SDL_keyboard.h
../engine/imnode.o: /usr/include/SDL/SDL_keysym.h
../engine/imnode.o: /usr/include/SDL/SDL_mouse.h /usr/include/SDL/SDL_video.h
../engine/imnode.o: /usr/include/SDL/SDL_mutex.h /usr/include/SDL/SDL_quit.h
../engine/imnode.o: /usr/include/SDL/SDL_version.h ../util/glmath.h
../engine/imnode.o: ../util/kyrautil.h ../util/dynarray.h
../engine/imnode.o: /usr/include/limits.h /usr/include/bits/posix1_lim.h
../engine/imnode.o: /usr/include/bits/local_lim.h /usr/include/linux/limits.h
../engine/imnode.o: /usr/include/bits/posix2_lim.h ../engine/types.h
../engine/imnode.o: ../engine/engine.h ../engine/vault.h ../util/kyrautil.h
../engine/imnode.o: ../util/kyralist.h ../util/dynarray.h ../util/glstring.h
../engine/imnode.o: ../engine/rle.h ../engine/types.h ../engine/debug.h
../engine/imnode.o: ../engine/painter.h ../engine/color.h
../engine/imnode.o: ../engine/imagetree.h ../util/glintmap.h
../engine/imnode.o: ../util/glstringmap.h ../engine/imagecontainer.h
../engine/imnode.o: ../engine/image.h ../engine/imagenode.h
../engine/imnode.o: ../engine/sprite.h ../engine/spriteresource.h
../engine/imnode.o: ../engine/kyraresource.h ../engine/action.h
../engine/imnode.o: ../engine/dirtyrectangle.h
../engine/kyraresource.o: ../engine/kyraresource.h ../util/glstring.h
../engine/kyraresource.o: ../engine/types.h ../util/dynarray.h
../engine/kyraresource.o: /usr/include/limits.h /usr/include/features.h
../engine/kyraresource.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
../engine/kyraresource.o: /usr/include/bits/posix1_lim.h
../engine/kyraresource.o: /usr/include/bits/local_lim.h
../engine/kyraresource.o: /usr/include/linux/limits.h
../engine/kyraresource.o: /usr/include/bits/posix2_lim.h ../util/kyrautil.h
../engine/kyraresource.o: ../engine/debug.h ../engine/types.h
../engine/kyraresource.o: ../util/kyrautil.h /usr/include/SDL/SDL_endian.h
../engine/kyraresource.o: /usr/include/stdio.h
../engine/kyraresource.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stddef.h
../engine/kyraresource.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stdarg.h
../engine/kyraresource.o: /usr/include/bits/types.h /usr/include/libio.h
../engine/kyraresource.o: /usr/include/_G_config.h
../engine/kyraresource.o: /usr/include/bits/stdio_lim.h
../engine/kyraresource.o: /usr/include/SDL/SDL_types.h
../engine/kyraresource.o: /usr/include/SDL/SDL_rwops.h
../engine/kyraresource.o: /usr/include/SDL/begin_code.h
../engine/kyraresource.o: /usr/include/SDL/close_code.h
../engine/kyraresource.o: /usr/include/SDL/SDL_byteorder.h
../engine/kyraresource.o: ../engine/spriteresource.h ../util/glstringmap.h
../engine/kyraresource.o: ../util/glintmap.h ../util/glmath.h
../engine/kyraresource.o: ../engine/action.h /usr/include/SDL/SDL.h
../engine/kyraresource.o: /usr/include/SDL/SDL_main.h
../engine/kyraresource.o: /usr/include/SDL/SDL_error.h
../engine/kyraresource.o: /usr/include/SDL/SDL_timer.h
../engine/kyraresource.o: /usr/include/SDL/SDL_audio.h
../engine/kyraresource.o: /usr/include/SDL/SDL_cdrom.h
../engine/kyraresource.o: /usr/include/SDL/SDL_events.h
../engine/kyraresource.o: /usr/include/SDL/SDL_active.h
../engine/kyraresource.o: /usr/include/SDL/SDL_keyboard.h
../engine/kyraresource.o: /usr/include/SDL/SDL_keysym.h
../engine/kyraresource.o: /usr/include/SDL/SDL_mouse.h
../engine/kyraresource.o: /usr/include/SDL/SDL_video.h
../engine/kyraresource.o: /usr/include/SDL/SDL_mutex.h
../engine/kyraresource.o: /usr/include/SDL/SDL_quit.h
../engine/kyraresource.o: /usr/include/SDL/SDL_version.h ../engine/rle.h
../engine/kyraresource.o: ../engine/debug.h ../engine/painter.h
../engine/kyraresource.o: ../engine/color.h ../util/dynarray.h
../engine/kyraresource.o: ../engine/tileresource.h ../engine/pixelblock.h
../engine/painter.o: ../engine/painter.h /usr/include/SDL/SDL.h
../engine/painter.o: /usr/include/SDL/SDL_main.h /usr/include/SDL/SDL_types.h
../engine/painter.o: /usr/include/SDL/SDL_error.h
../engine/painter.o: /usr/include/SDL/begin_code.h
../engine/painter.o: /usr/include/SDL/close_code.h
../engine/painter.o: /usr/include/SDL/SDL_rwops.h /usr/include/stdio.h
../engine/painter.o: /usr/include/features.h /usr/include/sys/cdefs.h
../engine/painter.o: /usr/include/gnu/stubs.h
../engine/painter.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stddef.h
../engine/painter.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stdarg.h
../engine/painter.o: /usr/include/bits/types.h /usr/include/libio.h
../engine/painter.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
../engine/painter.o: /usr/include/SDL/SDL_timer.h
../engine/painter.o: /usr/include/SDL/SDL_audio.h
../engine/painter.o: /usr/include/SDL/SDL_byteorder.h
../engine/painter.o: /usr/include/SDL/SDL_cdrom.h
../engine/painter.o: /usr/include/SDL/SDL_events.h
../engine/painter.o: /usr/include/SDL/SDL_active.h
../engine/painter.o: /usr/include/SDL/SDL_keyboard.h
../engine/painter.o: /usr/include/SDL/SDL_keysym.h
../engine/painter.o: /usr/include/SDL/SDL_mouse.h
../engine/painter.o: /usr/include/SDL/SDL_video.h
../engine/painter.o: /usr/include/SDL/SDL_mutex.h /usr/include/SDL/SDL_quit.h
../engine/painter.o: /usr/include/SDL/SDL_version.h ../engine/types.h
../engine/painter.o: ../engine/color.h ../util/glmath.h ../util/kyrautil.h
../engine/painter.o: ../util/dynarray.h /usr/include/limits.h
../engine/painter.o: /usr/include/bits/posix1_lim.h
../engine/painter.o: /usr/include/bits/local_lim.h
../engine/painter.o: /usr/include/linux/limits.h
../engine/painter.o: /usr/include/bits/posix2_lim.h ../engine/debug.h
../engine/painter.o: ../engine/types.h ../engine/debug.h
../engine/parser.o: ../tinyxml/tinyxml.h /usr/include/stdio.h
../engine/parser.o: /usr/include/features.h /usr/include/sys/cdefs.h
../engine/parser.o: /usr/include/gnu/stubs.h
../engine/parser.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stddef.h
../engine/parser.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stdarg.h
../engine/parser.o: /usr/include/bits/types.h /usr/include/libio.h
../engine/parser.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
../engine/parser.o: /usr/include/assert.h ../engine/parser.h
../engine/parser.o: ../util/kyralist.h ../engine/debug.h ../util/dynarray.h
../engine/pixelblock.o: ../engine/pixelblock.h ../engine/types.h
../engine/pixelblock.o: ../engine/debug.h ../util/glmath.h ../util/kyrautil.h
../engine/pixelblock.o: ../util/dynarray.h /usr/include/limits.h
../engine/pixelblock.o: /usr/include/features.h /usr/include/sys/cdefs.h
../engine/pixelblock.o: /usr/include/gnu/stubs.h
../engine/pixelblock.o: /usr/include/bits/posix1_lim.h
../engine/pixelblock.o: /usr/include/bits/local_lim.h
../engine/pixelblock.o: /usr/include/linux/limits.h
../engine/pixelblock.o: /usr/include/bits/posix2_lim.h ../engine/debug.h
../engine/pixelblock.o: ../engine/types.h /usr/include/SDL/SDL.h
../engine/pixelblock.o: /usr/include/SDL/SDL_main.h
../engine/pixelblock.o: /usr/include/SDL/SDL_types.h
../engine/pixelblock.o: /usr/include/SDL/SDL_error.h
../engine/pixelblock.o: /usr/include/SDL/begin_code.h
../engine/pixelblock.o: /usr/include/SDL/close_code.h
../engine/pixelblock.o: /usr/include/SDL/SDL_rwops.h /usr/include/stdio.h
../engine/pixelblock.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stddef.h
../engine/pixelblock.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stdarg.h
../engine/pixelblock.o: /usr/include/bits/types.h /usr/include/libio.h
../engine/pixelblock.o: /usr/include/_G_config.h
../engine/pixelblock.o: /usr/include/bits/stdio_lim.h
../engine/pixelblock.o: /usr/include/SDL/SDL_timer.h
../engine/pixelblock.o: /usr/include/SDL/SDL_audio.h
../engine/pixelblock.o: /usr/include/SDL/SDL_byteorder.h
../engine/pixelblock.o: /usr/include/SDL/SDL_cdrom.h
../engine/pixelblock.o: /usr/include/SDL/SDL_events.h
../engine/pixelblock.o: /usr/include/SDL/SDL_active.h
../engine/pixelblock.o: /usr/include/SDL/SDL_keyboard.h
../engine/pixelblock.o: /usr/include/SDL/SDL_keysym.h
../engine/pixelblock.o: /usr/include/SDL/SDL_mouse.h
../engine/pixelblock.o: /usr/include/SDL/SDL_video.h
../engine/pixelblock.o: /usr/include/SDL/SDL_mutex.h
../engine/pixelblock.o: /usr/include/SDL/SDL_quit.h
../engine/pixelblock.o: /usr/include/SDL/SDL_version.h ../engine/painter.h
../engine/pixelblock.o: ../engine/color.h /usr/include/SDL/SDL_endian.h
../engine/rle.o: ../engine/rle.h ../engine/types.h ../engine/debug.h
../engine/rle.o: ../util/glmath.h ../util/kyrautil.h ../util/dynarray.h
../engine/rle.o: /usr/include/limits.h /usr/include/features.h
../engine/rle.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
../engine/rle.o: /usr/include/bits/posix1_lim.h /usr/include/bits/local_lim.h
../engine/rle.o: /usr/include/linux/limits.h /usr/include/bits/posix2_lim.h
../engine/rle.o: ../engine/debug.h ../engine/types.h /usr/include/SDL/SDL.h
../engine/rle.o: /usr/include/SDL/SDL_main.h /usr/include/SDL/SDL_types.h
../engine/rle.o: /usr/include/SDL/SDL_error.h /usr/include/SDL/begin_code.h
../engine/rle.o: /usr/include/SDL/close_code.h /usr/include/SDL/SDL_rwops.h
../engine/rle.o: /usr/include/stdio.h
../engine/rle.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stddef.h
../engine/rle.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stdarg.h
../engine/rle.o: /usr/include/bits/types.h /usr/include/libio.h
../engine/rle.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
../engine/rle.o: /usr/include/SDL/SDL_timer.h /usr/include/SDL/SDL_audio.h
../engine/rle.o: /usr/include/SDL/SDL_byteorder.h
../engine/rle.o: /usr/include/SDL/SDL_cdrom.h /usr/include/SDL/SDL_events.h
../engine/rle.o: /usr/include/SDL/SDL_active.h
../engine/rle.o: /usr/include/SDL/SDL_keyboard.h
../engine/rle.o: /usr/include/SDL/SDL_keysym.h /usr/include/SDL/SDL_mouse.h
../engine/rle.o: /usr/include/SDL/SDL_video.h /usr/include/SDL/SDL_mutex.h
../engine/rle.o: /usr/include/SDL/SDL_quit.h /usr/include/SDL/SDL_version.h
../engine/rle.o: ../engine/painter.h ../engine/color.h
../engine/rle.o: /usr/include/SDL/SDL_endian.h ../util/kyrautil.h
../engine/rle.o: /usr/include/string.h
../engine/sdlutil.o: ../engine/sdlutil.h /usr/include/SDL/SDL_rwops.h
../engine/sdlutil.o: /usr/include/stdio.h /usr/include/features.h
../engine/sdlutil.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
../engine/sdlutil.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stddef.h
../engine/sdlutil.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stdarg.h
../engine/sdlutil.o: /usr/include/bits/types.h /usr/include/libio.h
../engine/sdlutil.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
../engine/sdlutil.o: /usr/include/SDL/SDL_types.h
../engine/sdlutil.o: /usr/include/SDL/begin_code.h
../engine/sdlutil.o: /usr/include/SDL/close_code.h ../util/glstring.h
../engine/sdlutil.o: ../engine/types.h ../util/dynarray.h
../engine/sdlutil.o: /usr/include/limits.h /usr/include/bits/posix1_lim.h
../engine/sdlutil.o: /usr/include/bits/local_lim.h
../engine/sdlutil.o: /usr/include/linux/limits.h
../engine/sdlutil.o: /usr/include/bits/posix2_lim.h ../util/kyrautil.h
../engine/sdlutil.o: ../engine/debug.h
../engine/sprite.o: ../engine/sprite.h ../engine/image.h ../engine/imnode.h
../engine/sprite.o: ../util/insidelist.h ../engine/debug.h
../engine/sprite.o: /usr/include/SDL/SDL.h /usr/include/SDL/SDL_main.h
../engine/sprite.o: /usr/include/SDL/SDL_types.h /usr/include/SDL/SDL_error.h
../engine/sprite.o: /usr/include/SDL/begin_code.h
../engine/sprite.o: /usr/include/SDL/close_code.h
../engine/sprite.o: /usr/include/SDL/SDL_rwops.h /usr/include/stdio.h
../engine/sprite.o: /usr/include/features.h /usr/include/sys/cdefs.h
../engine/sprite.o: /usr/include/gnu/stubs.h
../engine/sprite.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stddef.h
../engine/sprite.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stdarg.h
../engine/sprite.o: /usr/include/bits/types.h /usr/include/libio.h
../engine/sprite.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
../engine/sprite.o: /usr/include/SDL/SDL_timer.h /usr/include/SDL/SDL_audio.h
../engine/sprite.o: /usr/include/SDL/SDL_byteorder.h
../engine/sprite.o: /usr/include/SDL/SDL_cdrom.h
../engine/sprite.o: /usr/include/SDL/SDL_events.h
../engine/sprite.o: /usr/include/SDL/SDL_active.h
../engine/sprite.o: /usr/include/SDL/SDL_keyboard.h
../engine/sprite.o: /usr/include/SDL/SDL_keysym.h
../engine/sprite.o: /usr/include/SDL/SDL_mouse.h /usr/include/SDL/SDL_video.h
../engine/sprite.o: /usr/include/SDL/SDL_mutex.h /usr/include/SDL/SDL_quit.h
../engine/sprite.o: /usr/include/SDL/SDL_version.h ../util/glmath.h
../engine/sprite.o: ../util/kyrautil.h ../util/dynarray.h
../engine/sprite.o: /usr/include/limits.h /usr/include/bits/posix1_lim.h
../engine/sprite.o: /usr/include/bits/local_lim.h /usr/include/linux/limits.h
../engine/sprite.o: /usr/include/bits/posix2_lim.h ../engine/types.h
../engine/sprite.o: ../engine/spriteresource.h ../util/glstring.h
../engine/sprite.o: ../util/glstringmap.h ../util/glintmap.h
../engine/sprite.o: ../engine/kyraresource.h ../engine/types.h
../engine/sprite.o: ../util/kyrautil.h ../engine/action.h ../engine/rle.h
../engine/sprite.o: ../engine/debug.h ../engine/painter.h ../engine/color.h
../engine/sprite.o: ../util/dynarray.h ../engine/engine.h ../engine/vault.h
../engine/sprite.o: ../util/kyralist.h ../engine/imagetree.h
../engine/sprite.o: ../engine/imagecontainer.h ../engine/imagenode.h
../engine/sprite.o: ../engine/dirtyrectangle.h
../engine/spriteresource.o: /usr/include/SDL/SDL_endian.h
../engine/spriteresource.o: /usr/include/stdio.h /usr/include/features.h
../engine/spriteresource.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
../engine/spriteresource.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stddef.h
../engine/spriteresource.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stdarg.h
../engine/spriteresource.o: /usr/include/bits/types.h /usr/include/libio.h
../engine/spriteresource.o: /usr/include/_G_config.h
../engine/spriteresource.o: /usr/include/bits/stdio_lim.h
../engine/spriteresource.o: /usr/include/SDL/SDL_types.h
../engine/spriteresource.o: /usr/include/SDL/SDL_rwops.h
../engine/spriteresource.o: /usr/include/SDL/begin_code.h
../engine/spriteresource.o: /usr/include/SDL/close_code.h
../engine/spriteresource.o: /usr/include/SDL/SDL_byteorder.h ../engine/rle.h
../engine/spriteresource.o: ../engine/types.h ../engine/debug.h
../engine/spriteresource.o: ../util/glmath.h ../util/kyrautil.h
../engine/spriteresource.o: ../util/dynarray.h /usr/include/limits.h
../engine/spriteresource.o: /usr/include/bits/posix1_lim.h
../engine/spriteresource.o: /usr/include/bits/local_lim.h
../engine/spriteresource.o: /usr/include/linux/limits.h
../engine/spriteresource.o: /usr/include/bits/posix2_lim.h ../engine/debug.h
../engine/spriteresource.o: ../engine/types.h /usr/include/SDL/SDL.h
../engine/spriteresource.o: /usr/include/SDL/SDL_main.h
../engine/spriteresource.o: /usr/include/SDL/SDL_error.h
../engine/spriteresource.o: /usr/include/SDL/SDL_timer.h
../engine/spriteresource.o: /usr/include/SDL/SDL_audio.h
../engine/spriteresource.o: /usr/include/SDL/SDL_cdrom.h
../engine/spriteresource.o: /usr/include/SDL/SDL_events.h
../engine/spriteresource.o: /usr/include/SDL/SDL_active.h
../engine/spriteresource.o: /usr/include/SDL/SDL_keyboard.h
../engine/spriteresource.o: /usr/include/SDL/SDL_keysym.h
../engine/spriteresource.o: /usr/include/SDL/SDL_mouse.h
../engine/spriteresource.o: /usr/include/SDL/SDL_video.h
../engine/spriteresource.o: /usr/include/SDL/SDL_mutex.h
../engine/spriteresource.o: /usr/include/SDL/SDL_quit.h
../engine/spriteresource.o: /usr/include/SDL/SDL_version.h
../engine/spriteresource.o: ../engine/painter.h ../engine/color.h
../engine/spriteresource.o: ../engine/spriteresource.h ../util/glstring.h
../engine/spriteresource.o: ../util/glstringmap.h ../util/glintmap.h
../engine/spriteresource.o: ../engine/kyraresource.h ../util/kyrautil.h
../engine/spriteresource.o: ../engine/action.h ../util/dynarray.h
../engine/spriteresource.o: ../engine/sdlutil.h
../engine/tile.o: ../engine/tile.h ../engine/image.h ../engine/imnode.h
../engine/tile.o: ../util/insidelist.h ../engine/debug.h
../engine/tile.o: /usr/include/SDL/SDL.h /usr/include/SDL/SDL_main.h
../engine/tile.o: /usr/include/SDL/SDL_types.h /usr/include/SDL/SDL_error.h
../engine/tile.o: /usr/include/SDL/begin_code.h /usr/include/SDL/close_code.h
../engine/tile.o: /usr/include/SDL/SDL_rwops.h /usr/include/stdio.h
../engine/tile.o: /usr/include/features.h /usr/include/sys/cdefs.h
../engine/tile.o: /usr/include/gnu/stubs.h
../engine/tile.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stddef.h
../engine/tile.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stdarg.h
../engine/tile.o: /usr/include/bits/types.h /usr/include/libio.h
../engine/tile.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
../engine/tile.o: /usr/include/SDL/SDL_timer.h /usr/include/SDL/SDL_audio.h
../engine/tile.o: /usr/include/SDL/SDL_byteorder.h
../engine/tile.o: /usr/include/SDL/SDL_cdrom.h /usr/include/SDL/SDL_events.h
../engine/tile.o: /usr/include/SDL/SDL_active.h
../engine/tile.o: /usr/include/SDL/SDL_keyboard.h
../engine/tile.o: /usr/include/SDL/SDL_keysym.h /usr/include/SDL/SDL_mouse.h
../engine/tile.o: /usr/include/SDL/SDL_video.h /usr/include/SDL/SDL_mutex.h
../engine/tile.o: /usr/include/SDL/SDL_quit.h /usr/include/SDL/SDL_version.h
../engine/tile.o: ../util/glmath.h ../util/kyrautil.h ../util/dynarray.h
../engine/tile.o: /usr/include/limits.h /usr/include/bits/posix1_lim.h
../engine/tile.o: /usr/include/bits/local_lim.h /usr/include/linux/limits.h
../engine/tile.o: /usr/include/bits/posix2_lim.h ../engine/types.h
../engine/tile.o: ../engine/tileresource.h ../engine/kyraresource.h
../engine/tile.o: ../util/glstring.h ../engine/types.h ../util/kyrautil.h
../engine/tile.o: ../engine/pixelblock.h ../engine/debug.h
../engine/tile.o: ../engine/painter.h ../engine/color.h ../engine/engine.h
../engine/tile.o: ../engine/vault.h ../util/kyralist.h ../util/dynarray.h
../engine/tile.o: ../engine/rle.h ../engine/imagetree.h ../util/glintmap.h
../engine/tile.o: ../util/glstringmap.h ../engine/imagecontainer.h
../engine/tile.o: ../engine/imagenode.h ../engine/sprite.h
../engine/tile.o: ../engine/spriteresource.h ../engine/action.h
../engine/tile.o: ../engine/dirtyrectangle.h
../engine/tileresource.o: /usr/include/SDL/SDL_endian.h /usr/include/stdio.h
../engine/tileresource.o: /usr/include/features.h /usr/include/sys/cdefs.h
../engine/tileresource.o: /usr/include/gnu/stubs.h
../engine/tileresource.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stddef.h
../engine/tileresource.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stdarg.h
../engine/tileresource.o: /usr/include/bits/types.h /usr/include/libio.h
../engine/tileresource.o: /usr/include/_G_config.h
../engine/tileresource.o: /usr/include/bits/stdio_lim.h
../engine/tileresource.o: /usr/include/SDL/SDL_types.h
../engine/tileresource.o: /usr/include/SDL/SDL_rwops.h
../engine/tileresource.o: /usr/include/SDL/begin_code.h
../engine/tileresource.o: /usr/include/SDL/close_code.h
../engine/tileresource.o: /usr/include/SDL/SDL_byteorder.h
../engine/tileresource.o: ../engine/tileresource.h ../engine/kyraresource.h
../engine/tileresource.o: ../util/glstring.h ../engine/types.h
../engine/tileresource.o: ../util/dynarray.h /usr/include/limits.h
../engine/tileresource.o: /usr/include/bits/posix1_lim.h
../engine/tileresource.o: /usr/include/bits/local_lim.h
../engine/tileresource.o: /usr/include/linux/limits.h
../engine/tileresource.o: /usr/include/bits/posix2_lim.h ../util/kyrautil.h
../engine/tileresource.o: ../engine/debug.h ../engine/types.h
../engine/tileresource.o: ../util/kyrautil.h ../util/glmath.h
../engine/tileresource.o: ../engine/pixelblock.h ../engine/debug.h
../engine/tileresource.o: /usr/include/SDL/SDL.h /usr/include/SDL/SDL_main.h
../engine/tileresource.o: /usr/include/SDL/SDL_error.h
../engine/tileresource.o: /usr/include/SDL/SDL_timer.h
../engine/tileresource.o: /usr/include/SDL/SDL_audio.h
../engine/tileresource.o: /usr/include/SDL/SDL_cdrom.h
../engine/tileresource.o: /usr/include/SDL/SDL_events.h
../engine/tileresource.o: /usr/include/SDL/SDL_active.h
../engine/tileresource.o: /usr/include/SDL/SDL_keyboard.h
../engine/tileresource.o: /usr/include/SDL/SDL_keysym.h
../engine/tileresource.o: /usr/include/SDL/SDL_mouse.h
../engine/tileresource.o: /usr/include/SDL/SDL_video.h
../engine/tileresource.o: /usr/include/SDL/SDL_mutex.h
../engine/tileresource.o: /usr/include/SDL/SDL_quit.h
../engine/tileresource.o: /usr/include/SDL/SDL_version.h ../engine/painter.h
../engine/tileresource.o: ../engine/color.h ../engine/sdlutil.h
../engine/userimage.o: ../engine/userimage.h ../engine/image.h
../engine/userimage.o: ../engine/imnode.h ../util/insidelist.h
../engine/userimage.o: ../engine/debug.h /usr/include/SDL/SDL.h
../engine/userimage.o: /usr/include/SDL/SDL_main.h
../engine/userimage.o: /usr/include/SDL/SDL_types.h
../engine/userimage.o: /usr/include/SDL/SDL_error.h
../engine/userimage.o: /usr/include/SDL/begin_code.h
../engine/userimage.o: /usr/include/SDL/close_code.h
../engine/userimage.o: /usr/include/SDL/SDL_rwops.h /usr/include/stdio.h
../engine/userimage.o: /usr/include/features.h /usr/include/sys/cdefs.h
../engine/userimage.o: /usr/include/gnu/stubs.h
../engine/userimage.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stddef.h
../engine/userimage.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stdarg.h
../engine/userimage.o: /usr/include/bits/types.h /usr/include/libio.h
../engine/userimage.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
../engine/userimage.o: /usr/include/SDL/SDL_timer.h
../engine/userimage.o: /usr/include/SDL/SDL_audio.h
../engine/userimage.o: /usr/include/SDL/SDL_byteorder.h
../engine/userimage.o: /usr/include/SDL/SDL_cdrom.h
../engine/userimage.o: /usr/include/SDL/SDL_events.h
../engine/userimage.o: /usr/include/SDL/SDL_active.h
../engine/userimage.o: /usr/include/SDL/SDL_keyboard.h
../engine/userimage.o: /usr/include/SDL/SDL_keysym.h
../engine/userimage.o: /usr/include/SDL/SDL_mouse.h
../engine/userimage.o: /usr/include/SDL/SDL_video.h
../engine/userimage.o: /usr/include/SDL/SDL_mutex.h
../engine/userimage.o: /usr/include/SDL/SDL_quit.h
../engine/userimage.o: /usr/include/SDL/SDL_version.h ../util/glmath.h
../engine/userimage.o: ../util/kyrautil.h ../util/dynarray.h
../engine/userimage.o: /usr/include/limits.h /usr/include/bits/posix1_lim.h
../engine/userimage.o: /usr/include/bits/local_lim.h
../engine/userimage.o: /usr/include/linux/limits.h
../engine/userimage.o: /usr/include/bits/posix2_lim.h ../engine/types.h
../engine/userimage.o: ../engine/engine.h ../engine/vault.h
../engine/userimage.o: ../util/kyrautil.h ../util/kyralist.h
../engine/userimage.o: ../util/dynarray.h ../util/glstring.h ../engine/rle.h
../engine/userimage.o: ../engine/types.h ../engine/debug.h
../engine/userimage.o: ../engine/painter.h ../engine/color.h
../engine/userimage.o: ../engine/imagetree.h ../util/glintmap.h
../engine/userimage.o: ../util/glstringmap.h ../engine/imagecontainer.h
../engine/userimage.o: ../engine/imagenode.h ../engine/sprite.h
../engine/userimage.o: ../engine/spriteresource.h ../engine/kyraresource.h
../engine/userimage.o: ../engine/action.h ../engine/dirtyrectangle.h
../engine/vault.o: /usr/include/time.h /usr/include/bits/types.h
../engine/vault.o: /usr/include/features.h /usr/include/sys/cdefs.h
../engine/vault.o: /usr/include/gnu/stubs.h
../engine/vault.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stddef.h
../engine/vault.o: ../engine/vault.h /usr/include/SDL/SDL.h
../engine/vault.o: /usr/include/SDL/SDL_main.h /usr/include/SDL/SDL_types.h
../engine/vault.o: /usr/include/SDL/SDL_error.h /usr/include/SDL/begin_code.h
../engine/vault.o: /usr/include/SDL/close_code.h /usr/include/SDL/SDL_rwops.h
../engine/vault.o: /usr/include/stdio.h
../engine/vault.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stdarg.h
../engine/vault.o: /usr/include/libio.h /usr/include/_G_config.h
../engine/vault.o: /usr/include/bits/stdio_lim.h /usr/include/SDL/SDL_timer.h
../engine/vault.o: /usr/include/SDL/SDL_audio.h
../engine/vault.o: /usr/include/SDL/SDL_byteorder.h
../engine/vault.o: /usr/include/SDL/SDL_cdrom.h /usr/include/SDL/SDL_events.h
../engine/vault.o: /usr/include/SDL/SDL_active.h
../engine/vault.o: /usr/include/SDL/SDL_keyboard.h
../engine/vault.o: /usr/include/SDL/SDL_keysym.h /usr/include/SDL/SDL_mouse.h
../engine/vault.o: /usr/include/SDL/SDL_video.h /usr/include/SDL/SDL_mutex.h
../engine/vault.o: /usr/include/SDL/SDL_quit.h /usr/include/SDL/SDL_version.h
../engine/vault.o: ../util/kyrautil.h ../util/kyralist.h ../engine/debug.h
../engine/vault.o: ../util/dynarray.h ../util/glstring.h ../engine/types.h
../engine/vault.o: ../util/dynarray.h /usr/include/limits.h
../engine/vault.o: /usr/include/bits/posix1_lim.h
../engine/vault.o: /usr/include/bits/local_lim.h /usr/include/linux/limits.h
../engine/vault.o: /usr/include/bits/posix2_lim.h ../util/kyrautil.h
../engine/vault.o: ../engine/rle.h ../engine/types.h ../engine/debug.h
../engine/vault.o: ../util/glmath.h ../engine/painter.h ../engine/color.h
../engine/vault.o: /usr/include/SDL/SDL_endian.h ../engine/kyraresource.h
../engine/vault.o: ../engine/sdlutil.h
../util/glintmap.o: ../util/glintmap.h ../util/glstringmap.h
../util/glintmap.o: ../engine/debug.h
../util/glmath.o: ../util/glmath.h ../util/kyrautil.h ../util/dynarray.h
../util/glmath.o: /usr/include/limits.h /usr/include/features.h
../util/glmath.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
../util/glmath.o: /usr/include/bits/posix1_lim.h
../util/glmath.o: /usr/include/bits/local_lim.h /usr/include/linux/limits.h
../util/glmath.o: /usr/include/bits/posix2_lim.h ../engine/debug.h
../util/glmath.o: ../engine/types.h /usr/include/math.h
../util/glmath.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
../util/glmath.o: /usr/include/bits/mathcalls.h
../util/glmath.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/float.h
../util/glrandom.o: ../util/glrandom.h ../engine/types.h
../util/glrandom.o: /usr/include/stdlib.h /usr/include/features.h
../util/glrandom.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
../util/glrandom.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stddef.h
../util/glrandom.o: /usr/include/sys/types.h /usr/include/bits/types.h
../util/glrandom.o: /usr/include/time.h /usr/include/endian.h
../util/glrandom.o: /usr/include/bits/endian.h /usr/include/sys/select.h
../util/glrandom.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
../util/glrandom.o: /usr/include/sys/sysmacros.h /usr/include/alloca.h
../util/glrandom.o: ../util/kyrautil.h ../util/dynarray.h
../util/glrandom.o: /usr/include/limits.h /usr/include/bits/posix1_lim.h
../util/glrandom.o: /usr/include/bits/local_lim.h /usr/include/linux/limits.h
../util/glrandom.o: /usr/include/bits/posix2_lim.h ../engine/debug.h
../util/glstring.o: ../util/glstring.h ../engine/types.h ../util/dynarray.h
../util/glstring.o: /usr/include/limits.h /usr/include/features.h
../util/glstring.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
../util/glstring.o: /usr/include/bits/posix1_lim.h
../util/glstring.o: /usr/include/bits/local_lim.h /usr/include/linux/limits.h
../util/glstring.o: /usr/include/bits/posix2_lim.h ../util/kyrautil.h
../util/glstring.o: ../engine/debug.h /usr/include/stdlib.h
../util/glstring.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stddef.h
../util/glstring.o: /usr/include/sys/types.h /usr/include/bits/types.h
../util/glstring.o: /usr/include/time.h /usr/include/endian.h
../util/glstring.o: /usr/include/bits/endian.h /usr/include/sys/select.h
../util/glstring.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
../util/glstring.o: /usr/include/sys/sysmacros.h /usr/include/alloca.h
../util/glstring.o: /usr/include/ctype.h /usr/include/string.h
../util/glstring.o: /usr/include/stdio.h
../util/glstring.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stdarg.h
../util/glstring.o: /usr/include/libio.h /usr/include/_G_config.h
../util/glstring.o: /usr/include/bits/stdio_lim.h
../util/glstringmap.o: ../util/glstringmap.h ../engine/debug.h
../tinyxml/tinyxml.o: ../tinyxml/tinyxml.h /usr/include/stdio.h
../tinyxml/tinyxml.o: /usr/include/features.h /usr/include/sys/cdefs.h
../tinyxml/tinyxml.o: /usr/include/gnu/stubs.h
../tinyxml/tinyxml.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stddef.h
../tinyxml/tinyxml.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stdarg.h
../tinyxml/tinyxml.o: /usr/include/bits/types.h /usr/include/libio.h
../tinyxml/tinyxml.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
../tinyxml/tinyxml.o: /usr/include/assert.h
../tinyxml/tinyxmlparser.o: ../tinyxml/tinyxml.h /usr/include/stdio.h
../tinyxml/tinyxmlparser.o: /usr/include/features.h /usr/include/sys/cdefs.h
../tinyxml/tinyxmlparser.o: /usr/include/gnu/stubs.h
../tinyxml/tinyxmlparser.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stddef.h
../tinyxml/tinyxmlparser.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stdarg.h
../tinyxml/tinyxmlparser.o: /usr/include/bits/types.h /usr/include/libio.h
../tinyxml/tinyxmlparser.o: /usr/include/_G_config.h
../tinyxml/tinyxmlparser.o: /usr/include/bits/stdio_lim.h
../tinyxml/tinyxmlparser.o: /usr/include/assert.h
