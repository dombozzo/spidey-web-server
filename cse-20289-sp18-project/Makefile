CC=		gcc
CFLAGS=		-g -gdwarf-2 -Wall -Werror -std=gnu99
LD=		gcc
LDFLAGS=	-L.
AR=		ar
ARFLAGS=	rcs
TARGETS=	spidey

all:		$(TARGETS)

spidey: forking.o request.o single.o socket.o spidey.o utils.o handler.o spidey.h
	@$(LD) $(LDFLAGS) -o $@ forking.o handler.o request.o single.o socket.o spidey.o utils.o
	@echo Linking spidey...

%.o: %.c spidey.h
	@$(CC) $(CFLAGS) -c -o $@ $<
	@echo Compiling $@...


clean:
	@echo Cleaning...
	@rm -f $(TARGETS) *.o *.log *.input

.SUFFIXES:
.PHONY:		all test benchmark clean
