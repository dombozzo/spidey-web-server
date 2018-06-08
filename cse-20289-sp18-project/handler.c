/* handler.c: HTTP Request Handlers */

#include "spidey.h"

#include <errno.h>
#include <limits.h>
#include <string.h>

#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

/* Internal Declarations */
HTTPStatus handle_browse_request(Request *request);
HTTPStatus handle_file_request(Request *request);
HTTPStatus handle_cgi_request(Request *request);
HTTPStatus handle_error(Request *request, HTTPStatus status);

/**
 * Handle HTTP Request.
 *
 * @param   r           HTTP Request structure
 * @return  Status of the HTTP request.
 *
 * This parses a request, determines the request path, determines the request
 * type, and then dispatches to the appropriate handler type.
 *
 * On error, handle_error should be used with an appropriate HTTP status code.
 **/
HTTPStatus  handle_request(Request *r) {
    HTTPStatus result;

    /* Parse request */
    if(parse_request(r)<0){
        log("Parse request failed");
        return handle_error(r, HTTP_STATUS_BAD_REQUEST);
    }

    /* Determine request path */
    r->path=determine_request_path(r->uri);
    if(!r->path){
        log("Determine_request_path failed");
        return handle_error(r, HTTP_STATUS_NOT_FOUND);
    }
    debug("HTTP REQUEST PATH: %s", r->path);

    /* Dispatch to appropriate request handler type based on file type */
    struct stat s;
    if(stat(r->path, &s) <0 ){
        log("Unable to stat: %s\n", strerror(errno));
        return handle_error(r, HTTP_STATUS_NOT_FOUND);
    }
    if(S_ISREG(s.st_mode))
        if(access(r->path, X_OK)==0)
            result=handle_cgi_request(r);
        else
            result=handle_file_request(r);
    else if(S_ISDIR(s.st_mode))
        result=handle_browse_request(r);

    log("HTTP REQUEST STATUS: %s", http_status_string(result));
    return handle_error(r, result);
}

/**
 * Handle browse request.
 *
 * @param   r           HTTP Request structure.
 * @return  Status of the HTTP browse request.
 *
 * This lists the contents of a directory in HTML.
 *
 * If the path cannot be opened or scanned as a directory, then handle error
 * with HTTP_STATUS_NOT_FOUND.
 **/
HTTPStatus  handle_browse_request(Request *r) {
    struct dirent **entries;
    int n;

    /* Open a directory for reading or scanning */
    n=scandir(r->path, &entries, NULL, alphasort);
    if(n < 0){
      fprintf(stderr, "Unable to scan directory: %s\n", strerror(errno));
      return HTTP_STATUS_NOT_FOUND;
    }

    /* Write HTTP Header with OK Status and text/html Content-Type */
    fprintf(r->file, "HTTP/1.0 200 OK\r\n");
    fprintf(r->file, "Content-Type: text/html\r\n");
    fprintf(r->file, "\r\n");
    
    debug("handle_browse_request: header");

    /* For each entry in directory, emit HTML list item */
    fprintf(r->file, "<ul>\n");

    for(int i=0; i<n; i++){
        if(!streq(entries[i]->d_name, ".")){
          fprintf(r->file, "<li><a href=\"%s/%s\">%s</a></li>\n", streq(r->uri, "/") ? "" : r->uri, entries[i]->d_name, entries[i]->d_name);
        }
        free(entries[i]);
      }
      fprintf(r->file, "</ul>\n");
      fflush(r->file);
    
    debug("handle_browse_request: listing");

    /* Flush socket, return OK */
    free(entries);

    return HTTP_STATUS_OK;
}

/**
 * Handle file request.
 *
 * @param   r           HTTP Request structure.
 * @return  Status of the HTTP file request.
 *
 * This opens and streams the contents of the specified file to the socket.
 *
 * If the path cannot be opened for reading, then handle error with
 * HTTP_STATUS_NOT_FOUND.
 **/
HTTPStatus  handle_file_request(Request *r) {
    FILE *fs;
    char buffer[BUFSIZ];
    char *mimetype = NULL;
    size_t nread=0;

    /* Open file for reading */
    fs = fopen(r->path, "r");
    if(fs==NULL){
        log("Unable to fopen");
        return HTTP_STATUS_NOT_FOUND;
    }

    /* Determine mimetype */
    mimetype=determine_mimetype(r->path);

    /* Write HTTP Headers with OK status and determined Content-Type */
    fprintf(r->file, "HTTP/1.0 200 OK\r\n");
    fprintf(r->file, "Content-Type: %s\r\n", mimetype);
    fprintf(r->file, "\r\n");

    /* Read from file and write to socket in chunks */
    while((nread = fread(buffer, sizeof(char), BUFSIZ, fs)) > 0){
     if(fwrite(buffer, sizeof(char), nread, r->file) != nread){
       fprintf(stderr, "fwrite failed %s\n", strerror(errno));
       goto fail;
     }
    }
    fflush(r->file);

    /* Close file, flush socket, deallocate mimetype, return OK */
    fclose(fs);
    free(mimetype);
    return HTTP_STATUS_OK;

fail:
    /* Close file, free mimetype, return INTERNAL_SERVER_ERROR */
    fclose(fs);
    free(mimetype);
    return HTTP_STATUS_INTERNAL_SERVER_ERROR;
}

/**
 * Handle CGI request
 *
 * @param   r           HTTP Request structure.
 * @return  Status of the HTTP file request.
 *
 * This popens and streams the results of the specified executables to the
 * socket.
 *
 * If the path cannot be popened, then handle error with
 * HTTP_STATUS_INTERNAL_SERVER_ERROR.
 **/
HTTPStatus handle_cgi_request(Request *r) {
    FILE *pfs;
    char buffer[BUFSIZ];
    struct header *header = r->headers;

    /* Export CGI environment variables from request structure:
     * http://en.wikipedia.org/wiki/Common_Gateway_Interface */
    setenv("DOCUMENT_ROOT", RootPath, 1);
    setenv("QUERY_STRING", r->query, 1);
    setenv("REMOTE_ADDR", r->host, 1);
    setenv("REMOTE_PORT", r->port, 1);
    setenv("REQUEST_METHOD", r->method, 1);
    setenv("REQUEST_URI", r->uri, 1);
    setenv("SCRIPT_FILENAME", r->path, 1);
    setenv("SERVER_PORT", Port, 1);


    /* Export CGI environment variables from request headers */
    while(header != NULL){
      if(streq(header->name, "Accept")){
        setenv("HTTP_ACCEPT", header->value, 1);
      }
      else if(streq(header->name, "Host")){
          if(setenv("HTTP_HOST", header->value, 1)) debug("setenv: %s", strerror(errno));
      }
      else if(streq(header->name, "Accept-Language")){
        setenv("HTTP_ACCEPT_LANGUAGE", header->value, 1);
      }
      else if(streq(header->name, "Accept-Encoding")){
        setenv("HTTP_ACCEPT_ENCODING", header->value, 1);
      }
      else if(streq(header->name, "Connection")){
        setenv("HTTP_CONNECTION", header->value, 1);
      }
      else if(streq(header->name, "User-Agent")){
        setenv("HTTP_USER_AGENT", header->value, 1);
      }
      header = header->next;
    }


    /* POpen CGI Script */
    pfs = popen(r->path, "r");
    while(fgets(buffer, BUFSIZ, pfs)){
      fputs(buffer, r->file);
    }

    /* Copy data from popen to socket */
    while(fgets(buffer, BUFSIZ, pfs))
        fputs(buffer, r->file);

    /* Close popen, flush socket, return OK */
    pclose(pfs);
    fflush(r->file);
    return HTTP_STATUS_OK;
}

/**
 * Handle displaying error page
 *
 * @param   r           HTTP Request structure.
 * @return  Status of the HTTP error request.
 *
 * This writes an HTTP status error code and then generates an HTML message to
 * notify the user of the error.
 **/
HTTPStatus  handle_error(Request *r, HTTPStatus status) {
    if (status == HTTP_STATUS_OK)
        return status;
    const char *status_string = http_status_string(status);


    /* Write HTTP Header */
    fprintf(r->file, "HTTP/1.0 %s\r\n", status_string);
    fprintf(r->file, "Content-Type: text/html\r\n");
    fprintf(r->file, "\r\n");

    /* Write HTML Description of Error*/
    fprintf(r->file, "<html>\r\n");
    fprintf(r->file, "<h1>\r\n");
    fprintf(r->file, "%s\r\n", status_string);
    fprintf(r->file, "</h1>\r\n");
    fprintf(r->file, "<h2>\r\n");
    fprintf(r->file, "WHY DOESNT THIS WORK\r\n");  // LOVELY
    fprintf(r->file, "</h2>\r\n");
    fprintf(r->file, "</html>\r\n");

    fflush(r->file);

    /* Return specified status */
    return status;
}

/* vim: set expandtab sts=4 sw=4 ts=8 ft=c: */
