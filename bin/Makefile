
# Makefile to build the simple examples of FreeLing usage.
# If your installation paths are not /usr/local, call make with:
#
# make FREELINGDIR=/my/freeling/dir BOOSTINCLUDEDIR=/my/boost/include BOOSTLIBDIR=/my/boost/lib


# directories where freeling and boost are installed
FREELINGDIR=/usr/local
BOOSTINCLUDEDIR=/usr/local/include
BOOSTLIBDIR=/usr/local/lib

# paths for compilation commands below
PATHS=-L$(FREELINGDIR)/lib -I$(FREELINGDIR)/include -I$(FREELINGDIR)/include/foma -L$(BOOSTLIBDIR) -I$(BOOSTINCLUDEDIR)
LIBS=-lfreeling -lboost_system

all:	featuresGenerator

featuresGenerator: featuresGenerator.cpp
	g++ -std=c++11 -o featuresGenerator featuresGenerator.cpp ../pugixml-1.7/src/pugixml.hpp ../pugixml-1.7/src/pugixml.cpp $(PATHS) $(LIBS)

clean:
	rm -f featuresGenerator
