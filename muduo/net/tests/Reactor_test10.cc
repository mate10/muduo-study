#include <muduo/base/CurrentThread.h>
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>

#include <boost/bind.hpp>

#include <stdio.h>

using namespace muduo;
using namespace muduo::net;

class TestServer
{
 public:
  TestServer(EventLoop* loop,
             	 const InetAddress& listenAddr,
             	 int numThreads)
    : loop_(loop),
      server_(loop, listenAddr, "TestServer"),
      numThreads_(numThreads)
  {
    server_.setConnectionCallback(
        boost::bind(&TestServer::onConnection, this, _1));
    server_.setMessageCallback(
        boost::bind(&TestServer::onMessage, this, _1, _2, _3));
	
    server_.setThreadNum(numThreads);
  }

  void start()
  {
	  server_.start();
  }


 private:
  void onConnection(const TcpConnectionPtr& conn)
  {
    if (conn->connected())
    {
      printf("onConnection(): new connection [%s] from %s\n",
             conn->name().c_str(),
             conn->peerAddress().toIpPort().c_str());
    }
    else
    {
      printf("onConnection(): connection [%s] is down\n",
             conn->name().c_str());
    }
  }

  void onMessage(const TcpConnectionPtr& conn,
                   const char* data,
                   ssize_t len)
  {
    printf("onMessage(): received %zd bytes from connection [%s]\n",
           len, conn->name().c_str());
  }

  EventLoop* loop_;
  TcpServer server_;
  int numThreads_;
};


int main()
{
  printf("main(): pid = %d, tid = %d\n", getpid(), CurrentThread::tid());

  InetAddress listenAddr(8888);
  EventLoop mainLoop;

  TestServer server(&mainLoop, listenAddr, 4);
  server.start();

  mainLoop.loop();
}