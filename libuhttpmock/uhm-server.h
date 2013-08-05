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

#ifndef UHM_SERVER_H
#define UHM_SERVER_H

#include <glib.h>
#include <glib-object.h>
#include <gio/gio.h>
#include <libsoup/soup.h>

#include "uhm-resolver.h"

G_BEGIN_DECLS

/**
 * UhmServerError:
 * @UHM_SERVER_ERROR_MESSAGE_MISMATCH: In comparison mode, a message received from the client did not match the next message in the current trace file.
 *
 * Error codes for #UhmServer operations.
 **/
typedef enum {
	UHM_SERVER_ERROR_MESSAGE_MISMATCH = 1,
} UhmServerError;

#define UHM_SERVER_ERROR		uhm_server_error_quark ()

GQuark uhm_server_error_quark (void) G_GNUC_CONST;

#define UHM_TYPE_SERVER			(uhm_server_get_type ())
#define UHM_SERVER(o)			(G_TYPE_CHECK_INSTANCE_CAST ((o), UHM_TYPE_SERVER, UhmServer))
#define UHM_SERVER_CLASS(k)		(G_TYPE_CHECK_CLASS_CAST((k), UHM_TYPE_SERVER, UhmServerClass))
#define UHM_IS_SERVER(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), UHM_TYPE_SERVER))
#define UHM_IS_SERVER_CLASS(k)		(G_TYPE_CHECK_CLASS_TYPE ((k), UHM_TYPE_SERVER))
#define UHM_SERVER_GET_CLASS(o)		(G_TYPE_INSTANCE_GET_CLASS ((o), UHM_TYPE_SERVER, UhmServerClass))

typedef struct _UhmServerPrivate	UhmServerPrivate;

/**
 * UhmServer:
 *
 * All the fields in the #UhmServer structure are private and should never be accessed directly.
 *
 * Since: UNRELEASED
 */
typedef struct {
	/*< private >*/
	GObject parent;
	UhmServerPrivate *priv;
} UhmServer;

/**
 * UhmServerClass:
 * @handle_message: Class handler for the #UhmServer::handle-message signal. Subclasses may implement this to override the
 * default handler for the signal. The default handler should always return %TRUE to indicate that it has handled
 * the @message from @client by setting an appropriate response on the #SoupMessage.
 * @compare_messages: Class handler for the #UhmServer::compare-messages signal. Subclasses may implement this to override
 * the default handler for the signal. The handler should return %TRUE if @expected_message and @actual_message compare
 * equal, and %FALSE otherwise.
 *
 * Most of the fields in the #UhmServerClass structure are private and should never be accessed directly.
 *
 * Since: UNRELEASED
 */
typedef struct {
	/*< private >*/
	GObjectClass parent;

	/*< public >*/
	gboolean (*handle_message) (UhmServer *self, SoupMessage *message, SoupClientContext *client);
	gboolean (*compare_messages) (UhmServer *self, SoupMessage *expected_message, SoupMessage *actual_message, SoupClientContext *actual_client);
} UhmServerClass;

GType uhm_server_get_type (void) G_GNUC_CONST;

UhmServer *uhm_server_new (void) G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT;

void uhm_server_load_trace (UhmServer *self, GFile *trace_file, GCancellable *cancellable, GError **error);
void uhm_server_load_trace_async (UhmServer *self, GFile *trace_file, GCancellable *cancellable, GAsyncReadyCallback callback, gpointer user_data);
void uhm_server_load_trace_finish (UhmServer *self, GAsyncResult *result, GError **error);
void uhm_server_unload_trace (UhmServer *self);

void uhm_server_run (UhmServer *self);
void uhm_server_stop (UhmServer *self);

GFile *uhm_server_get_trace_directory (UhmServer *self);
void uhm_server_set_trace_directory (UhmServer *self, GFile *trace_directory);

void uhm_server_start_trace (UhmServer *self, const gchar *trace_name, GError **error);
void uhm_server_start_trace_full (UhmServer *self, GFile *trace_file, GError **error);
void uhm_server_end_trace (UhmServer *self);

gboolean uhm_server_get_enable_online (UhmServer *self);
void uhm_server_set_enable_online (UhmServer *self, gboolean enable_online);

gboolean uhm_server_get_enable_logging (UhmServer *self);
void uhm_server_set_enable_logging (UhmServer *self, gboolean enable_logging);

void uhm_server_received_message_chunk (UhmServer *self, const gchar *message_chunk, goffset message_chunk_length, GError **error);

const gchar *uhm_server_get_address (UhmServer *self);
guint uhm_server_get_port (UhmServer *self);

UhmResolver *uhm_server_get_resolver (UhmServer *self);

GTlsCertificate *uhm_server_get_tls_certificate (UhmServer *self);
void uhm_server_set_tls_certificate (UhmServer *self, GTlsCertificate *tls_certificate);

GTlsCertificate *uhm_server_set_default_tls_certificate (UhmServer *self) G_GNUC_MALLOC;

G_END_DECLS

#endif /* !UHM_SERVER_H */
