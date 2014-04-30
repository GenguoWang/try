#include <string>
#include "HttpUtility.h"
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <vector>
#include <map>
using boost::asio::ip::tcp;

class HttpClient
{
private:
    typedef std::map<std::string,std::string> HD;
    typedef HD::iterator HD_IT;
    HD head;
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
public:
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
        std::cout << head["Host"] << std::endl;
        std::string formStr = getFormStr(kv);
        std::cout << head["Host"] << std::endl;
        //head["Content-Length"] = std::string((int)(formStr.size()));
        std::cout << formStr<< std::endl;
        return send(addr.host,addr.port,"POST "+addr.path+" HTTP/1.1\r\n"+headStr()+"\r\n"+formStr);
    }
    static std::string send(std::string host,std::string port,std::string message)
    {
        std::cout << message << std::endl;
        boost::asio::io_service io_service;
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
        return std::string(data.begin(),data.end());
    }
};
