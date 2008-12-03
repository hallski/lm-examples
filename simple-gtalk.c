/*
 * Copyright (C) 2008 Mikael Hallendal <micke@imendio.com>
 */

#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <loudmouth/loudmouth.h>

static GMainLoop *main_loop = NULL;

typedef struct {
    const gchar *username;
    const gchar *password;
    const gchar *test_contact;
    const gchar *test_message;
} Arguments;

static void
print_usage (void)
{
    g_print ("Usage: simple-gtalk <username> <password> <test-contact-jid> <test-message>\n");
}

static gchar *
ensure_full_jid (const gchar *username)
{
    gchar *full_jid;

    if (strstr (username, "@") == NULL) {
        full_jid = g_strconcat (username, "@gmail.com", NULL);
    } else {
        full_jid = g_strdup (username);
    }

    return full_jid;
}

static void
send_message (LmConnection *conn, Arguments *arguments)
{
    LmMessage     *m;
    LmMessageNode *node;

    m = lm_message_new (arguments->test_contact, LM_MESSAGE_TYPE_MESSAGE);

    lm_message_node_add_child (m->node, "body", arguments->test_message);
    lm_connection_send (conn, m, NULL);
    lm_message_unref (m);
}

static void
auth_cb (LmConnection *conn, gboolean result, gpointer user_data)
{
    Arguments *arguments = (Arguments *) user_data;

    if (result == FALSE) {
        g_print ("Failed to authenticate\n");
        g_main_loop_quit (main_loop);
        return;
    }

    g_print ("Connected to Google Talk as %s\n", arguments->username);

    send_message (conn, arguments);

    /* Close the connection */
    lm_connection_close (conn, NULL);

    /* Return to main () */
    g_main_loop_quit (main_loop);
}

static void
open_cb (LmConnection *conn, gboolean result, gpointer user_data)
{
    Arguments *arguments = (Arguments *) user_data;

    if (result == FALSE) {
        g_print ("Failed to open the connection, asynchronous result\n");
        g_main_loop_quit (main_loop);
        return;
    }

    g_print ("Connection open, attempting to authenticate\n");

    /* Connection is now open, time to authenticate */
    lm_connection_authenticate (conn, arguments->username, arguments->password,
                                "simple-gtalk",  auth_cb,
                                user_data, NULL, NULL);
}

int
main (int argc, char **argv)
{
    LmConnection *conn;
    gchar        *jid;
    LmSSL        *ssl;
    Arguments      arguments;
    GError       *error = NULL;

    if (argc != 5) {
        print_usage ();
        return EXIT_FAILURE;
    }

    arguments.username     = argv[1];
    arguments.password     = argv[2];
    arguments.test_contact = argv[3];
    arguments.test_message = argv[4];

    /* Create a new mainloop to handle the default context */
    main_loop = g_main_loop_new (NULL, FALSE);

    /* Create a new connection, don't set a specific host */
    conn = lm_connection_new (NULL);

    ssl = lm_ssl_new (NULL, NULL, NULL, NULL);

    /* Require the use of STARTTLS */
    lm_ssl_use_starttls (ssl, TRUE, TRUE);

    /* Use SSL for the connection */
    lm_connection_set_ssl (conn, ssl);

    /* conn holds a ref now so we can safely remove the initial one */
    lm_ssl_unref (ssl);

    jid = ensure_full_jid (argv[1]);

    /* Set the connection jid, this will implicitely lookup the host to  *
     * connect to.                                                       */
    lm_connection_set_jid (conn, jid);
    
    g_free (jid);

    if (!lm_connection_open (conn, open_cb, &arguments, NULL, &error)) {
        g_print ("Failed to open connection: %s\n", error->message);
        g_clear_error (&error);
        return EXIT_FAILURE;
    }

    /* Starting the mainloop with get Loudmouth to start process incoming *
     * events. This call will not return until g_main_loop_quit is called */
    g_main_loop_run (main_loop);

    return EXIT_SUCCESS;
}

