INCPATH=-I$(HOME)/Softwares/include
LIBPATH=-L$(HOME)/Softwares/lib
CXX=g++
CXXLDFLAGS= -lboost_system

tcpServer:tcpServer.cpp
	$(CXX) $(INCPATH) -o $@ $^ $(LIBPATH) $(CXXLDFLAGS)
tcpSyncServer:tcpSyncServer.cpp
	$(CXX) $(INCPATH) -o $@ $^ $(LIBPATH) $(CXXLDFLAGS)
tcpClient:tcpClient.cpp
	$(CXX) $(INCPATH) -o $@ $^ $(LIBPATH) $(CXXLDFLAGS)
httpServer:httpServer.cpp HttpServer.h
	$(CXX) $(INCPATH) -o $@ httpServer.cpp $(LIBPATH) $(CXXLDFLAGS)
