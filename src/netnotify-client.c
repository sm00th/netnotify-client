#include "netnotify-client.h"
#include "config.h"

void show_notification( const char *img, const char *summary, const char *text, const int timeout )
{
    NotifyNotification *notify;

#if NOTIFY_VERSION_MAJOR == 0 && NOTIFY_VERSION_MINOR >= 7
    notify = notify_notification_new( summary, text, ( img == NULL ? DEFAULT_ICON : img ) );
#else
    notify = notify_notification_new( summary, text, ( img == NULL ? DEFAULT_ICON : img ), NULL );
#endif

    notify_notification_set_timeout( notify, timeout );
    notify_notification_show( notify, NULL );

    g_object_unref ( G_OBJECT( notify ) );
}

static gchar *read_till( GIOChannel *source, const gchar *stop_string )
{
    GString *tmp = g_string_new( NULL );
    GString *result = g_string_new( "" );
    GError *error = NULL;

    while( g_io_channel_read_line_string( source, tmp, NULL, &error ) == G_IO_STATUS_NORMAL ) {
        if( !g_strcmp0( tmp->str, stop_string ) ) {
            break;
        }
        result = g_string_append( result, tmp->str );
    }
    g_string_free( tmp, TRUE );
    g_strchomp( result->str );
    return g_string_free( result, FALSE );
}

static gboolean incoming_message( GIOChannel *source, GIOCondition cond, gpointer userdata )
{
    GString *msg = g_string_new( NULL );
    GError *error = NULL;
    GMainLoop *loop = (GMainLoop*) userdata;
    GIOStatus ret;
    gchar *summary = NULL, *body = NULL, *img = NULL;
    int timeout = -1;

    if( cond == G_IO_ERR || cond == G_IO_HUP ) {
        g_printerr( "Connection lost\n" );
        g_main_loop_quit( loop );
        return FALSE;
    }

    while( ( ret = g_io_channel_read_line_string( source, msg, NULL, &error ) ) == G_IO_STATUS_NORMAL ) {
        if( g_strrstr( msg->str, NETNOTIFY_HELLO ) != NULL ) {
            g_print( "Succesfully connected\n" );
        } else if( !g_strcmp0( msg->str, SUMMARY_STR"\n" ) ) {
            summary = read_till( source, "END"SUMMARY_STR"\n" );
        } else if( !g_strcmp0( msg->str, BODY_STR"\n" ) ) {
            body = read_till( source, "END"BODY_STR"\n" );
        } else if( !g_strcmp0( msg->str, IMG_STR"\n" ) ) {
            img = read_till( source, "END"IMG_STR"\n" );
        } else if( !g_strcmp0( msg->str, TIMEOUT_STR"\n" ) ) {
            gchar *tmp = read_till( source, "END"TIMEOUT_STR"\n" );
            timeout = g_ascii_strtoll( tmp, NULL, 10 );
            g_free( tmp );
        }
    }
    g_string_free( msg, TRUE );

    if( summary != NULL ) {
        show_notification( img, summary, body, timeout );
        g_free( summary );
        if( body != NULL ) {
            g_free( body );
        }
        if( img != NULL ) {
            g_free( img );
        }
    }

    if ( ret == G_IO_STATUS_ERROR ) {
        g_printerr( "Error reading: %s\n", error->message );
    } else if( ret == G_IO_STATUS_EOF ) {
        g_printerr( "Connection lost\n" );
        g_main_loop_quit( loop );
        return FALSE;
    }

    return TRUE;
}

int main( int argc, char *argv[] )
{
    GSocketConnection *conn;
    GSocket *socket;
    GError *error;
    GMainLoop *loop;
    GIOChannel *io_channel;

    if( argc < 2 ) {
        g_printerr( "Usage: %s <host> <port>\n", argv[0] );
        exit( 1 );
    }

    g_type_init();

    if ( !notify_init( "notify-send" ) ) {
        g_printerr( "Failed to initialize notify\n" );
        exit( 1 );
    }


    error  = NULL;
    conn   = g_socket_client_connect(
            g_socket_client_new(),
            g_network_address_new( argv[1], g_ascii_strtoull( argv[2], NULL, 10 ) ),
            NULL,
            &error );

    if( conn == NULL ) {
        g_printerr( "Failed to connect: %s\n", error->message );
        g_error_free( error );
        exit( 1 );
    }

    loop = g_main_loop_new( NULL, FALSE );

    socket = g_socket_connection_get_socket( conn );
    io_channel = g_io_channel_unix_new( g_socket_get_fd( socket ) );
    g_io_add_watch( io_channel, G_IO_IN | G_IO_ERR | G_IO_HUP, incoming_message, loop );

    g_main_loop_run( loop );

    g_io_channel_unref( io_channel );
    g_object_unref( conn );
    notify_uninit();
    return 0;
}
