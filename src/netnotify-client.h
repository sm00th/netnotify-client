#include <libnotify/notify.h>
#include <stdlib.h>
#include <gio/gio.h>

#define NETNOTIFY_HELLO "OK NETNOTIFY"

#define SUMMARY_STR "SUM"
#define BODY_STR "BODY"
#define IMG_STR "IMG"
#define TIMEOUT_STR "TOUT"

#define DEFAULT_ICON "notification-message-im"

void show_notification( const char *img, const char *summary, const char *text, const int timeout );
static gchar *read_till( GIOChannel *source, const gchar *stop_string );
static gboolean incoming_message( GIOChannel *source, GIOCondition cond, gpointer userdata );
