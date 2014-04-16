tryThreadPool:tryThreadPool.cpp
	g++ -I$(HOME)/Softwares/include -Ithreadpool/boost/ -L$(HOME)/Softwares/lib tryThreadPool.cpp -lboost_thread -lboost_system -o tryThreadPool
