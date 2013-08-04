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
	$(top_builddir)/libuhttpmock/libuhttpmock.la \
	$(UHM_LIBS) \
	$(NULL)

noinst_PROGRAMS = $(TEST_PROGS)

TEST_SRCS = \
	$(NULL)

TEST_PROGS += server
server_SOURCES = server.c $(TEST_SRCS)

-include $(top_srcdir)/git.mk
