# spidey-web-server
(Spring 2018) Systems Programming final project. It consists of a basic HTTP server that supports directory listings, 
static files, and CGI scripts (spidey.c) and a basic HTTP client that will hammer a remote HTTP server by making 
multiple requests (thor.py).

This project was made to draw conclusions about the performance of a single server and a forking server, and a presentation
was given to convey findings.

# Project Members
Dominic J. Bozzo (dbozzo@nd.edu)
Fiona J. McCarter (fmccarte@nd.edu)
Matthew J. Malir (mmalir@nd.edu)

# Demonstration
Link to Presentation:
https://docs.google.com/presentation/d/1hzGn84mSa9tkoIc3cKO7OSxbVf7H1hc9cpDFC9eFJY8/edit?usp=sharing

# Errata
Summary of things that don't work (quite right).

While we were able to implement Spidey and Thor successfully, we had some trouble verifying their effectiveness. As we discussed in the presentation, we believe that several problems existed with our strategy to "hammer" Spidey. The results of our initial benchmarking script were confusing (to say the least). Single Spidey seemed to outperform forking Spidey with regard to latency and throughput. After some digging, we discovered that the problem was with our testing, not necessarily forking Spidey. The output of the Thor program, while useful, can be somewhat deceiving when using multiprocessing. We, however, believe that we were able to make sense the results to arrive at reasonable conclusions about our project.
