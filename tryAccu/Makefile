INCPATH = -I$(HOME)/Softwares/include
INCPATH += -I./include/
LIBPATH	=  -L$(HOME)/Softwares/lib -L$(HOME)/Softwares/lib64
CPPLDFLAGS  = -lcrypto -lntl
CXXFLAGS = -Wall -O3
CXX = g++
tryTwo : tryTwo.cpp RSAAccumulator.cpp PrimeRep.cpp
	$(CXX) $(CXXFLAGS) $^ $(INCPATH) $(LIBPATH) $(CPPLDFLAGS) -o $@
tryAccu : tryAccu.cpp RSAAccumulator.cpp PrimeRep.cpp
	$(CXX) $(CXXFLAGS) $^ $(INCPATH) $(LIBPATH) $(CPPLDFLAGS) -o $@
clean:
	rm -rf tryTwo tryAccu
