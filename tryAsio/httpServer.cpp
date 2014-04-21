#include "HttpServer.h"
#include <boost/bind.hpp>
#include <boost/function.hpp>
void handle(int x,const HttpRequest *req,HttpResponse *resp)
{
    resp->write("<b>wgg</b>");
}
int main()
{
  try
  {
    HttpServer server(8088);
    boost::function<void(const HttpRequest *,HttpResponse*)> f2;
    int a=100;
    f2 = boost::bind(handle,a,_1,_2);
    server.run(f2);
  }
  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}

