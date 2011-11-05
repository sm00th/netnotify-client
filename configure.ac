# Initialize autoconf

AC_PREREQ([2.68])
AC_INIT([netnotify-client], [0.0.4], [BUG-REPORT-ADDRESS])
AM_INIT_AUTOMAKE([-Wall -Werror])
AC_CONFIG_SRCDIR([src/netnotify-client.c])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_MAKE_SET
AC_PROG_LN_S
AC_PROG_INSTALL

# Checks for libraries.
REQ_GLIB_VERSION=2.16
REQ_LIBNOTIFY_VERSION=0.5

pkg_modules="glib-2.0 >= $REQ_GLIB_VERSION, gio-2.0 >= $REQ_GLIB_VERSION, libnotify >= $REQ_LIBNOTIFY_VERSION"
PKG_CHECK_MODULES(NETNOTIFY_CLIENT, $pkg_modules)
AC_SUBST(NETNOTIFY_CLIENT_CFLAGS)
AC_SUBST(NETNOTIFY_CLIENT_LIBS)

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_OUTPUT
