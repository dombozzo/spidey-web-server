#!/usr/bin/env python3

import os
import string

# Calls thor.py to test latency & throughput (bandwidth)
# latency = time delay between request & response
# throughput = # responses completed per time unit

# What is the average latency of spidey in single connection vs forking mode for: directory listings, static files, CGI scripts?
# What is the average throughput of spidey in single connection vs forking mode for: small static files (1KB), medium static files (1MB), and large static files (1GB)?

# format MD table and test
f = open("s_w_s.txt","w")
f.write("|{:^20}|{:^20}|{:^15}|{:^15}|{:^15}|\n".format("MODE", "PATH", "PROCESSES", "AVG TIME", "BPS"))
f.write("|{:^20}|{:^20}|{:^15}|{:^15}|{:^15}|\n".format(('-')*20, ("-")*20, ("-")*15,("-")*15, ("-")*15))

# TEST LATENCY
MODE = "Single"

nr = 7  # number of requests per process in thor

for np in [1,1,1,1]: #[1,2,4,6]: # number of processes in thor
    for path in ["/","/text/hackers.txt","/scripts/env.sh","/text/1kb.txt","/text/1mb.txt","/text/1gb.txt"]:
        #for trials in range(1,np):

        # run thor using os.popen
        out = os.popen('./thor.py -p {} -r {}  http://student02.cse.nd.edu:9968{}'.format(np,nr,path)).readlines()[-1].split()

        avgtime = out[-1]
        print(avgtime)
        if path == "/text/1kb.txt":
            bps = 8 * 1000 / float(avgtime)
        elif path == "/text/1mb.txt":
            bps = 8 * 1000 * 1000 / float(avgtime)
        elif path == "/text/1gb.txt":
            bps = 8 * 1000 * 1000 * 1000 / float(avgtime)
        else:
            bps = "-------"
        f.write("|{:^20}|{:^20}|{:^15}|{:^15}|{:^15}|\n".format(MODE, path , np,avgtime, bps))

