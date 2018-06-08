/* spidey: Simple HTTP Server */

#include "spidey.h"

#include <errno.h>
#include <stdbool.h>
#include <string.h>

#include <unistd.h>

/* Global Variables */
char *Port	      = "9898";
char *MimeTypesPath   = "/etc/mime.types";
char *DefaultMimeType = "text/plain";
char *RootPath	      = "www";

/**
 * Display usage message and exit with specified status code.
 *
 * @param   progname    Program Name
 * @param   status      Exit status.
 */
void usage(const char *progname, int status) {
    fprintf(stderr, "Usage: %s [hcmMpr]\n", progname);
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "    -h            Display help message\n");
    fprintf(stderr, "    -c mode       Single or Forking mode\n");
    fprintf(stderr, "    -m path       Path to mimetypes file\n");
    fprintf(stderr, "    -M mimetype   Default mimetype\n");
    fprintf(stderr, "    -p port       Port to listen on\n");
    fprintf(stderr, "    -r path       Root directory\n");
    exit(status);
}

/**
 * Parse command-line options.
 *
 * @param   argc        Number of arguments.
 * @param   argv        Array of argument strings.
 * @param   mode        Pointer to ServerMode variable.
 * @return  true if parsing was successful, false if there was an error.
 *
 * This should set the mode, MimeTypesPath, DefaultMimeType, Port, and RootPath
 * if specified.
 */
bool parse_options(int argc, char *argv[], ServerMode *mode) {
    int argind=0;
    const char *PROGRAM_NAME=argv[argind++];
    while(argind<argc) {
        char *arg=argv[argind++];
        if(streq(arg, "-h"))
            usage(PROGRAM_NAME, 0);
        else if (streq(arg, "-c")) {
            arg=argv[argind++];
            if(streq(arg, "forking"))
                *mode=FORKING;
            else
                *mode=SINGLE;
        } else if (streq(arg, "-m")){
            arg=argv[argind++];
            MimeTypesPath=determine_mimetype(arg);
        } else if (streq(arg, "-M")){
            arg=argv[argind++];
            DefaultMimeType=arg;
        } else if (streq(arg, "-p")){
            arg=argv[argind++];
            Port=arg;
        } else if (streq(arg, "-r")){
            arg=argv[argind++];
            RootPath=arg;
        } else 
            usage(PROGRAM_NAME, 1);
    }

    return true;
}

/**
 * Parses command line options and starts appropriate server
 **/
int main(int argc, char *argv[]) {
    ServerMode mode;

    /* Parse command line options */
    if(!parse_options(argc, argv, &mode))
        return EXIT_FAILURE;

    /* Listen to server socket */
    int server_fd = socket_listen(Port);
    if( server_fd < 0 )
        return EXIT_FAILURE;

    /* Determine real RootPath */
    RootPath=realpath(RootPath, NULL);

    log("Listening on port %s", Port);
    debug("RootPath        = %s", RootPath);
    debug("MimeTypesPath   = %s", MimeTypesPath);
    debug("DefaultMimeType = %s", DefaultMimeType);
    debug("ConcurrencyMode = %s", mode == SINGLE ? "Single" : "Forking");

    /* Start either forking or single HTTP server */
    int status;
    if(mode == SINGLE)
        status=single_server(server_fd);
    else
        status=forking_server(server_fd); 
    
    free(RootPath);
    return status;   
}

/* vim: set expandtab sts=4 sw=4 ts=8 ft=c: */
