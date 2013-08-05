AC_PREREQ(2.65)

# Release version
m4_define(uhm_version_major, 0)
m4_define(uhm_version_minor, 1)
m4_define(uhm_version_micro, 0)

# API version
# TODO: This currently isn't used in the library name; once we hit uhm_version_major=1, that should be changed
m4_define(uhm_api_version_major, 0)
m4_define(uhm_api_version_minor, 0)

AC_INIT([uhttpmock],[uhm_version_major.uhm_version_minor.uhm_version_micro],[https://bugzilla.gnome.org/enter_bug.cgi?product=uhttpmock],[uhttpmock],[http://live.gnome.org/uhttpmock])

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_SRCDIR([libuhttpmock/uhm.h])
AC_CONFIG_HEADERS([config.h])
AC_USE_SYSTEM_EXTENSIONS

AM_INIT_AUTOMAKE([1.9 dist-bzip2 no-dist-gzip check-news subdir-objects])

# Enable silent build when available (Automake 1.11)
m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])

AC_PROG_CXX
AM_PROG_CC_C_O
LT_INIT([])
PKG_PROG_PKG_CONFIG

# Requirements
GLIB_REQS=2.31.0
GIO_REQS=2.17.3
SOUP_REQS=2.37.91

# Before making a release, the UHM_LT_VERSION string should be modified. The string is of the form c:r:a. Follow these instructions sequentially:
#
#  1. If the library source code has changed at all since the last update, then increment revision (‘c:r:a’ becomes ‘c:r+1:a’).
#  2. If any interfaces have been added, removed, or changed since the last update, increment current, and set revision to 0.
#  3. If any interfaces have been added since the last public release, then increment age.
#  4. If any interfaces have been removed or changed since the last public release, then set age to 0.
UHM_LT_VERSION=0:0:0
AC_SUBST([UHM_LT_VERSION])

UHM_VERSION_MAJOR=uhm_version_major
UHM_VERSION_MINOR=uhm_version_minor
UHM_VERSION_MICRO=uhm_version_micro
UHM_API_VERSION=uhm_api_version_major.uhm_api_version_minor
UHM_API_VERSION_MAJOR=uhm_api_version_major
UHM_API_VERSION_MINOR=uhm_api_version_minor
AC_SUBST([UHM_VERSION_MAJOR])
AC_SUBST([UHM_VERSION_MINOR])
AC_SUBST([UHM_VERSION_MICRO])
AC_SUBST([UHM_API_VERSION])
AC_SUBST([UHM_API_VERSION_MAJOR])
AC_SUBST([UHM_API_VERSION_MINOR])

UHM_PACKAGES_PUBLIC="gobject-2.0 glib-2.0 >= $GLIB_REQS gio-2.0 >= $GIO_REQS libsoup-2.4 >= $SOUP_REQS"
UHM_PACKAGES_PRIVATE="gthread-2.0"
UHM_PACKAGES="$UHM_PACKAGES_PUBLIC $UHM_PACKAGES_PRIVATE"
AC_SUBST([UHM_PACKAGES_PUBLIC])
AC_SUBST([UHM_PACKAGES_PRIVATE])
AC_SUBST([UHM_PACKAGES])

PKG_CHECK_MODULES(UHM, [$UHM_PACKAGES])

# Code coverage
GNOME_CODE_COVERAGE

# General GNOME macros
GNOME_DEBUG_CHECK
GNOME_COMPILE_WARNINGS([maximum])
GNOME_MAINTAINER_MODE_DEFINES

GOBJECT_INTROSPECTION_CHECK([0.9.7])

VAPIGEN_CHECK()

AC_SUBST([AM_CPPFLAGS])
AC_SUBST([AM_CFLAGS])
AC_SUBST([AM_CXXFLAGS])
AC_SUBST([AM_LDFLAGS])

GTK_DOC_CHECK([1.14],[--flavour no-tmpl])

AC_CONFIG_FILES([
Makefile
libuhttpmock/libuhttpmock.pc
libuhttpmock/uhm-version.h
libuhttpmock/tests/Makefile
libuhttpmock/docs/Makefile
libuhttpmock/docs/version.xml
])
AC_OUTPUT
