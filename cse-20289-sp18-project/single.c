/* single.c: Single User HTTP Server */

#include "spidey.h"

#include <errno.h>
#include <string.h>

#include <unistd.h>

/**
 * Handle one HTTP request at a time.
 *
 * @param   sfd         Server socket file descriptor.
 * @return  Exit status of server (EXIT_SUCCESS).
 **/

 // You should use accept_request, handle_request and free_request
int single_server(int sfd) {
    /* Accept and handle HTTP request */
    while (true) {
        /* Accept request */
        Request * r = accept_request(sfd);
        if (!r) {
            fprintf(stderr, "Unable to accept: %s\n", strerror(errno));
            free_request(r);
            return EXIT_FAILURE;
        }

        /* Handle request */
        handle_request(r);

        /* Free request */
        free_request(r);
    }

    /* Close server socket */
    close(sfd);

    return EXIT_SUCCESS;
}
