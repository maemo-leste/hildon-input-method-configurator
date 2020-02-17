AC_INIT([Hildon Input method configurator], patsubst(esyscmd([dpkg-parsechangelog | sed -n '/^Version: \(.*\)$/ {s//\1/;p}']), [
]), [], [hildon-input-method-configurator])
AM_CONFIG_HEADER(config.h)

AC_CONFIG_MACRO_DIR([m4])
AM_MAINTAINER_MODE
AM_INIT_AUTOMAKE([foreign])

AC_PROG_CC
AC_PROG_INSTALL
AM_PROG_LIBTOOL

AC_HEADER_STDC

AC_PATH_X
AC_PATH_XTRA
AC_SUBST(X_CFLAGS)

PKG_PROG_PKG_CONFIG

#+++++++++++++++
# Misc programs
#+++++++++++++++


PKG_CHECK_MODULES(GLIB, [gtk+-2.0 gconf-2.0 hildon-input-method-ui-3.0])

PKG_CHECK_MODULES(EXPAT, [expat])

#+++++++++++++++++++
# Directories setup
#+++++++++++++++++++

AC_OUTPUT([
Makefile
src/Makefile
])
