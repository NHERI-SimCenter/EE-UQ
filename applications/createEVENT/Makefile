CC=gcc
CXX=g++
RM=rm -f
CPPFLAGS= -I /usr/local/jansson/include -I ./nanoflann/include
LDFLAGS= 
LDLIBS=/usr/local/jansson/lib/libjansson.a

SRCS=OpenSeesPreprocessor.cpp OpenSeesPostprocessor.cpp

OBJS_PRE=OpenSeesPreprocessor.o
OBJS_POST=OpenSeesPostprocessor.o


debug: CPPFLAGS += -DDEBUG -g
debug: all

MultipleEvents: MultipleSimCenterEvents.o
	$(CXX) $(LDFLAGS) -o MultipleSimCenterEvents MultipleSimCenterEvents.o $(LDLIBS)

all: MultipleEvents

debug:

clean:
	$(RM) $(OBJS) *~ \#*

distclean: clean MultipleSimCenterEvents



