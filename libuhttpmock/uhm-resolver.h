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

#ifndef UHM_RESOLVER_H
#define UHM_RESOLVER_H

#include <glib.h>
#include <glib-object.h>
#include <gio/gio.h>

G_BEGIN_DECLS

#define UHM_TYPE_RESOLVER		(uhm_resolver_get_type ())
#define UHM_RESOLVER(o)			(G_TYPE_CHECK_INSTANCE_CAST ((o), UHM_TYPE_RESOLVER, UhmResolver))
#define UHM_RESOLVER_CLASS(k)		(G_TYPE_CHECK_CLASS_CAST((k), UHM_TYPE_RESOLVER, UhmResolverClass))
#define UHM_IS_RESOLVER(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), UHM_TYPE_RESOLVER))
#define UHM_IS_RESOLVER_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), UHM_TYPE_RESOLVER))
#define UHM_RESOLVER_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), UHM_TYPE_RESOLVER, UhmResolverClass))

typedef struct _UhmResolverPrivate	UhmResolverPrivate;

/**
 * UhmResolver:
 *
 * All the fields in the #UhmResolver structure are private and should never be accessed directly.
 *
 * Since: 0.1.0
 */
typedef struct {
	/*< private >*/
	GResolver parent;
	UhmResolverPrivate *priv;
} UhmResolver;

/**
 * UhmResolverClass:
 *
 * All the fields in the #UhmResolverClass structure are private and should never be accessed directly.
 *
 * Since: 0.1.0
 */
typedef struct {
	/*< private >*/
	GResolverClass parent;
} UhmResolverClass;

GType uhm_resolver_get_type (void) G_GNUC_CONST;

UhmResolver *uhm_resolver_new (void) G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT;

void uhm_resolver_reset (UhmResolver *self);

gboolean uhm_resolver_add_A (UhmResolver *self, const gchar *hostname, const gchar *addr);
gboolean uhm_resolver_add_SRV (UhmResolver *self, const gchar *service, const gchar *protocol, const gchar *domain, const gchar *addr, guint16 port);

G_END_DECLS

#endif /* !UHM_RESOLVER_H */
