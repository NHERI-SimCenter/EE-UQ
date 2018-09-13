CC=gcc-7.2
CXX=g++
RM=rm -f
CPPFLAGS= -I /usr/local/jansson/include -I ./nanoflann/include
LDFLAGS= 
LDLIBS=/usr/local/jansson/lib/libjansson.a

SRCS=OpenSeesInput.cpp
OBJS=OpenSeesInput.o

all: OpenSeesInput

debug: CPPFLAGS += -DDEBUG -g
debug: all


OpenSeesINput: $(OBJS)
	$(CXX) $(LDFLAGS) -o OpenSeesInput $(OBJS) $(LDLIBS)

clean:
	$(RM) *~ \#* *.o *.out

distclean: clean
	$(RM) OpenSeesInput