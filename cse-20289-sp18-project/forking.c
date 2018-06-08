/* forking.c: Forking HTTP Server */

#include "spidey.h"

#include <errno.h>
#include <signal.h>
#include <string.h>

#include <unistd.h>

/**
 * Fork incoming HTTP requests to handle the concurrently.
 *
 * @param   sfd         Server socket file descriptor.
 * @return  Exit status of server (EXIT_SUCCESS).
 *
 * The parent should accept a request and then fork off and let the child
 * handle the request.
 **/

// You should use accept_request, handle_request and free_request
// You should use fork after you accept a new client.
//      The child should close the server socket, call handle_request, and then exit.
//      The parent should call free_request
int forking_server(int sfd) {
    /* Accept and handle HTTP request */
    while (true) {
        /* Accept request */
        Request * r = accept_request(sfd);
        if (!r) {
            log( "Unable to accept: %s\n", strerror(errno));
            free_request(r);
            return EXIT_FAILURE;
        }

        /* Ignore children */
        signal(SIGCHLD, SIG_IGN);

        /* Fork off child process to handle request */
        pid_t pid = fork();
        if (pid == 0) {	    // Child
            // The child should close the server socket, call handle_request, and then exit.
            close(sfd);
            HTTPStatus status = handle_request(r); // should I use exec here?
            free_request(r);
            // not sure this is completely correct
            if (status != 200) exit(1);
            else exit(0);
        } else if (pid < 0) { // failure to fork
            fprintf(stderr, "Unable to fork: %s\n", strerror(errno));
            free_request(r);
            return EXIT_FAILURE;
        } else { // parent
            // The parent should call free_request
            free_request(r);
        }
    }

    /* Close server socket */
    close(sfd);

    return EXIT_SUCCESS;
}
