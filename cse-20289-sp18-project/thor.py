#!/usr/bin/env python3

import multiprocessing
import os
import requests
import sys
import time
import functools

from functools import reduce

# Globals

PROCESSES = 1
REQUESTS  = 1
VERBOSE   = False
URL       = None
ARGUMENTS = sys.argv[1:]

# Functions

def usage(status=0):
    print('''Usage: {} [-p PROCESSES -r REQUESTS -v] URL
    -h              Display help message
    -v              Display verbose output

    -p  PROCESSES   Number of processes to utilize (1)
    -r  REQUESTS    Number of requests per process (1)
    '''.format(os.path.basename(sys.argv[0])))
    sys.exit(status)

def do_request(pid):
    ''' Perform REQUESTS HTTP requests and return the average elapsed time. '''
    SUM=0
    for num in range(0, REQUESTS):
        START=time.time()
        r=requests.get(URL)
        TIME=time.time()-START
        SUM=SUM+TIME
        TIME=round(TIME, 10)
        if VERBOSE:
            print(r.text)
        print("Process: {}, Request: {}, Elapsed Time: {}".format(pid, num, TIME))
    AVERAGE=SUM/REQUESTS
    print("Process: {}, AVERAGE:  , Elapsed Time: {}".format(pid, round(AVERAGE, 2)))
    return AVERAGE

# Main execution

if __name__ == '__main__':
    # Parse command line arguments
    if not ARGUMENTS: #if there are no arguments
        usage(1)
    while ARGUMENTS:
        arg = ARGUMENTS.pop(0)
        if arg[0] == "-":
            if arg == "-p":
                PROCESSES=int(ARGUMENTS.pop(0))
            elif arg == "-r":
                REQUESTS=int(ARGUMENTS.pop(0))
            elif arg == "-v":
                VERBOSE=True
            elif arg == '-h':
                usage(0)
            else:
                usage(1)
        else:
            URL=arg;

    # Create pool of workers and perform requests
    pool=multiprocessing.Pool(PROCESSES)
    TIMES=pool.map(do_request, range(0, PROCESSES))
    #average all of the returned times from each process and round 2 dec places
    AVGTIME=reduce(lambda x, y: x + y, TIMES)/len(TIMES)
    print("TOTAL AVERAGE ELAPSED TIME: {}".format(round(AVGTIME, 10)))

# vim: set sts=4 sw=4 ts=8 expandtab ft=python:
