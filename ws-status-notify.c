/*
 * Copyright (c) 2007,2009 Brian Tarricone <bjt23@cornell.edu>
 *
 * Licensed for use and redistribution under the terms of the GNU General
 * Public License, version 2 ONLY.  See:
 * http://www.gnu.org/licenses/gpl.html
 */

/*
 * Updated compile line for CentOS 7
 * cc -o ws-status-notify ws-status-notify.c `pkg-config libwnck-3.0 gtk+-3.0 libnotify --cflags --libs`
 *
 */

/* To compile:
 *   cc -o ws-status-notify ws-status-notify.c `pkg-config libwnck-1.0 libnotify --cflags --libs`
 */

#include <gtk/gtk.h>
#include <libnotify/notify.h>
#define WNCK_I_KNOW_THIS_IS_UNSTABLE
#include <libwnck/libwnck.h>

static NotifyNotification *notif = NULL;

static void
ws_status_workspace_changed(WnckScreen *wnck_screen,
                            WnckWorkspace *previous_workspace,
                            gpointer user_data)
{
    WnckWorkspace *ws = wnck_screen_get_active_workspace(wnck_screen);
    gchar *name = g_markup_escape_text(wnck_workspace_get_name(ws), -1);

    if(notif)
        notify_notification_update(notif, "Workspace Changed", name, "desktop");
    else {
        notif = notify_notification_new("Workspace Changed", name, "desktop");
        g_signal_connect(G_OBJECT(notif), "closed",
                         G_CALLBACK(g_object_unref), NULL);
        g_object_add_weak_pointer(G_OBJECT(notif), (gpointer)&notif);
    }

    notify_notification_show(notif, NULL);
    g_free(name);
}

int
main(int argc,
     char **argv)
{
    WnckScreen *wnck_screen;

    gtk_init(&argc, &argv);

    if(!notify_init("Workspace Status")) {
        g_printerr("Failed to connect to notification daemon\n");
        return 1;
    }

    wnck_screen = wnck_screen_get_default();
    wnck_screen_force_update(wnck_screen);
    g_signal_connect(G_OBJECT(wnck_screen), "active-workspace-changed",
                     G_CALLBACK(ws_status_workspace_changed), NULL);

    gtk_main();

    return 0;
}
