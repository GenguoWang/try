#ifndef HTTPCLIENT_H_
#define HTTPCLIENT_H_

#include <string>
#include "HttpUtility.h"
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <vector>
#include <map>
#include <sstream>
#include <istream>
#include <ostream>
#include <boost/bind.hpp>
#include <boost/function.hpp>
using boost::asio::ip::tcp;

class AsyncHttpRequest
{
public:
  AsyncHttpRequest(boost::asio::io_service& io_service,
      const std::string& server, const std::string &port,const std::string message,boost::function<void(const std::string &)> f)
    : resolver_(io_service),
      socket_(io_service),
      request_(message),
      callback(f)
  {
    // Start an asynchronous resolve to translate the server and service names
    // into a list of endpoints.
    tcp::resolver::query query(server, port);
    resolver_.async_resolve(query,
        boost::bind(&AsyncHttpRequest::handle_resolve, this,
          boost::asio::placeholders::error,
          boost::asio::placeholders::iterator));
  }

private:
  void handle_resolve(const boost::system::error_code& err,
      tcp::resolver::iterator endpoint_iterator)
  {
    if (!err)
    {
      // Attempt a connection to each endpoint in the list until we
      // successfully establish a connection.
      boost::asio::async_connect(socket_, endpoint_iterator,
          boost::bind(&AsyncHttpRequest::handle_connect, this,
            boost::asio::placeholders::error));
    }
    else
    {
      throw boost::system::system_error(err); 
    }
  }

  void handle_connect(const boost::system::error_code& err)
  {
    if (!err)
    {
      // The connection was successful. Send the request.
      boost::asio::async_write(socket_, boost::asio::buffer(request_),
          boost::bind(&AsyncHttpRequest::handle_write_request, this,
            boost::asio::placeholders::error));
    }
    else
    {
      throw boost::system::system_error(err); 
    }
  }

  void handle_write_request(const boost::system::error_code& err)
  {
    if (!err)
    {
      // Read the response status line. The response_ streambuf will
      // automatically grow to accommodate the entire line. The growth may be
      // limited by passing a maximum size to the streambuf constructor.
      boost::asio::async_read_until(socket_, response_, "\r\n",
          boost::bind(&AsyncHttpRequest::handle_read_status_line, this,
            boost::asio::placeholders::error));
    }
    else
    {
      throw boost::system::system_error(err); 
    }
  }

  void handle_read_status_line(const boost::system::error_code& err)
  {
    if (!err)
    {
      // Check that response is OK.
      std::istream response_stream(&response_);
      std::string http_version;
      response_stream >> http_version;
      unsigned int status_code;
      response_stream >> status_code;
      std::string status_message;
      std::getline(response_stream, status_message);
      if (!response_stream || http_version.substr(0, 5) != "HTTP/")
      {
        throw std::runtime_error("Invalid response"); 
      }
      if (status_code != 200)
      {
        throw std::runtime_error("Response returned with status code"+ status_code);
      }

      // Read the response headers, which are terminated by a blank line.
      boost::asio::async_read_until(socket_, response_, "\r\n\r\n",
          boost::bind(&AsyncHttpRequest::handle_read_headers, this,
            boost::asio::placeholders::error));
    }
    else
    {
      throw boost::system::system_error(err); 
    }
  }

  void handle_read_headers(const boost::system::error_code& err)
  {
    if (!err)
    {
      // Process the response headers.
      /*
      std::istream response_stream(&response_);
      std::string header;
      while (std::getline(response_stream, header) && header != "\r")
        std::cout << header << "\n";
      std::cout << "\n";

      // Write whatever content we already have to output.
      if (response_.size() > 0)
        std::cout << &response_;
      */

      // Start reading remaining data until EOF.
      boost::asio::async_read(socket_, response_,
          boost::asio::transfer_at_least(1),
          boost::bind(&AsyncHttpRequest::handle_read_content, this,
            boost::asio::placeholders::error));
    }
    else
    {
      throw boost::system::system_error(err); 
    }
  }

  void handle_read_content(const boost::system::error_code& err)
  {
    std::ostringstream ostr;
    if (!err)
    {
      // Write all of the data that has been read so far.
      ostr << &response_;

      // Continue reading remaining data until EOF.
      boost::asio::async_read(socket_, response_,
          boost::asio::transfer_at_least(1),
          boost::bind(&AsyncHttpRequest::handle_read_content, this,
            boost::asio::placeholders::error));
    }
    else if(err == boost::asio::error::eof)
    {
        callback(ostr.str());
    }
    else
    {
      throw boost::system::system_error(err); 
    }
  }

  tcp::resolver resolver_;
  tcp::socket socket_;
  std::string request_;
  boost::asio::streambuf response_;
  boost::function<void(const std::string &)> callback;
};

class HttpClient
{
private:
    typedef std::map<std::string,std::string> HD;
    typedef HD::iterator HD_IT;
    HD head;
    std::vector<AsyncHttpRequest*> asyncRequests;
    boost::asio::io_service io_service;
    void initHead()
    {
        head["Connection"]= "keep-alive";
        head["Accept"]= "*/*";
        head["User-Agent"]= "Wget/1.12 (linux-gnu)";
    }
    std::string headStr()
    {
        std::string str;
        for(HD_IT it=head.begin();it != head.end();++it)
        {
            str += it->first + ": " + it->second + "\r\n";
        }
        return str;
    }
    void release_requests()
    {
        for(int i=0;i<asyncRequests.size();++i)
        {
            delete asyncRequests[i];
        }
        asyncRequests.clear();
    }
public:
    std::string resHeadStr;
    std::string resBodyStr;
    struct ReqAddr
    {
        std::string host;
        std::string path;
        std::string port;
    };
    HttpClient()
    {
        initHead();
    }
    ~HttpClient()
    {
        release_requests();
    }
    static void parseUrl(std::string url,ReqAddr & addr)
    {
        if(url.substr(0,7)=="http://")
        {
            url = url.substr(7);
            int pos = url.find('/');
            std::string hostAndPort;
            if(pos!=std::string::npos)
            {
                hostAndPort = url.substr(0,pos);   
                addr.path = url.substr(pos);
            }
            else
            {
                hostAndPort = url.substr(0);   
                addr.path = "/";
            }
            pos = hostAndPort.find(':');
            if(pos!=std::string::npos)
            {
                addr.host = hostAndPort.substr(0,pos);
                addr.port = hostAndPort.substr(pos+1);
            }
            else
            {
                addr.host = hostAndPort;
                addr.port = "80";
            }
        }
    }
    std::string getFormStr(HD &form)
    {
        std::string str;
        bool isFirst = true;
        for(HD_IT it = form.begin();it != form.end();++it)
        {
            if(isFirst)
            {
                str += HttpUtility::URLEncode(it->first) +"="+HttpUtility::URLEncode(it->second);
                isFirst = false; 
            }
            else
            {
                str += "&"+HttpUtility::URLEncode(it->first) +"="+HttpUtility::URLEncode(it->second);
            }
        }
        return str;
    }
    std::string get(std::string url)
    {
        ReqAddr addr;
        parseUrl(url,addr);
        head["Host"] = addr.host+":"+addr.port;
        return send(addr.host,addr.port,"GET "+addr.path+" HTTP/1.1\r\n"+headStr()+"\r\n");
    }
    std::string post(std::string url,std::map<std::string,std::string> kv)
    {
        ReqAddr addr;
        parseUrl(url,addr);
        head["Host"] = addr.host+":"+addr.port;
        std::string formStr = getFormStr(kv);
        std::ostringstream oStream;
        oStream << formStr.size();
        head["Content-Length"] = oStream.str();
        return send(addr.host,addr.port,"POST "+addr.path+" HTTP/1.1\r\n"+headStr()+"\r\n"+formStr);
    }
    void asyncGet(std::string url,boost::function<void(const std::string &)> callback)
    {
        ReqAddr addr;
        parseUrl(url,addr);
        AsyncHttpRequest *request = new AsyncHttpRequest(io_service, addr.host, addr.port,"GET "+addr.path+" HTTP/1.1\r\n"+headStr()+"\r\n",callback);
        asyncRequests.push_back(request);
    }
    void asyncPost(std::string url,std::map<std::string,std::string> kv, boost::function<void(const std::string &)> callback)
    {
        ReqAddr addr;
        parseUrl(url,addr);
        head["Host"] = addr.host+":"+addr.port;
        std::string formStr = getFormStr(kv);
        std::ostringstream oStream;
        oStream << formStr.size();
        head["Content-Length"] = oStream.str();
        AsyncHttpRequest *request = new AsyncHttpRequest(io_service, addr.host, addr.port,"POST "+addr.path+" HTTP/1.1\r\n"+headStr()+"\r\n"+formStr,callback);
        asyncRequests.push_back(request);
    }
    void run()
    {
        io_service.run();
        //release_requests();
    }
    std::string send(std::string host,std::string port,std::string message)
    {
        tcp::resolver resolver(io_service);
        tcp::resolver::query query(host, port);
        tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

        tcp::socket socket(io_service);
        boost::asio::connect(socket, endpoint_iterator);

        boost::system::error_code error;
        boost::asio::write(socket, boost::asio::buffer(message), error);
        std::vector<char> data;
        for (;;)
        {
            boost::array<char, 102400> buf;
            size_t len = socket.read_some(boost::asio::buffer(buf), error);
            for(int i=0;i<len;++i) 
            {
                data.push_back(buf[i]);   
            }
            if (error == boost::asio::error::eof)
                break; // Connection closed cleanly by peer.
            else if (error)
                throw boost::system::system_error(error); // Some other error.
        }
        std::string res = std::string(data.begin(),data.end());
        int pos = res.find("\r\n\r\n");
        resHeadStr = res.substr(0,pos);
        resBodyStr = res.substr(pos+4);
        return resBodyStr;
    }
};

#endif
