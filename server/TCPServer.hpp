#ifndef __TCPSERVER_HPP__
#define __TCPSERVER_HPP__

#include <list>
#include <memory>
#include <mutex>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <thread>

class TCPConnection;

class TCPServer {
public:
  TCPServer(in_port_t port);

  ~TCPServer();

  void listen();

  void wait();

  void addMsg(const std::string& msg);
  void addMsg(std::string&& msg);
  void addMsg(float msg);  

  void stop();
  
private:
  void prune();
  
  std::thread prune_;
  std::list<std::shared_ptr<TCPConnection>> connections_;
  int sock_desc_;
  struct sockaddr_in server_;

  bool done_;
  std::mutex mtx_;
};

#endif
