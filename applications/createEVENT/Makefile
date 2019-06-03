CC= gcc
CXX = g++
LINKER           = $(CXX)
#CXX=g++
RM=rm -f
CPPFLAGS= -I /usr/local/jansson/include -I ./nanoflann/include -std=c++11
LDFLAGS= -static-libgcc
LDLIBS=/usr/local/jansson/lib/libjansson.a

STOCHASTIC_INC = -I ./StochasticGroundMotionGenerator/external \
	-I ./StochasticGroundMotionGenerator/include \
	-I ./StochasticGroundMotionGenerator/external/nlohmann_json \
	-I ./StochasticGroundMotionGenerator/external/stochastic_loading/include

STOCHASTIC_LIBS = /opt/intel/mkl/lib/libmkl_intel_lp64.a /opt/intel/mkl/lib/libmkl_intel_thread.a /opt/intel/mkl/lib/libmkl_core.a /opt/intel/compilers_and_libraries/mac/lib/libiomp5.a /opt/intel/ipp/lib/libippcore.a /opt/intel/ipp/lib/libippvm.a /opt/intel/ipp/lib/libipps.a ../../../smelt/build/liblstoc.a

SRC = StochasticGroundMotionGenerator/src/command_parser.cc \
	StochasticGroundMotionGenerator/src/eq_generator.cc \
	StochasticGroundMotionGenerator/src/main.cc

OBJS = command_parser.o eq_generator.o main.o

all: MultipleEvents MultiplePeer SiteResponse StochasticGroundMotion

StochasticGroundMotion: $(SRC)
	$(CXX) $(LDFLAGS) -c StochasticGroundMotionGenerator/src/command_parser.cc $(CPPFLAGS) $(STOCHASTIC_INC)
	$(CXX) $(LDFLAGS) -c StochasticGroundMotionGenerator/src/eq_generator.cc $(CPPFLAGS) $(STOCHASTIC_INC)
	$(CXX) $(LDFLAGS) -c StochasticGroundMotionGenerator/src/main.cc $(STOCHASTIC_INC)
	$(LINKER) $(LDFLAGS) -o StochasticGroundMotion $(OBJS) $(LDLIBS) $(STOCHASTIC_LIBS)

MultipleEvents: MultipleSimCenterEvents.o
	$(CXX) $(LDFLAGS) -o MultipleSimCenterEvents MultipleSimCenterEvents.o $(LDLIBS)

MultiplePeer: MultiplePEER_Events.o
	$(CXX) $(LDFLAGS) -o MultiplePEER_Events MultiplePEER_Events.o $(LDLIBS)

SiteResponse: SiteResponse.o
	$(CXX) $(LDFLAGS) -o SiteResponse SiteResponse.o $(LDLIBS)

debug:

clean:
	$(RM) $(OBJS) *~ \#* *.o

distclean: clean 
	$(RM) MultipleSimCenterEvents MultiplePEER_Events StochasticGroundMotion SiteResponse
