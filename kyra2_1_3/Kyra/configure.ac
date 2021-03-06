dnl Process this file with autoconf to produce a configure script.
AC_INIT(Kyra, 2.1.3, kyra-general@lists.sourceforge.net)
AC_PREREQ(2.52)
AC_CANONICAL_HOST
AC_CANONICAL_TARGET

dnl Check for automake
AM_INIT_AUTOMAKE

dnl Set the verion of Kyra here too
KYRA_MAJOR_VERSION=2
KYRA_MINOR_VERSION=1
KYRA_BUILD_VERSION=3
KYRA_VERSION=$KYRA_MAJOR_VERSION.$KYRA_MINOR_VERSION.$KYRA_BUILD_VERSION
AC_SUBST(KYRA_MAJOR_VERSION)
AC_SUBST(KYRA_MINOR_VERSION)
AC_SUBST(KYRA_BUILD_VERSION)
AC_SUBST(KYRA_VERSION)

dnl Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AM_PROG_LIBTOOL
AC_PROG_LIBTOOL

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([limits.h memory.h stdlib.h string.h])

dnl Checks for typedefs, structures, and compiler characteristics.
dnl Kyra doesn't use C constant: AC_C_CONST
dnl Or C inline: AC_C_INLINE

dnl Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([atexit memset sqrt strchr strrchr strstr strtoul])

dnl Checks for libraries.
dnl Check SDL version
SDL_VERSION=1.2.0
AM_PATH_SDL($SDL_VERSION,
            :,
            AC_MSG_ERROR([*** SDL version $SDL_VERSION not found!])
)

dnl Add the SDL preprocessor flags and libraries to the build process

LDFLAGS="-L/usr/lib -L/usr/local/lib -L/usr/X11R6/lib"
CFLAGS="$CFLAGS $SDL_CFLAGS -I/usr/local/include -I/usr/X11R6/include"
CXXFLAGS="$CXXFLAGS $SDL_CFLAGS -I/usr/local/include -I/usr/X11R6/include"
LIBS="$LDFLAGS $LIBS $SDL_LIBS"

dnl "borrowed" and modified from Bill Kendrick's Circus Linux
AC_CHECK_LIB(SDL_image, main,
             LIBS="$LIBS -lSDL_image",
             AC_MSG_ERROR([*** SDL_image library not found!])
            )

dnl Check if user wants OpenGL support... if not don't check
dnl set nogl to 0 first
nogl=0
AC_ARG_WITH(opengl, [supply this if you want OpenGL support],
AC_CHECK_LIB(GL, main,
             [LIBS="$LIBS -lGL"; CXXFLAGS="$CXXFLAGS -DKYRA_SUPPORT_OPENGL"],
             [echo "OpenGL not found.. disabling"]
	    ), [nogl=1])

AM_CONDITIONAL(NOGL, [test $nogl = 1])

dnl Check for DEBUG flag 
dnl set debug to 0 first
nodebug=0
AC_ARG_WITH(debug, [supply this to enable debugging support],
[CFLAGS="$CFLAGS -DDEBUG"; CXXFLAGS="$CXXFLAGS -DDEBUG"],
[nodebug=1])

AC_SUBST(SDL_CFLAGS)

dnl For kyra-config
KYRA_LIBS="-lkyra $LIBS"
AC_SUBST(KYRA_LIBS)

AC_OUTPUT(	Makefile
          	tinyxml/Makefile
			util/Makefile
	  		gui/Makefile
			guiExtended/Makefile
	  		engine/Makefile
	  		encoder/Makefile
	  		spriteed/Makefile
			kyra-config
	      )

