CC=gcc
CXX=g++
RM=rm -f
CPPFLAGS= -I /usr/local/jansson/include -I ./nanoflann/include -I ../common -std=c++11 
LDFLAGS= 
LDLIBS=/usr/local/jansson/lib/libjansson.a

SRCS=OpenSeesPreprocessor.cpp OpenSeesPostprocessor.cpp

OBJS_PRE=OpenSeesPreprocessor.o
OBJS_POST=OpenSeesPostprocessor.o

all: OpenSeesPreprocessor mainPostprocessor

debug: CPPFLAGS += -DDEBUG -g
debug: all

OpenSeesPreprocessor.o: OpenSeesPreprocessor.cpp OpenSeesPreprocessor.h

mainPostprocessor.o: OpesnSeesPostprocessor.h  OpenSeesPostprocessor.h 

OpenSeesPreprocessor: $(OBJS_PRE)
	$(CXX) -c ../common/Units.cpp $(CPPFLAGS) -o Units.o
	$(CXX) $(LDFLAGS) -o OpenSeesPreprocessor $(OBJS_PRE) Units.o $(LDLIBS)

mainPostprocessor: $(OBJS_POST)
	$(CXX) $(LDFLAGS) -o OpenSeesPostprocessor $(OBJS_POST) $(LDLIBS)

clean:
	$(RM) *~ \#* *.o *.out

distclean: clean
	$(RM) OpenSeesPostprocessor OpenSeesPreprocessor


