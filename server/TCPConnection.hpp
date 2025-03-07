#ifndef __TCPCONNECTION_HPP__
#define __TCPCONNECTION_HPP__

#include <functional>
#include <list>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>

class TCPConnection {
public:
  TCPConnection(int sock);
  
  ~TCPConnection();

  void addMsg(const std::string& msg);
  void addMsg(std::string&& msg);
  int socket() const { return client_sock_; }
  bool connected() const  { return client_sock_ > 0; }
  
  void terminate();
  
private:
  void send();
  
  std::queue<std::string> msgs_;
  int client_sock_;
  std::mutex mtx_;
  size_t keep_history_;
  std::thread thread_;
  bool running_;
};

#endif
