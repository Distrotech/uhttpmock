include $(top_srcdir)/gtester.mk

AM_CPPFLAGS = \
	-I$(top_srcdir)/ \
	-I$(top_srcdir)/libuhttpmock \
	-DTEST_FILE_DIR="\"$(top_srcdir)/libuhttpmock/tests/\"" \
	-DG_LOG_DOMAIN=\"libuhttpmock-tests\" \
	-DLIBUHTTPMOCK_DISABLE_DEPRECATED \
	$(DISABLE_DEPRECATED) \
	$(NULL)
AM_CFLAGS = \
	$(WARN_CFLAGS) \
	$(UHM_CFLAGS) \
	$(NULL)

LIBS = \
	$(top_builddir)/libuhttpmock/libuhttpmock-@UHM_API_VERSION@.la \
	$(UHM_LIBS) \
	$(NULL)

noinst_PROGRAMS = $(TEST_PROGS)

TEST_SRCS = \
	$(NULL)

TEST_PROGS += server
server_SOURCES = server.c $(TEST_SRCS)

TEST_PROGS += resolver
resolver_SOURCES = resolver.c $(TEST_SRCS)

EXTRA_DIST += \
	server_logging_trace_failure_method \
	server_logging_trace_failure_unexpected-request \
	server_logging_trace_failure_uri \
	server_logging_trace_success_multiple-messages \
	server_logging_trace_success_normal \
	$(NULL)

-include $(top_srcdir)/git.mk
