INCPATH=-I$(HOME)/Softwares/include
LIBPATH=-L$(HOME)/Softwares/lib
CXX=g++
CXXLDFLAGS= -lboost_system
CXXFLAG= -Wall

tcpServer:tcpServer.cpp
	$(CXX) $(INCPATH) -o $@ $^ $(LIBPATH) $(CXXLDFLAGS)
tcpSyncServer:tcpSyncServer.cpp
	$(CXX) $(INCPATH) -o $@ $^ $(LIBPATH) $(CXXLDFLAGS)
tcpClient:tcpClient.cpp
	$(CXX) $(INCPATH) -o $@ $^ $(LIBPATH) $(CXXLDFLAGS)
httpServer:httpServer.cpp HttpServer.h
	$(CXX) $(INCPATH) -o $@ httpServer.cpp $(LIBPATH) $(CXXLDFLAGS)
httpClient:httpClient.cpp HttpClient.h
	$(CXX) $(CXXFLAG) $(INCPATH) -o $@ httpClient.cpp $(LIBPATH) $(CXXLDFLAGS)
