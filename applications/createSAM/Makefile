CC=gcc
CXX=g++
RM=rm -f
CPPFLAGS= -I /usr/local/jansson/include -I ./nanoflann/include -I ../common -std=c++11
LDFLAGS= 
LDLIBS=/usr/local/jansson/lib/libjansson.a

SRCS=OpenSeesInput.c MDOF_BuildingModelToSAM.cpp
OBJS=OpenSeesInput.o MDOF_BuildingModelToSAM.o

all: OpenSeesInput MDOF

debug: CPPFLAGS += -DDEBUG -g
debug: all


OpenSeesInput: $(OBJS)
	$(CXX) $(LDFLAGS) -o OpenSeesInput  OpenSeesInput.o $(LDLIBS)

MDOF: $(OBJS)
	$(CXX) -c ../common/Units.cpp $(CPPFLAGS) -o Units.o
	$(CXX) $(LDFLAGS) -o MDOF_BuildingModel MDOF_BuildingModelToSAM.o Units.o $(LDLIBS)

clean:
	$(RM) *~ \#* *.o *.out

distclean: clean
	$(RM) OpenSeesInput MDOF_BuildingModel