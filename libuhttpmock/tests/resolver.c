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

#include "uhm-resolver.h"

typedef struct {
	UhmResolver *resolver;
	GMainLoop *main_loop;
} AsyncData;

/* Construct a new UhmResolver and see if anything explodes. */
static void
test_resolver_construction (void)
{
	UhmResolver *resolver;

	resolver = uhm_resolver_new ();
	g_assert (UHM_IS_RESOLVER (resolver));
	g_object_unref (resolver);
}

static void
assert_single_address_result (GList/*<GInetAddress>*/ *addresses, const gchar *expected_ip_address)
{
	gchar *address_string;

	g_assert_cmpuint (g_list_length (addresses), ==, 1);

	address_string = g_inet_address_to_string (G_INET_ADDRESS (addresses->data));
	g_assert_cmpstr (address_string, ==, expected_ip_address);
	g_free (address_string);
}

/* Add A records and query for existent and non-existent ones. Test that resolution fails after resetting the resolver. */
static void
test_resolver_lookup_by_name (void)
{
	UhmResolver *resolver;
	GError *child_error = NULL;
	GList/*<GInetAddress>*/ *addresses = NULL;

	resolver = uhm_resolver_new ();

	/* Add some domains and then query them. */
	uhm_resolver_add_A (resolver, "example.com", "127.0.0.1");
	uhm_resolver_add_A (resolver, "test.com", "10.0.0.1");

	/* Query for example.com. */
	addresses = g_resolver_lookup_by_name (G_RESOLVER (resolver), "example.com", NULL, &child_error);
	g_assert_no_error (child_error);
	assert_single_address_result (addresses, "127.0.0.1");
	g_resolver_free_addresses (addresses);

	/* Query for nonexistent.com. */
	addresses = g_resolver_lookup_by_name (G_RESOLVER (resolver), "nonexistent.com", NULL, &child_error);
	g_assert_error (child_error, G_RESOLVER_ERROR, G_RESOLVER_ERROR_NOT_FOUND);
	g_assert (addresses == NULL);
	g_clear_error (&child_error);

	/* Reset and then query for example.com again. */
	uhm_resolver_reset (resolver);

	addresses = g_resolver_lookup_by_name (G_RESOLVER (resolver), "example.com", NULL, &child_error);
	g_assert_error (child_error, G_RESOLVER_ERROR, G_RESOLVER_ERROR_NOT_FOUND);
	g_assert (addresses == NULL);
	g_clear_error (&child_error);

	g_object_unref (resolver);
}

static void
resolver_lookup_by_name_async_success_cb (GObject *source_object, GAsyncResult *result, AsyncData *data)
{
	GList/*<GInetAddress>*/ *addresses;
	GError *child_error = NULL;

	addresses = g_resolver_lookup_by_name_finish (G_RESOLVER (data->resolver), result, &child_error);
	g_assert_no_error (child_error);
	assert_single_address_result (addresses, "127.0.0.1");
	g_resolver_free_addresses (addresses);

	g_main_loop_quit (data->main_loop);
}

static void
resolver_lookup_by_name_async_failure_cb (GObject *source_object, GAsyncResult *result, AsyncData *data)
{
	GList/*<GInetAddress>*/ *addresses;
	GError *child_error = NULL;

	addresses = g_resolver_lookup_by_name_finish (G_RESOLVER (data->resolver), result, &child_error);
	g_assert_error (child_error, G_RESOLVER_ERROR, G_RESOLVER_ERROR_NOT_FOUND);
	g_assert (addresses == NULL);
	g_clear_error (&child_error);

	g_main_loop_quit (data->main_loop);
}

/* Add an A record and asynchronously query for existent and non-existent ones. Test that resolution fails after resetting the resolver. */
static void
test_resolver_lookup_by_name_async (void)
{
	AsyncData data;

	data.main_loop = g_main_loop_new (NULL, FALSE);
	data.resolver = uhm_resolver_new ();

	/* Add a domain and query it. */
	uhm_resolver_add_A (data.resolver, "example.com", "127.0.0.1");

	g_resolver_lookup_by_name_async (G_RESOLVER (data.resolver), "example.com", NULL, (GAsyncReadyCallback) resolver_lookup_by_name_async_success_cb, &data);
	g_main_loop_run (data.main_loop);

	/* Query for a non-existent domain. */
	g_resolver_lookup_by_name_async (G_RESOLVER (data.resolver), "nonexistent.com", NULL, (GAsyncReadyCallback) resolver_lookup_by_name_async_failure_cb, &data);
	g_main_loop_run (data.main_loop);

	/* Reset and query for example.com again. */
	uhm_resolver_reset (data.resolver);

	g_resolver_lookup_by_name_async (G_RESOLVER (data.resolver), "example.com", NULL, (GAsyncReadyCallback) resolver_lookup_by_name_async_failure_cb, &data);
	g_main_loop_run (data.main_loop);

	g_object_unref (data.resolver);
	g_main_loop_unref (data.main_loop);
}

static void
assert_single_service_result (GList/*<GSrvTarget>*/ *services, const gchar *expected_ip_address, guint16 expected_port)
{
	g_assert_cmpuint (g_list_length (services), ==, 1);

	g_assert_cmpstr (g_srv_target_get_hostname ((GSrvTarget *) services->data), ==, expected_ip_address);
	g_assert_cmpuint (g_srv_target_get_port ((GSrvTarget *) services->data), ==, expected_port);
}

/* Add SRV records and query for existent and non-existent ones. Test that resolution fails after resetting the resolver. */
static void
test_resolver_lookup_service (void)
{
	UhmResolver *resolver;
	GError *child_error = NULL;
	GList/*<GSrvTarget>*/ *services = NULL;

	resolver = uhm_resolver_new ();

	/* Add some services and then query them. */
	uhm_resolver_add_SRV (resolver, "ldap", "tcp", "example.com", "127.0.0.5", 666);
	uhm_resolver_add_SRV (resolver, "imap", "tcp", "test.com", "10.0.0.1", 1234);

	/* Query for the LDAP service. */
	services = g_resolver_lookup_service (G_RESOLVER (resolver), "ldap", "tcp", "example.com", NULL, &child_error);
	g_assert_no_error (child_error);
	assert_single_service_result (services, "127.0.0.5", 666);
	g_resolver_free_targets (services);

	/* Query for a non-existent XMPP service. */
	services = g_resolver_lookup_service (G_RESOLVER (resolver), "xmpp", "tcp", "jabber.org", NULL, &child_error);
	g_assert_error (child_error, G_RESOLVER_ERROR, G_RESOLVER_ERROR_NOT_FOUND);
	g_assert (services == NULL);
	g_clear_error (&child_error);

	/* Reset and then query for the LDAP service again. */
	uhm_resolver_reset (resolver);

	services = g_resolver_lookup_service (G_RESOLVER (resolver), "ldap", "tcp", "example.com", NULL, &child_error);
	g_assert_error (child_error, G_RESOLVER_ERROR, G_RESOLVER_ERROR_NOT_FOUND);
	g_assert (services == NULL);
	g_clear_error (&child_error);

	g_object_unref (resolver);
}

static void
resolver_lookup_service_async_success_cb (GObject *source_object, GAsyncResult *result, AsyncData *data)
{
	GList/*<GSrvTarget>*/ *services;
	GError *child_error = NULL;

	services = g_resolver_lookup_service_finish (G_RESOLVER (data->resolver), result, &child_error);
	g_assert_no_error (child_error);
	assert_single_service_result (services, "127.0.0.5", 666);
	g_resolver_free_targets (services);

	g_main_loop_quit (data->main_loop);
}

static void
resolver_lookup_service_async_failure_cb (GObject *source_object, GAsyncResult *result, AsyncData *data)
{
	GList/*<GSrvTarget>*/ *services;
	GError *child_error = NULL;

	services = g_resolver_lookup_service_finish (G_RESOLVER (data->resolver), result, &child_error);
	g_assert_error (child_error, G_RESOLVER_ERROR, G_RESOLVER_ERROR_NOT_FOUND);
	g_assert (services == NULL);
	g_clear_error (&child_error);

	g_main_loop_quit (data->main_loop);
}

/* Add an SRV record and asynchronously query for existent and non-existent ones. Test that resolution fails after resetting the resolver. */
static void
test_resolver_lookup_service_async (void)
{
	AsyncData data;

	data.main_loop = g_main_loop_new (NULL, FALSE);
	data.resolver = uhm_resolver_new ();

	/* Add a service and query it. */
	uhm_resolver_add_SRV (data.resolver, "ldap", "tcp", "example.com", "127.0.0.5", 666);

	g_resolver_lookup_service_async (G_RESOLVER (data.resolver), "ldap", "tcp", "example.com", NULL,
	                                 (GAsyncReadyCallback) resolver_lookup_service_async_success_cb, &data);
	g_main_loop_run (data.main_loop);

	/* Query for a non-existent service. */
	g_resolver_lookup_service_async (G_RESOLVER (data.resolver), "xmpp", "tcp", "nonexistent.com", NULL,
	                                 (GAsyncReadyCallback) resolver_lookup_service_async_failure_cb, &data);
	g_main_loop_run (data.main_loop);

	/* Reset and query for the LDAP service again. */
	uhm_resolver_reset (data.resolver);

	g_resolver_lookup_service_async (G_RESOLVER (data.resolver), "ldap", "tcp", "example.com", NULL,
	                                 (GAsyncReadyCallback) resolver_lookup_service_async_failure_cb, &data);
	g_main_loop_run (data.main_loop);

	g_object_unref (data.resolver);
	g_main_loop_unref (data.main_loop);
}

int
main (int argc, char *argv[])
{
#if !GLIB_CHECK_VERSION (2, 35, 0)
	g_type_init ();
#endif

	g_test_init (&argc, &argv, NULL);
	g_test_bug_base ("http://bugzilla.gnome.org/show_bug.cgi?id=");

	/* Resolver tests. */
	g_test_add_func ("/resolver/construction", test_resolver_construction);

	g_test_add_func ("/resolver/lookup-by-name", test_resolver_lookup_by_name);
	g_test_add_func ("/resolver/lookup-by-name/async", test_resolver_lookup_by_name_async);
	g_test_add_func ("/resolver/lookup-service", test_resolver_lookup_service);
	g_test_add_func ("/resolver/lookup-service/async", test_resolver_lookup_service_async);

	return g_test_run ();
}
