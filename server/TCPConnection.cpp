#include "TCPConnection.hpp"

#include <iostream>
#include <unistd.h>
#include <errno.h>

static const size_t kKeepHistory = 10;

TCPConnection::TCPConnection(int sock) :
  client_sock_(sock),
  keep_history_(kKeepHistory),
  running_(true)
{
  thread_ = std::thread(&TCPConnection::send, this);
}

TCPConnection::~TCPConnection()
{
  if(thread_.joinable()) thread_.join();
}

void
TCPConnection::terminate()
{
  running_ = false;
  if (thread_.joinable()) thread_.join();
}

void
TCPConnection::addMsg(const std::string& msg)
{
  std::lock_guard<std::mutex> lock(mtx_);  
  if (msgs_.size() == keep_history_) {
    msgs_.pop();
  }
  msgs_.push(msg);
}

void
TCPConnection::addMsg(std::string&& msg)
{
  std::lock_guard<std::mutex> lock(mtx_);
  if (msgs_.size() == keep_history_) {
    msgs_.pop();
  }
  msgs_.push(msg);
}

void
TCPConnection::send()
{
  std::cout << client_sock_ << " start TCPConnection::send thread handler" << std::endl;
  while (running_) {
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    {
      std::lock_guard<std::mutex> lock(mtx_);
      if (not msgs_.empty()) {
        auto msg = msgs_.front();
//        std::cout << "send" << std::endl;
        ssize_t nbytes = write(client_sock_, msg.c_str(), msg.length());
//        std::cout << "nbytes: " << nbytes << std::endl;
        if (nbytes < 0) {
//          client_sock_ = -1;
          running_ = false;
        }
        msgs_.pop();
      }
    }
  }
  close(client_sock_);
  std::cout << "exit TCPConnection::send thread handler" << std::endl;  
}
