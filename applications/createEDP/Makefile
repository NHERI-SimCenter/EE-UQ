CC=gcc
CXX=g++
RM=rm -f
CPPFLAGS= -I /usr/local/jansson/include -I ./nanoflann/include
LDFLAGS= 
LDLIBS=/usr/local/jansson/lib/libjansson.a


all: StandardEarthquakeEDP UserDefinedEDP


StandardEarthquakeEDP.o : StandardEarthquakeEDP.cpp

StandardEarthquakeEDP: StandardEarthquakeEDP.o
	$(CXX) $(LDFLAGS) -o StandardEarthquakeEDP StandardEarthquakeEDP.o $(LDLIBS)


UserDefinedEDP.o : UserDefinedEDP.cpp

UserDefinedEDP: UserDefinedEDP.o
	$(CXX) $(LDFLAGS) -o UserDefinedEDP UserDefinedEDP.o $(LDLIBS)

clean:
	$(RM) *.o *~

distclean: clean
	$(RM) UserDefinedEDP StandardEarthquakeEDP


