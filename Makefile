CXXFLAGS=-Og -g -Wall -Wextra -fPIC

all: simplenet

simplenet: simplenet.cc resolve.o probe.o \
	tcp.o udp.o dccp.o sctp.o udplite.o mptcp.o

clean:
	rm -f *.o simplenet

.PHONY: clean
