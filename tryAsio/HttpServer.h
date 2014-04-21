#include <iostream>
#include <string>
#include <map>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/function.hpp>
#include "HttpUtility.h"
using boost::asio::ip::tcp;
using std::string;
using HttpUtility::URLDecode;

class HttpRequest
{
public:
    HttpRequest():state(S_INIT),readedContentLength(0),isContentParsed(false){}
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
        for(KV_IT it = header.begin();it != header.end();++it)
        {
            std::cout << it->first << ": " << it->second << std::endl;
        }
        std::cout << "Contest:" << std::endl;
        std::cout << std::string(content.begin(),content.end()) << std::endl;
        if(!isContentParsed)
        {
            parseContent();
        }
        for(KV_IT it = form.begin();it != form.end();++it)
        {
            std::cout << it->first << ": " << it->second << std::endl;
        }
    }
    string getFormString(string key)
    {
        if(!isContentParsed) parseContent();
        return form[key];
    }
private:
    typedef std::vector<char> DC;
    typedef std::vector<char>::iterator DC_IT;
    typedef std::map<std::string,std::string> KV;
    typedef std::map<std::string,std::string>::iterator KV_IT;
    void parseContent()
    {
        std::string contentStr(content.begin(),content.end());
        int prePos=0,pos=0;
        while((pos=contentStr.find('&',prePos))!= std::string::npos)
        {
            form.insert(parseKV(contentStr.substr(prePos,pos-prePos)));
            prePos = pos+1;
        }
        if(prePos<contentStr.size())
        {
            form.insert(parseKV(contentStr.substr(prePos)));
        }
        isContentParsed = true;
    }
    std::pair<std::string,std::string> parseKV(string kvStr)
    {
        int segPos = kvStr.find('=');
        string key = URLDecode(kvStr.substr(0,segPos));
        string value = URLDecode(kvStr.substr(segPos+1));
        return std::make_pair(key,value);
    }
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
    bool isContentParsed;
    std::string method,path,proxy;
    KV header;
    KV form;
    enum STATE{S_INIT,S_R,S_RN,S_RNR,S_RNRN};
    STATE state;
    int readedContentLength;
};
class HttpResponse
{
public:
    HttpResponse(tcp::socket *psocket):isHeadWrited(false),psocket(psocket),head("HTTP/1.1 200 OK") {}
    void write(string data)
    {
        boost::system::error_code ignored_error;
        if(!isHeadWrited)
        {
            std::cout << "writed head\n";
            boost::asio::write(*psocket, boost::asio::buffer(head+"\r\n\r\n"), ignored_error);
            isHeadWrited = true;
        }
        boost::asio::write(*psocket, boost::asio::buffer(data), ignored_error);
    }
    ~HttpResponse()
    {
        if(!isHeadWrited)
        {
            std::cout << "writed head un\n";
            boost::system::error_code ignored_error;
            boost::asio::write(*psocket, boost::asio::buffer(head+"\r\n\r\n"), ignored_error);
            isHeadWrited = true;
        }
    }
private:
    HttpResponse(HttpResponse&){}
    string head;
    bool isHeadWrited;
    tcp::socket *psocket;
};
class HttpServer
{
public:
    HttpServer(int port):port(port){}
    void run(boost::function<void(const HttpRequest *,HttpResponse*)> f)
    {
        boost::asio::io_service io_service;
        tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), port));
        for (;;)
        {
          tcp::socket socket(io_service);
          acceptor.accept(socket);

          boost::array<char, 128> buf;
          boost::system::error_code error;
          size_t len;
          HttpRequest his;
          while((len = socket.read_some(boost::asio::buffer(buf), error))>0)
          {
              for(int i=0;i<len;++i)
              {
                his.add(buf[i]);
                if(his.isEnded()) break;
              }
              if(his.isEnded()) break;
          }

          if (error == boost::asio::error::eof)
          {
            // Connection closed cleanly by peer.
          }
          else if (error)
            throw boost::system::system_error(error); // Some other error.
          else
          {
              if(his.isEnded())
              {
                HttpResponse rep(&socket);
                std::cout << "here" << std::endl;
                f(&his,&rep);
                std::cout << "endhere" << std::endl;
                his.outputHeader();
              }
          }
        }
        /*
      catch (std::exception& e)
      {
        std::cerr << e.what() << std::endl;
      }
      */
    }
private:
    int port;
};


