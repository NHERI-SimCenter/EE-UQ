CC=gcc
CXX=g++
RM=rm -f
CPPFLAGS= -I /usr/local/jansson/include -I ./nanoflann/include
LDFLAGS= 
LDLIBS=/usr/local/jansson/lib/libjansson.a


debug: CPPFLAGS += -DDEBUG -g
debug: all

MultipleEvents: MultipleSimCenterEvents.o
	$(CXX) $(LDFLAGS) -o MultipleSimCenterEvents MultipleSimCenterEvents.o $(LDLIBS)

MultiplePeer: MultiplePEER_Events.o
	$(CXX) $(LDFLAGS) -o MultiplePEER_Events MultiplePEER_Events.o $(LDLIBS)

all: MultipleEvents MultiplePeer

debug:

clean:
	$(RM) $(OBJS) *~ \#* *.o

distclean: clean 
	$(RM) MultipleSimCenterEvents MultiplePEER_Events



