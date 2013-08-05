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

/* Test TLS certificate for use below. */
static const gchar *test_tls_certificate =
	"-----BEGIN CERTIFICATE-----\n"
	"MIID6TCCAtGgAwIBAgIJAI9hYGsc661fMA0GCSqGSIb3DQEBBQUAMIGKMQswCQYD\n"
	"VQQGEwJHQjEVMBMGA1UEBwwMRGVmYXVsdCBDaXR5MREwDwYDVQQKDAhsaWJnZGF0\n"
	"YTEOMAwGA1UECwwFVGVzdHMxFzAVBgNVBAMMDmxpYmdkYXRhIHRlc3RzMSgwJgYJ\n"
	"KoZIhvcNAQkBFhlsaWJnZGF0YS1tYWludEBnbm9tZS5idWdzMB4XDTEzMDcwNjE3\n"
	"NDQxNFoXDTEzMDgwNTE3NDQxNFowgYoxCzAJBgNVBAYTAkdCMRUwEwYDVQQHDAxE\n"
	"ZWZhdWx0IENpdHkxETAPBgNVBAoMCGxpYmdkYXRhMQ4wDAYDVQQLDAVUZXN0czEX\n"
	"MBUGA1UEAwwObGliZ2RhdGEgdGVzdHMxKDAmBgkqhkiG9w0BCQEWGWxpYmdkYXRh\n"
	"LW1haW50QGdub21lLmJ1Z3MwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIB\n"
	"AQDCbpdfrtWTz+ZpNaVZuxaeAAY+f/xZz4wEH1gaNBNb3u9CPEWofW+fLNB6izkn\n"
	"f9qhx2K8PrM9LKHDJS4uUU9dkfQHQsrCSffRWqQTeOOnpYHjS21iDYdOt4e//f/J\n"
	"erAEIyWMQAP5eqMt4hp5wrfhSh2ul9lcHz2Lv5u6H+I8ygoUaMyH15WIlEzxOsn4\n"
	"i+lXSkdmso2n1FYbiMyyMYButnnmv+EcPvOdw88PB8Y6PnCN2Ye0fo9HvcJhHEdg\n"
	"GM4SKsejA/L+T8fX0FYCXrsLPYU0Ntm15ZV8nNsxCoZFGmdTs/prL8ztXaI1tYdi\n"
	"lI1RKVTOVxD2DKdrCs5bnxYhAgMBAAGjUDBOMB0GA1UdDgQWBBQwhz/4hEriPnF5\n"
	"F3TDY9TQLzxlnDAfBgNVHSMEGDAWgBQwhz/4hEriPnF5F3TDY9TQLzxlnDAMBgNV\n"
	"HRMEBTADAQH/MA0GCSqGSIb3DQEBBQUAA4IBAQAIVnySe76zjb5UHromPibgT9eL\n"
	"n8oZ76aGj6+VMLucpaK8K7U7y2ONAO+BB+wUyLaq48EYb6DmpFKThAxTajYd1f/7\n"
	"14bJIew8papMEooiJHyOVfZPLOePjCldV5+hPfwsfJ3NSDL8dc+IB2PQmgp32nom\n"
	"9uvQMOdD56hHSIP5zFZwNiWH75piWyUNs/+cbHKgnyVGWaoVd7z3E5dNelOMopbo\n"
	"qvWk2MM3nHpiQqMZyliTkq5uD2Q8WiXBD4rPUeEU55NaPslB8xKwldmrAlcwYvIg\n"
	"2SDhAsTPLcCgjNuHlr/v4VC29YsF37oYCGfmWvLwGFGufsxcxXkrVhhNr2WB\n"
	"-----END CERTIFICATE-----"
	"\n"
	"-----BEGIN PRIVATE KEY-----\n"
	"MIIEvwIBADANBgkqhkiG9w0BAQEFAASCBKkwggSlAgEAAoIBAQDCbpdfrtWTz+Zp\n"
	"NaVZuxaeAAY+f/xZz4wEH1gaNBNb3u9CPEWofW+fLNB6izknf9qhx2K8PrM9LKHD\n"
	"JS4uUU9dkfQHQsrCSffRWqQTeOOnpYHjS21iDYdOt4e//f/JerAEIyWMQAP5eqMt\n"
	"4hp5wrfhSh2ul9lcHz2Lv5u6H+I8ygoUaMyH15WIlEzxOsn4i+lXSkdmso2n1FYb\n"
	"iMyyMYButnnmv+EcPvOdw88PB8Y6PnCN2Ye0fo9HvcJhHEdgGM4SKsejA/L+T8fX\n"
	"0FYCXrsLPYU0Ntm15ZV8nNsxCoZFGmdTs/prL8ztXaI1tYdilI1RKVTOVxD2DKdr\n"
	"Cs5bnxYhAgMBAAECggEAaMkhW7fl8xuAmgMHciyaK9zngJeJcP2iADbETJr0M/ca\n"
	"CyBgikXP+oE0ela+HsORGM9ULw+7maSMKZfII74+f7dBRQiCLeOfY3zuIHBugNN6\n"
	"BP2JneacnZfb2WUSjYtJgXFPsx5tBe9KMlhA3I5Me2ZuSMIdqsBLcx141/6G9ysZ\n"
	"qSfez4FCAmPB3CO6zjUyMUMwYkAilkZgpmSMvE/HtW5e/NDnCKk0/n30E2xthCUC\n"
	"eWwAMvekpyssKyAHxEHk7XZoIjMldUI7erFHRgsr5HFypp2Q7Gcd5KXVeotV8Y67\n"
	"O/aAKXURhdESeqJUS0D9ezJg3ES6Q2YOgA61OcK74QKBgQD8O5HM/MyN86RQV69s\n"
	"VduYpndq+tKnXBAnLxD9D2cjEI9wUXJFOZJTfVYzeVxMY9iWmuKJq9IwJLG4/Zkp\n"
	"s66pukesnJvLki2hhTcoOFfMzT4ZT/CaDPOO6PAgvdBL581uX68Uh8rPn9bYqxCA\n"
	"IG0n8sA/1j4H86YQVWzpI4tPtwKBgQDFVgQ54TT/yRTVdhNYooPHzSOQIf/BfNBe\n"
	"JW3B1FWONeznwV81TNdvTmNiSU0SKF5/VMuM6g8QOqXYLCo0i9DD65tH16XaArhw\n"
	"ctxtHN5ivDDFxEP9mgsO5jeVvk+e3516S9o/8BzzmTwo1zTz6MMT/JedIC6shhSW\n"
	"OnT6cBcY5wKBgQD8DEbQ8VkzDGGIy2aHunAa5VX1uDjidnPJxBWU21xzxKuhUDIB\n"
	"DNu0xE1sWHyr9SZMsO9pJSJ/a1uRARGZg20pO/U9fq2MSkGA4w7QCSVriTjhsGk8\n"
	"d262wvyZqzPHdhZpkgHxYRSATzgxARgXANAzGDeWUu9foNC0B7kya4tdlwKBgQCm\n"
	"qY0MLS4L0ZIs7npMY4UU3CZq9qwAiB+bQ9U83M4dO2IIIgL9CxbwRK4fNnVHHp0g\n"
	"wUbgjlWGiWHD/xjuJB9/OJ9+v5ytUZrgLcIIzVbs4K/4d1hM+SrZvIm5iG/KaGWi\n"
	"Aioj0fFBs2thutBYJ3+Kg8ywwZtpzhvY/SoK0VxQhQKBgQCB2e3HKOFnMFBO93dZ\n"
	"IyaUmhTM0ad+cP94mX/7REVdmN2NUsG3brIoibkgESL1h+2UbEkeOKIYWD4+ZZYJ\n"
	"V4ZhTsRcefOgTqU+EOAYSwNvNCld3X5oBi+b9Ie6y1teT6As5zGnH4YspTa5iCAk\n"
	"M97r3MWTAb4wpTnkHHoJ0GIHpA==\n"
	"-----END PRIVATE KEY-----"
;

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
	gchar *address;

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

/* Test getting and setting the UhmServer:tls-certificate property. */
static void
test_server_properties_tls_certificate (void)
{
	UhmServer *server;
	GTlsCertificate *tls_certificate, *new_tls_certificate;
	guint counter;
	GError *child_error = NULL;

	server = uhm_server_new ();

	counter = 0;
	g_signal_connect (G_OBJECT (server), "notify::tls-certificate", (GCallback) notify_emitted_cb, &counter);

	/* Check the default value. */
	g_assert (uhm_server_get_tls_certificate (server) == NULL);
	g_object_get (G_OBJECT (server), "tls-certificate", &tls_certificate, NULL);
	g_assert (tls_certificate == NULL);

	/* Set the tls-certificate to an arbitrary, existent, directory. */
	new_tls_certificate = g_tls_certificate_new_from_pem (test_tls_certificate, -1, &child_error);
	g_assert_no_error (child_error);
	uhm_server_set_tls_certificate (server, new_tls_certificate);
	g_assert_cmpuint (counter, ==, 1);

	/* Check the new certificate can be retrieved via the getter. */
	tls_certificate = uhm_server_get_tls_certificate (server);
	g_assert (G_IS_TLS_CERTIFICATE (tls_certificate));
	g_assert (g_tls_certificate_is_same (tls_certificate, new_tls_certificate) == TRUE);

	/* Check the new certificate can be retrieved as a property. */
	g_object_get (G_OBJECT (server), "tls-certificate", &tls_certificate, NULL);
	g_assert (G_IS_TLS_CERTIFICATE (tls_certificate));
	g_assert (g_tls_certificate_is_same (tls_certificate, new_tls_certificate) == TRUE);

	g_object_unref (tls_certificate);
	g_object_unref (new_tls_certificate);

	/* Set the certificate to NULL again, this time using the GObject setter. */
	g_object_set (G_OBJECT (server), "tls-certificate", NULL, NULL);
	g_assert_cmpuint (counter, ==, 2);
	g_assert (uhm_server_get_tls_certificate (server) == NULL);

	/* Set the certificate to the default. */
	new_tls_certificate = uhm_server_set_default_tls_certificate (server);
	g_assert (G_IS_TLS_CERTIFICATE (new_tls_certificate));
	tls_certificate = uhm_server_get_tls_certificate (server);
	g_assert (g_tls_certificate_is_same (tls_certificate, new_tls_certificate) == TRUE);
	g_assert_cmpuint (counter, ==, 3);

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
	uhm_server_set_default_tls_certificate (data->server);

	if (user_data != NULL) {
		g_signal_connect (G_OBJECT (data->server), "handle-message", (GCallback) user_data, NULL);
	}

	uhm_server_run (data->server);

	resolver = uhm_server_get_resolver (data->server);
	uhm_resolver_add_A (resolver, "example.com", uhm_server_get_address (data->server));

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

/* Test a server in onling/logging mode returning a success response from a custom signal handler. */
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

/* Test a server in onling/logging mode returning a failure response from a custom signal handler. */
static void
test_server_logging_no_trace_failure (LoggingData *data, gconstpointer user_data)
{
	g_idle_add ((GSourceFunc) server_logging_no_trace_failure_cb, data);
	g_main_loop_run (data->main_loop);
}

static gboolean
server_logging_trace_success_normal_cb (LoggingData *data)
{
	SoupMessage *message;
	SoupURI *uri;
	GFile *trace_file;
	GError *child_error = NULL;

	/* Load the trace. */
	trace_file = g_file_new_for_path ("server_logging_trace_success_normal"); /* FIXME */
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

/* Test a server in onling/logging mode returning a success response from a single-message trace. */
static void
test_server_logging_trace_success_normal (LoggingData *data, gconstpointer user_data)
{
	g_idle_add ((GSourceFunc) server_logging_trace_success_normal_cb, data);
	g_main_loop_run (data->main_loop);
}

static gboolean
server_logging_trace_success_multiple_messages_cb (LoggingData *data)
{
	SoupMessage *message;
	SoupURI *uri;
	GFile *trace_file;
	GError *child_error = NULL;
	guint i;
	SoupKnownStatusCode expected_status_codes[] = {
		SOUP_STATUS_OK,
		SOUP_STATUS_OK,
		SOUP_STATUS_NOT_FOUND,
	};

	/* Load the trace. */
	trace_file = g_file_new_for_path ("server_logging_trace_success_multiple-messages"); /* FIXME */
	uhm_server_load_trace (data->server, trace_file, NULL, &child_error);
	g_assert_no_error (child_error);
	g_object_unref (trace_file);

	/* Dummy unit test code. Send three messages. */
	for (i = 0; i < G_N_ELEMENTS (expected_status_codes); i++) {
		gchar *uri_string;

		uri_string = g_strdup_printf ("https://example.com/test-file%u", i);
		uri = soup_uri_new (uri_string);
		soup_uri_set_port (uri, uhm_server_get_port (data->server));
		g_free (uri_string);

		message = soup_message_new_from_uri (SOUP_METHOD_GET, uri);
		g_assert_cmpuint (soup_session_send_message (data->session, message), ==, expected_status_codes[i]);

		soup_uri_free (uri);
		g_object_unref (message);
	}

	g_main_loop_quit (data->main_loop);

	return FALSE;
}

/* Test a server in onling/logging mode returning several responses from a multi-message trace. */
static void
test_server_logging_trace_success_multiple_messages (LoggingData *data, gconstpointer user_data)
{
	g_idle_add ((GSourceFunc) server_logging_trace_success_multiple_messages_cb, data);
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

/* Test a server in onling/logging mode returning a non-matching response from a trace, not matching by method. */
static void
test_server_logging_trace_failure_method (LoggingData *data, gconstpointer user_data)
{
	g_idle_add ((GSourceFunc) server_logging_trace_failure_method_cb, data);
	g_main_loop_run (data->main_loop);
}

static gboolean
server_logging_trace_failure_uri_cb (LoggingData *data)
{
	SoupMessage *message;
	SoupURI *uri;
	GFile *trace_file;
	GError *child_error = NULL;

	/* Load the trace. */
	trace_file = g_file_new_for_path ("server_logging_trace_failure_uri"); /* FIXME */
	uhm_server_load_trace (data->server, trace_file, NULL, &child_error);
	g_assert_no_error (child_error);
	g_object_unref (trace_file);

	/* Dummy unit test code. */
	uri = soup_uri_new ("https://example.com/test-file-wrong-uri"); /* Note: wrong URI */
	soup_uri_set_port (uri, uhm_server_get_port (data->server));
	message = soup_message_new_from_uri (SOUP_METHOD_GET, uri);
	soup_uri_free (uri);

	g_assert_cmpuint (soup_session_send_message (data->session, message), ==, SOUP_STATUS_BAD_REQUEST);
	g_assert_cmpstr (message->response_body->data, !=, "The document was not found. Ha.");
	g_assert_cmpstr (message->response_body->data, ==, "Expected GET URI ‘/test-file’, but got GET ‘/test-file-wrong-uri’.");
	if (strstr (soup_message_headers_get_one (message->response_headers, "X-Mock-Trace-File"), "server_logging_trace_failure_uri") == NULL) {
		/* Report the error. */
		g_assert_cmpstr (soup_message_headers_get_one (message->response_headers, "X-Mock-Trace-File"), ==, "server_logging_trace_failure_uri");
	}
	g_assert_cmpstr (soup_message_headers_get_one (message->response_headers, "X-Mock-Trace-File-Offset"), ==, "1");

	g_object_unref (message);

	g_main_loop_quit (data->main_loop);

	return FALSE;
}

/* Test a server in onling/logging mode returning a non-matching response from a trace, not matching by URI. */
static void
test_server_logging_trace_failure_uri (LoggingData *data, gconstpointer user_data)
{
	g_idle_add ((GSourceFunc) server_logging_trace_failure_uri_cb, data);
	g_main_loop_run (data->main_loop);
}

static gboolean
server_logging_trace_failure_unexpected_request_cb (LoggingData *data)
{
	SoupMessage *message;
	SoupURI *uri;
	GFile *trace_file;
	GError *child_error = NULL;

	/* Load the trace. */
	trace_file = g_file_new_for_path ("server_logging_trace_failure_unexpected-request"); /* FIXME */
	uhm_server_load_trace (data->server, trace_file, NULL, &child_error);
	g_assert_no_error (child_error);
	g_object_unref (trace_file);

	/* Dummy unit test code. */
	uri = soup_uri_new ("https://example.com/test-file-unexpected"); /* Note: unexpected request; not in the trace file */
	soup_uri_set_port (uri, uhm_server_get_port (data->server));
	message = soup_message_new_from_uri (SOUP_METHOD_GET, uri);
	soup_uri_free (uri);

	g_assert_cmpuint (soup_session_send_message (data->session, message), ==, SOUP_STATUS_BAD_REQUEST);
	g_assert_cmpstr (message->response_body->data, !=, "The document was not found. Ha.");
	g_assert_cmpstr (message->response_body->data, ==, "Expected no request, but got GET ‘/test-file-unexpected’.");
	if (strstr (soup_message_headers_get_one (message->response_headers, "X-Mock-Trace-File"), "server_logging_trace_failure_unexpected-request") == NULL) {
		/* Report the error. */
		g_assert_cmpstr (soup_message_headers_get_one (message->response_headers, "X-Mock-Trace-File"), ==, "server_logging_trace_failure_unexpected-request");
	}
	g_assert_cmpstr (soup_message_headers_get_one (message->response_headers, "X-Mock-Trace-File-Offset"), ==, "0");

	g_object_unref (message);

	g_main_loop_quit (data->main_loop);

	return FALSE;
}

/* Test a server in onling/logging mode by making a request which doesn't exist in the trace file. */
static void
test_server_logging_trace_failure_unexpected_request (LoggingData *data, gconstpointer user_data)
{
	g_idle_add ((GSourceFunc) server_logging_trace_failure_unexpected_request_cb, data);
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
	g_test_add_func ("/server/properties/tls-certificate", test_server_properties_tls_certificate);

	g_test_add ("/server/logging/no-trace/success", LoggingData, server_logging_no_trace_success_handle_message_cb,
	            set_up_logging, test_server_logging_no_trace_success, tear_down_logging);
	g_test_add ("/server/logging/no-trace/failure", LoggingData, server_logging_no_trace_failure_handle_message_cb,
	            set_up_logging, test_server_logging_no_trace_failure, tear_down_logging);
	g_test_add ("/server/logging/trace/success/normal", LoggingData, NULL,
	            set_up_logging, test_server_logging_trace_success_normal, tear_down_logging);
	g_test_add ("/server/logging/trace/success/multiple-messages", LoggingData, NULL,
	            set_up_logging, test_server_logging_trace_success_multiple_messages, tear_down_logging);
	g_test_add ("/server/logging/trace/failure/method", LoggingData, NULL,
	            set_up_logging, test_server_logging_trace_failure_method, tear_down_logging);
	g_test_add ("/server/logging/trace/failure/uri", LoggingData, NULL,
	            set_up_logging, test_server_logging_trace_failure_uri, tear_down_logging);
	g_test_add ("/server/logging/trace/failure/unexpected-request", LoggingData, NULL,
	            set_up_logging, test_server_logging_trace_failure_unexpected_request, tear_down_logging);

	return g_test_run ();
}
