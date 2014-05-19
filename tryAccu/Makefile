INCPATH = -I$(HOME)/Softwares/include
INCPATH += -I./include/
LIBPATH	=  -L$(HOME)/Softwares/lib -L$(HOME)/Softwares/lib64
CPPLDFLAGS  = -lcrypto -lntl
CXXFLAGS = -Wall
CXX = g++
tryAccu : tryAccu.cpp RSAAccumulator.cpp PrimeRep.cpp
	$(CXX) $(CXXFLAGS) $^ $(INCPATH) $(LIBPATH) $(CPPLDFLAGS) -o $@
