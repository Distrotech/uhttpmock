/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * uhttpmock
 * Copyright (C) Philip Withnall 2013 <philip@tecnocode.co.uk>
 *
 * uhttpmock is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * uhttpmock is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with uhttpmock.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <glib.h>
#include <locale.h>
#include <string.h>
#include <libsoup/soup.h>

#include "uhm.h"

/* Construct a new UhmServer and see if anything explodes. */
static void
test_server_construction (void)
{
	UhmServer *server;

	server = uhm_server_new ();
	g_assert (UHM_IS_SERVER (server));
	g_object_unref (server);
}

static void
notify_emitted_cb (GObject *obj, GParamSpec *pspec, guint *counter)
{
	*counter = *counter + 1;
}

/* Test getting and setting the UhmServer:trace-directory property. */
static void
test_server_properties_trace_directory (void)
{
	UhmServer *server;
	GFile *trace_directory, *new_trace_directory;
	gchar *uri1, *uri2;
	guint counter;

	server = uhm_server_new ();

	counter = 0;
	g_signal_connect (G_OBJECT (server), "notify::trace-directory", (GCallback) notify_emitted_cb, &counter);

	/* Check the default value. */
	g_assert (uhm_server_get_trace_directory (server) == NULL);
	g_object_get (G_OBJECT (server), "trace-directory", &trace_directory, NULL);
	g_assert (trace_directory == NULL);

	/* Set the trace directory to an arbitrary, existent, directory. */
	new_trace_directory = g_file_new_for_path ("/"); /* arbitrary directory */
	uhm_server_set_trace_directory (server, new_trace_directory);
	g_assert_cmpuint (counter, ==, 1);
	g_object_unref (new_trace_directory);

	/* Check the new directory can be retrieved via the getter. */
	trace_directory = uhm_server_get_trace_directory (server);
	g_assert (G_IS_FILE (trace_directory));

	uri1 = g_file_get_uri (trace_directory);
	uri2 = g_file_get_uri (new_trace_directory);
	g_assert_cmpstr (uri1, ==, uri2);
	g_free (uri2);
	g_free (uri1);

	/* Check the new directory can be retrieved as a property. */
	g_object_get (G_OBJECT (server), "trace-directory", &trace_directory, NULL);
	g_assert (G_IS_FILE (trace_directory));

	uri1 = g_file_get_uri (trace_directory);
	uri2 = g_file_get_uri (new_trace_directory);
	g_assert_cmpstr (uri1, ==, uri2);
	g_free (uri2);
	g_free (uri1);

	g_object_unref (trace_directory);

	/* Set the directory to NULL again, this time using the GObject setter. */
	g_object_set (G_OBJECT (server), "trace-directory", NULL, NULL);
	g_assert_cmpuint (counter, ==, 2);
	g_assert (uhm_server_get_trace_directory (server) == NULL);

	g_object_unref (server);
}

/* Test getting and setting the UhmServer:enable-online property. */
static void
test_server_properties_enable_online (void)
{
	UhmServer *server;
	gboolean enable_online;
	guint counter;

	server = uhm_server_new ();

	counter = 0;
	g_signal_connect (G_OBJECT (server), "notify::enable-online", (GCallback) notify_emitted_cb, &counter);

	/* Check the default value. */
	g_assert (uhm_server_get_enable_online (server) == FALSE);
	g_object_get (G_OBJECT (server), "enable-online", &enable_online, NULL);
	g_assert (enable_online == FALSE);

	/* Toggle the value. */
	uhm_server_set_enable_online (server, TRUE);
	g_assert_cmpuint (counter, ==, 1);

	/* Check the new value can be retrieved via the getter and as a property. */
	g_assert (uhm_server_get_enable_online (server) == TRUE);
	g_object_get (G_OBJECT (server), "enable-online", &enable_online, NULL);
	g_assert (enable_online == TRUE);

	/* Toggle the value again, this time using the GObject setter. */
	g_object_set (G_OBJECT (server), "enable-online", FALSE, NULL);
	g_assert_cmpuint (counter, ==, 2);
	g_assert (uhm_server_get_enable_online (server) == FALSE);

	g_object_unref (server);
}

/* Test getting and setting UhmServer:enable-logging property. */
static void
test_server_properties_enable_logging (void)
{
	UhmServer *server;
	gboolean enable_logging;
	guint counter;

	server = uhm_server_new ();

	counter = 0;
	g_signal_connect (G_OBJECT (server), "notify::enable-logging", (GCallback) notify_emitted_cb, &counter);

	/* Check the default value. */
	g_assert (uhm_server_get_enable_logging (server) == FALSE);
	g_object_get (G_OBJECT (server), "enable-logging", &enable_logging, NULL);
	g_assert (enable_logging == FALSE);

	/* Toggle the value. */
	uhm_server_set_enable_logging (server, TRUE);
	g_assert_cmpuint (counter, ==, 1);

	/* Check the new value can be retrieved via the getter and as a property. */
	g_assert (uhm_server_get_enable_logging (server) == TRUE);
	g_object_get (G_OBJECT (server), "enable-logging", &enable_logging, NULL);
	g_assert (enable_logging == TRUE);

	/* Toggle the value again, this time using the GObject setter. */
	g_object_set (G_OBJECT (server), "enable-logging", FALSE, NULL);
	g_assert_cmpuint (counter, ==, 2);
	g_assert (uhm_server_get_enable_logging (server) == FALSE);

	g_object_unref (server);
}

/* Test getting the UhmServer:address property. */
static void
test_server_properties_address (void)
{
	UhmServer *server;
	SoupAddress *address;

	server = uhm_server_new ();

	/* Check the default value. */
	g_assert (uhm_server_get_address (server) == NULL);
	g_object_get (G_OBJECT (server), "address", &address, NULL);
	g_assert (address == NULL);

	/* The address is set when the server is taken online, which is tested separately. */

	g_object_unref (server);
}

/* Test getting the UhmServer:port property. */
static void
test_server_properties_port (void)
{
	UhmServer *server;
	guint port;

	server = uhm_server_new ();

	/* Check the default value. */
	g_assert (uhm_server_get_port (server) == 0);
	g_object_get (G_OBJECT (server), "port", &port, NULL);
	g_assert (port == 0);

	/* The port is set when the server is taken online, which is tested separately. */

	g_object_unref (server);
}

/* Test getting the UhmServer:resolver property. */
static void
test_server_properties_resolver (void)
{
	UhmServer *server;
	UhmResolver *resolver;

	server = uhm_server_new ();

	/* Check the default value. */
	g_assert (uhm_server_get_resolver (server) == NULL);
	g_object_get (G_OBJECT (server), "resolver", &resolver, NULL);
	g_assert (resolver == NULL);

	/* The resolver is set when the server is taken online, which is tested separately. */

	g_object_unref (server);
}

typedef struct {
	UhmServer *server;
	SoupSession *session;
	GMainLoop *main_loop;
} LoggingData;

static void
set_up_logging (LoggingData *data, gconstpointer user_data)
{
	UhmResolver *resolver;

	data->server = uhm_server_new ();
	data->main_loop = g_main_loop_new (NULL, FALSE);

	uhm_server_set_enable_logging (data->server, TRUE);
	uhm_server_set_enable_online (data->server, TRUE);

	if (user_data != NULL) {
		g_signal_connect (G_OBJECT (data->server), "handle-message", (GCallback) user_data, NULL);
	}

	uhm_server_run (data->server);

	resolver = uhm_server_get_resolver (data->server);
	uhm_resolver_add_A (resolver, "example.com", soup_address_get_physical (uhm_server_get_address (data->server)));

	data->session = soup_session_new_with_options (SOUP_SESSION_SSL_STRICT, FALSE, NULL);
}

static void
tear_down_logging (LoggingData *data, gconstpointer user_data)
{
	g_object_unref (data->session);
	uhm_server_stop (data->server);

	g_main_loop_unref (data->main_loop);
	g_object_unref (data->server);
}

static gboolean
server_logging_no_trace_success_handle_message_cb (UhmServer *server, SoupMessage *message, SoupClientContext *client)
{
	soup_message_set_status (message, SOUP_STATUS_OK);
	soup_message_body_append (message->response_body, SOUP_MEMORY_STATIC, "This is a success response.", strlen ("This is a success response."));
	soup_message_body_complete (message->response_body);

	return TRUE;
}

static gboolean
server_logging_no_trace_success_cb (LoggingData *data)
{
	SoupMessage *message;
	SoupURI *uri;

	/* Dummy unit test code. */
	uri = soup_uri_new ("https://example.com/test-file");
	soup_uri_set_port (uri, uhm_server_get_port (data->server));
	message = soup_message_new_from_uri (SOUP_METHOD_GET, uri);
	soup_uri_free (uri);

	g_assert_cmpuint (soup_session_send_message (data->session, message), ==, SOUP_STATUS_OK);

	g_object_unref (message);

	g_main_loop_quit (data->main_loop);

	return FALSE;
}

/* TODO */
static void
test_server_logging_no_trace_success (LoggingData *data, gconstpointer user_data)
{
	g_idle_add ((GSourceFunc) server_logging_no_trace_success_cb, data);
	g_main_loop_run (data->main_loop);
}

static gboolean
server_logging_no_trace_failure_handle_message_cb (UhmServer *server, SoupMessage *message, SoupClientContext *client)
{
	soup_message_set_status (message, SOUP_STATUS_PRECONDITION_FAILED);
	soup_message_body_append (message->response_body, SOUP_MEMORY_STATIC, "This is a failure response.", strlen ("This is a failure response."));
	soup_message_body_complete (message->response_body);

	return TRUE;
}

static gboolean
server_logging_no_trace_failure_cb (LoggingData *data)
{
	SoupMessage *message;
	SoupURI *uri;

	/* Dummy unit test code. */
	uri = soup_uri_new ("https://example.com/test-file");
	soup_uri_set_port (uri, uhm_server_get_port (data->server));
	message = soup_message_new_from_uri (SOUP_METHOD_GET, uri);
	soup_uri_free (uri);

	g_assert_cmpuint (soup_session_send_message (data->session, message), ==, SOUP_STATUS_PRECONDITION_FAILED);

	g_object_unref (message);

	g_main_loop_quit (data->main_loop);

	return FALSE;
}

/* TODO */
static void
test_server_logging_no_trace_failure (LoggingData *data, gconstpointer user_data)
{
	g_idle_add ((GSourceFunc) server_logging_no_trace_failure_cb, data);
	g_main_loop_run (data->main_loop);
}

static gboolean
server_logging_trace_success_cb (LoggingData *data)
{
	SoupMessage *message;
	SoupURI *uri;
	GFile *trace_file;
	GError *child_error = NULL;

	/* Load the trace. */
	trace_file = g_file_new_for_path ("server_logging_trace_success"); /* FIXME */
	uhm_server_load_trace (data->server, trace_file, NULL, &child_error);
	g_assert_no_error (child_error);
	g_object_unref (trace_file);

	/* Dummy unit test code. */
	uri = soup_uri_new ("https://example.com/test-file");
	soup_uri_set_port (uri, uhm_server_get_port (data->server));
	message = soup_message_new_from_uri (SOUP_METHOD_GET, uri);
	soup_uri_free (uri);

	g_assert_cmpuint (soup_session_send_message (data->session, message), ==, SOUP_STATUS_NOT_FOUND);

	g_object_unref (message);

	g_main_loop_quit (data->main_loop);

	return FALSE;
}

/* TODO */
static void
test_server_logging_trace_success (LoggingData *data, gconstpointer user_data)
{
	g_idle_add ((GSourceFunc) server_logging_trace_success_cb, data);
	g_main_loop_run (data->main_loop);
}

static gboolean
server_logging_trace_failure_method_cb (LoggingData *data)
{
	SoupMessage *message;
	SoupURI *uri;
	GFile *trace_file;
	GError *child_error = NULL;

	/* Load the trace. */
	trace_file = g_file_new_for_path ("server_logging_trace_failure_method"); /* FIXME */
	uhm_server_load_trace (data->server, trace_file, NULL, &child_error);
	g_assert_no_error (child_error);
	g_object_unref (trace_file);

	/* Dummy unit test code. */
	uri = soup_uri_new ("https://example.com/test-file");
	soup_uri_set_port (uri, uhm_server_get_port (data->server));
	message = soup_message_new_from_uri (SOUP_METHOD_PUT, uri); /* Note: we use PUT here, which doesn’t match the trace */
	soup_uri_free (uri);

	g_assert_cmpuint (soup_session_send_message (data->session, message), ==, SOUP_STATUS_BAD_REQUEST);
	g_assert_cmpstr (message->response_body->data, !=, "The document was not found. Ha.");
	g_assert_cmpstr (message->response_body->data, ==, "Expected GET URI ‘/test-file’, but got PUT ‘/test-file’.");
	if (strstr (soup_message_headers_get_one (message->response_headers, "X-Mock-Trace-File"), "server_logging_trace_failure_method") == NULL) {
		/* Report the error. */
		g_assert_cmpstr (soup_message_headers_get_one (message->response_headers, "X-Mock-Trace-File"), ==, "server_logging_trace_failure_method");
	}
	g_assert_cmpstr (soup_message_headers_get_one (message->response_headers, "X-Mock-Trace-File-Offset"), ==, "1");

	g_object_unref (message);

	g_main_loop_quit (data->main_loop);

	return FALSE;
}

/* TODO */
static void
test_server_logging_trace_failure_method (LoggingData *data, gconstpointer user_data)
{
	g_idle_add ((GSourceFunc) server_logging_trace_failure_method_cb, data);
	g_main_loop_run (data->main_loop);
}

int
main (int argc, char *argv[])
{
#if !GLIB_CHECK_VERSION (2, 35, 0)
	g_type_init ();
#endif

	g_test_init (&argc, &argv, NULL);
	g_test_bug_base ("http://bugzilla.gnome.org/show_bug.cgi?id=");

	/* Server tests. */
	g_test_add_func ("/server/construction", test_server_construction);

	g_test_add_func ("/server/properties/trace-directory", test_server_properties_trace_directory);
	g_test_add_func ("/server/properties/enable-online", test_server_properties_enable_online);
	g_test_add_func ("/server/properties/enable-logging", test_server_properties_enable_logging);
	g_test_add_func ("/server/properties/address", test_server_properties_address);
	g_test_add_func ("/server/properties/port", test_server_properties_port);
	g_test_add_func ("/server/properties/resolver", test_server_properties_resolver);

	g_test_add ("/server/logging/no-trace/success", LoggingData, server_logging_no_trace_success_handle_message_cb,
	            set_up_logging, test_server_logging_no_trace_success, tear_down_logging);
	g_test_add ("/server/logging/no-trace/failure", LoggingData, server_logging_no_trace_failure_handle_message_cb,
	            set_up_logging, test_server_logging_no_trace_failure, tear_down_logging);
	g_test_add ("/server/logging/trace/success", LoggingData, NULL,
	            set_up_logging, test_server_logging_trace_success, tear_down_logging);
	g_test_add ("/server/logging/trace/failure/method", LoggingData, NULL,
	            set_up_logging, test_server_logging_trace_failure_method, tear_down_logging);

	return g_test_run ();
}
