Project - README
================

Members
-------

- Fiona J. McCarter (fmccarte@nd.edu)
- Dominic J. Bozzo (dbozzo@nd.edu)
- Matthew J. Malir (mmalir@nd.edu)

Demonstration
-------------

https://docs.google.com/presentation/d/1hzGn84mSa9tkoIc3cKO7OSxbVf7H1hc9cpDFC9eFJY8/edit?usp=sharing

Link to Presentation.

Errata
------

Summary of things that don't work (quite right).

While we were able to implement Spidey and Thor successfully, we had
some trouble verifying their effectiveness. As we discussed in the
presentation, we believe that several problems existed with our
strategy to "hammer" Spidey. The results of our initial benchmarking script
were confusing (to say the least). Single Spidey seemed to outperform
forking Spidey with regard to latency and throughput. After some digging, we
discovered that the problem was with our testing, not necessarily
forking Spidey. The output of the Thor program, while useful, can be somewhat
deceiving when using multiprocessing. We, however, believe that we were able
to make sense the results to arrive at reasonable conclusions about our
project. 

Contributions
-------------

Enumeration of the contributions of each group member.

Fiona McCarter:
- request.c
- socket.c
- spidey.c
- thor.py
- utils.c
- Makefile

Dom Bozzo:
- handler.c
- spidey.c
- benchmarking
- presentation

Matthew Malir:
- request.c (headers)
- forking.c
- single.c
- benchmarking
- presentation
