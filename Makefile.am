SUBDIRS = . libuhttpmock/tests libuhttpmock/docs

ACLOCAL_AMFLAGS = -I m4

DISTCHECK_CONFIGURE_FLAGS =
CLEANFILES =
MAINTAINERCLEANFILES =
EXTRA_DIST =

# libuhttpmock library
lib_LTLIBRARIES = libuhttpmock/libuhttpmock-@UHM_API_VERSION@.la

uhmincludedir = $(pkgincludedir)/uhttpmock
uhm_headers = \
	libuhttpmock/uhm.h \
	libuhttpmock/uhm-resolver.h \
	libuhttpmock/uhm-server.h \
	$(NULL)

# The following headers are private, and shouldn't be installed:
private_headers = \
	libuhttpmock/uhm-default-tls-certificate.h \
	$(NULL)
uhminclude_HEADERS = \
	$(uhm_headers) \
	libuhttpmock/uhm-version.h
	$(NULL)

uhm_sources = \
	libuhttpmock/uhm-resolver.c \
	libuhttpmock/uhm-server.c \
	$(NULL)

main_header = libuhttpmock/uhm.h
public_headers = $(uhminclude_HEADERS)

libuhttpmock_libuhttpmock_@UHM_API_VERSION@_la_SOURCES = \
	$(private_headers) \
	$(uhm_sources) \
	$(NULL)

libuhttpmock_libuhttpmock_@UHM_API_VERSION@_la_CPPFLAGS = \
	-I$(top_srcdir) \
	-I$(top_srcdir)/libuhttpmock \
	-I$(top_builddir)/libuhttpmock \
	-DG_LOG_DOMAIN=\"libuhttpmock\" \
	$(DISABLE_DEPRECATED) \
	$(AM_CPPFLAGS) \
	$(NULL)

libuhttpmock_libuhttpmock_@UHM_API_VERSION@_la_CFLAGS = \
	$(UHM_CFLAGS) \
	$(CODE_COVERAGE_CFLAGS) \
	$(WARN_CFLAGS) \
	$(AM_CFLAGS) \
	$(NULL)

libuhttpmock_libuhttpmock_@UHM_API_VERSION@_la_LIBADD = \
	$(UHM_LIBS) \
	$(CODE_COVERAGE_LIBS) \
	$(AM_LIBADD) \
	$(NULL)

libuhttpmock_libuhttpmock_@UHM_API_VERSION@_la_LDFLAGS = \
	-version-info $(UHM_LT_VERSION) \
	-export-symbols $(srcdir)/libuhttpmock/libuhttpmock.symbols \
	-no-undefined \
	$(AM_LDFLAGS) \
	$(NULL)

EXTRA_DIST += libuhttpmock/libuhttpmock.symbols

# Check if uhm.h includes all the public headers
check-local: check-headers
check-headers:
	@any_missing=0; for header_file in $(public_headers); do \
		if test "x$$header_file" != "x$(main_header)"; then \
			if ! grep "#include <$$header_file>" $(top_srcdir)/$(main_header) >/dev/null; then \
				echo "$(main_header) doesn't appear to include \"$$header_file\""; \
				any_missing=1; \
			fi; \
		fi; \
	done; exit "$$any_missing"

# Introspection
-include $(INTROSPECTION_MAKEFILE)
INTROSPECTION_GIRS =
INTROSPECTION_SCANNER_ARGS = --warn-all
INTROSPECTION_COMPILER_ARGS =
DISTCHECK_CONFIGURE_FLAGS += --enable-introspection
EXTRA_DIST += m4/introspection.m4

if HAVE_INTROSPECTION
libuhttpmock/Uhm-@UHM_API_VERSION@.gir: libuhttpmock/libuhttpmock-@UHM_API_VERSION@.la
libuhttpmock_Uhm_@UHM_API_VERSION_U@_gir_INCLUDES = GObject-2.0 Soup-2.4
libuhttpmock_Uhm_@UHM_API_VERSION_U@_gir_CFLAGS = \
	$(UHM_CFLAGS) \
	$(libuhttpmock_libuhttpmock_@UHM_API_VERSION@_la_CPPFLAGS) \
	$(NULL)
libuhttpmock_Uhm_@UHM_API_VERSION_U@_gir_LIBS = libuhttpmock/libuhttpmock-@UHM_API_VERSION@.la
libuhttpmock_Uhm_@UHM_API_VERSION_U@_gir_FILES = $(uhm_sources) $(public_headers)
libuhttpmock_Uhm_@UHM_API_VERSION_U@_gir_NAMESPACE = Uhm
libuhttpmock_Uhm_@UHM_API_VERSION_U@_gir_EXPORT_PACKAGES = libuhttpmock
libuhttpmock_Uhm_@UHM_API_VERSION_U@_gir_SCANNERFLAGS = \
	--nsversion=@UHM_API_VERSION@ \
	--symbol-prefix=uhm \
	--c-include="libuhttpmock/uhm.h"

INTROSPECTION_GIRS += libuhttpmock/Uhm-@UHM_API_VERSION@.gir

girdir = $(datadir)/gir-1.0
gir_DATA = $(INTROSPECTION_GIRS)

typelibdir = $(libdir)/girepository-1.0
typelib_DATA = $(INTROSPECTION_GIRS:.gir=.typelib)

CLEANFILES += $(gir_DATA) $(typelib_DATA)

if ENABLE_VAPIGEN
-include $(VAPIGEN_MAKEFILE)

# Note: The VAPI file name has to match the pkg-config file name.
# If we break API and add a version number to the pkg-config file name, the VAPI can be changed to match.
# See: https://bugzilla.gnome.org/show_bug.cgi?id=693865
libuhttpmock/libuhttpmock.vapi: libuhttpmock/Uhm-@UHM_API_VERSION@.gir

VAPIGEN_VAPIS = libuhttpmock/libuhttpmock.vapi

libuhttpmock_libuhttpmock_vapi_DEPS = libxml-2.0 libsoup-2.4
libuhttpmock_libuhttpmock_vapi_METADATADIRS = $(srcdir)/libuhttpmock
libuhttpmock_libuhttpmock_vapi_FILES = libuhttpmock/Uhm-@UHM_API_VERSION@.gir

libuhttpmock/libuhttpmock.deps:
	$(AM_V_GEN) for pkg in $(libuhttpmock_libuhttpmock_vapi_DEPS); do \
		echo $$pkg >> $@; \
	done

vapidir = $(datadir)/vala/vapi
vapi_DATA = $(VAPIGEN_VAPIS) $(VAPIGEN_VAPIS:.vapi=.deps)

CLEANFILES += $(vapi_DATA)
endif
endif

# Code coverage
@GNOME_CODE_COVERAGE_RULES@
CODE_COVERAGE_DIRECTORY = $(top_builddir)/libuhttpmock
CODE_COVERAGE_LCOV_OPTIONS = --base-directory $(abs_top_srcdir)

# Cleaning
EXTRA_DIST += \
	autogen.sh \
	HACKING \
	gtk-doc.make \
	uhttpmock.doap \
	$(NULL)

CLEANFILES += \
	`find "$(srcdir)" -type f -name .dirstamp -print` \
	$(NULL)

DISTCLEANFILES = \
	gtk-doc.make \
	$(NULL)

MAINTAINERCLEANFILES += \
	$(GITIGNORE_MAINTAINERCLEANFILES_TOPLEVEL) \
	$(GITIGNORE_MAINTAINERCLEANFILES_MAKEFILE_IN) \
	$(GITIGNORE_MAINTAINERCLEANFILES_M4_LIBTOOL) \
	build-aux/ \
	INSTALL \
	omf.make \
	gtk-doc.make \
	config.rpath \
	m4/gtk-doc.m4 \
	$(NULL)

DISTCHECK_CONFIGURE_FLAGS += --enable-gtk-doc

# pkgconfig data
pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libuhttpmock/libuhttpmock.pc

CLEANFILES += $(pkgconfig_DATA)
EXTRA_DIST += \
	libuhttpmock/libuhttpmock.pc.in \
	libuhttpmock/uhm-version.h.in \
	$(NULL)

# ChangeLog
ChangeLog: $(srcdir)/ChangeLog
$(srcdir)/ChangeLog:
	@echo Creating $@
	@if test -d "$(srcdir)/.git"; then \
	  (GIT_DIR=$(top_srcdir)/.git ./missing --run \
	   git log --stat) | fmt --split-only > $@.tmp \
	  && mv -f $@.tmp $@ \
	  || ($(RM) $@.tmp; \
	      echo Failed to generate ChangeLog, your ChangeLog may be outdated >&2; \
	      (test -f $@ || echo git-log is required to generate this file >> $@)); \
	else \
	  test -f $@ || \
	  (echo A git checkout and git-log is required to generate ChangeLog >&2 && \
	  echo A git checkout and git-log is required to generate this file >> $@); \
	fi
.PHONY: $(srcdir)/ChangeLog

MAINTAINERCLEANFILES += ChangeLog

-include $(top_srcdir)/git.mk
