#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([ManifestDBView], [1.1.4], [erich.kuester@arcor.de])
AM_INIT_AUTOMAKE([foreign])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

IT_PROG_INTLTOOL([0.35.0])

GETTEXT_PACKAGE=$PACKAGE
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE], ["$GETTEXT_PACKAGE"],
                   [The domain to use with gettext])

AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION([0.19.7])

PROGRAMNAME_LOCALEDIR=[${datadir}/locale]
AC_SUBST(PROGRAMNAME_LOCALEDIR)

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([gettext.h])
PKG_CHECK_MODULES([GTKMM], [gtkmm-3.0 >= 3.8.0])

GTK_DOC_CHECK([1.29])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile src/Makefile]
                 po/Makefile.in)
#                 16x16/Makefile)
AC_OUTPUT

