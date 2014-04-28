//
// server.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2013 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <ctime>
#include <iostream>
#include <string>
#include <map>
#include <boost/asio.hpp>
#include <boost/array.hpp>

using boost::asio::ip::tcp;

std::string make_daytime_string()
{
  using namespace std; // For time_t, time and ctime;
  time_t now = time(0);
  return ctime(&now);
}

class HttpInStream
{
public:
    HttpInStream():state(S_INIT),readedContentLength(0){}
    void add(char c)
    {
        if(isEnded())return;
        line.push_back(c);
        data.push_back(c);
        switch(state)
        {
            case S_INIT:
                if(c=='\r') state = S_R;
                break;
            case S_R:
                if(c=='\n')
                {
                    state = S_RN;   
                    handleLine();
                }
                else state = S_INIT;
                break;
            case S_RN:
                if(c=='\r') state = S_RNR;
                else state = S_INIT; 
                break;
            case S_RNR:
                if(c=='\n') state = S_RNRN;
                else state = S_INIT;
                break;
            case S_RNRN:
                content.push_back(c);
                readedContentLength++;
                break;
        }
    }
    bool isEnded()
    {
        return state==S_RNRN&&readedContentLength==getContentLength();
    }
    std::string toString()
    {
        return std::string(data.begin(),data.end());
    }
    std::string getMethod()
    {
        return method;
    }
    int getContentLength()
    {
        if(header.count("Content-Length")>0)
        {
            return atoi(header["Content-Length"].c_str());
        }
        else
        {
            return 0;
        }
    }
    void outputHeader()
    {
        for(HD_IT it = header.begin();it != header.end();++it)
        {
            std::cout << it->first << ": " << it->second << std::endl;
        }
        std::cout << "Contest:" << std::endl;
        std::cout << std::string(content.begin(),content.end()) << std::endl;
    }
private:
    typedef std::vector<char> DC;
    typedef std::vector<char>::iterator DC_IT;
    typedef std::map<std::string,std::string> HD;
    typedef std::map<std::string,std::string>::iterator HD_IT;
    void handleLine()
    {
        if(method=="")
        {
            DC_IT it = line.begin();
            while(it!=line.end()&&*it != ' ') ++it;
            method = std::string(line.begin(),it);
        }
        else
        {
            DC_IT it = line.begin();
            while(it!=line.end()&&*it != ':') ++it;
            if(it != line.end())
            {
                std::string key(line.begin(),it);
                it++;
                it++;
                DC_IT pit = it;
                while(it!=line.end()&&*it != '\r'&&*it!='\n') ++it;
                std::string value(pit,it);
                header[key] = value;
            }
        }
        line.clear();
    }
    DC data;
    DC line;
    DC content;
    std::string method,path,proxy;
    HD header;
    enum STATE{S_INIT,S_R,S_RN,S_RNR,S_RNRN};
    STATE state;
    int readedContentLength;
};
int main()
{
  try
  {
    boost::asio::io_service io_service;

    tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), 8088));

    for (;;)
    {
      tcp::socket socket(io_service);
      acceptor.accept(socket);

      boost::array<char, 128> buf;
      boost::system::error_code error;
      size_t len;
      HttpInStream his;
      while((len = socket.read_some(boost::asio::buffer(buf), error))>0)
      {
          for(int i=0;i<len;++i)
          {
            his.add(buf[i]);
            if(his.isEnded()) break;
          }
          if(his.isEnded()) break;
      }
      std::cout << his.toString();
      std::cout << his.getMethod()<<std::endl;
      std::cout << his.getContentLength()<<std::endl;
      his.outputHeader();

      if (error == boost::asio::error::eof)
      {
        // Connection closed cleanly by peer.
      }
      else if (error)
        throw boost::system::system_error(error); // Some other error.

      //if(his.isEnded())
      {
        boost::system::error_code ignored_error;
        std::string header = "HTTP/1.1 200 OK\r\n\r\n";
        boost::asio::write(socket, boost::asio::buffer(header), ignored_error);
        std::string message = make_daytime_string();
        boost::asio::write(socket, boost::asio::buffer(message), ignored_error);
      }
    }
  }
  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}

