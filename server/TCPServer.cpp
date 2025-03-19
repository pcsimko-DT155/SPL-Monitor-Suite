#include "TCPServer.hpp"
#include "TCPConnection.hpp"

#include <algorithm>
#include <iostream>
#include <arpa/inet.h>
#include <system_error>
#include <stdexcept>
#include <unistd.h>

TCPServer::TCPServer(in_port_t port) : done_(false)
{
  // Create the socket
  sock_desc_ = socket(AF_INET, SOCK_STREAM, 0);

  // Convert POSIX error ECONNREFUSED to std::error_code and throw if
  // creation failed
  auto errCode = std::make_error_code(std::errc::connection_refused);
  if (sock_desc_ == -1) throw std::system_error(errCode, std::string("Could not create socket"));

  // Prepare socket
  server_.sin_family = AF_INET;
  server_.sin_addr.s_addr = INADDR_ANY;
  server_.sin_port = htons(port);

  // Bind
  if(bind(sock_desc_,(struct sockaddr*)&server_, sizeof(server_)) < 0) {
    throw std::system_error(errCode, std::string("bind failed")); // TODO: fix error code
  }

  prune_ = std::thread(&TCPServer::prune, this);
}

TCPServer::~TCPServer()
{
  if (prune_.joinable()) prune_.join();
  close(sock_desc_);
  std::cout << "~TCPServer" << std::endl;
}

void
TCPServer::terminate()
{
  std::cout << "terminate" << std::endl;
  for (auto& connection : connections_) {
    connection->terminate(); // block until all connections have terminated
  }
  done_ = true;
}
void
TCPServer::listen()
{
  constexpr int backlog = 3;
  ::listen(sock_desc_, backlog );
}

void
TCPServer::addMsg(const std::string& msg)
{
  std::lock_guard<std::mutex> lock(mtx_);
  for(auto& connection : connections_) {
    if (connection->connected()) {
      connection->addMsg(msg);
    }
  }
}

void
TCPServer::addMsg(std::string&& msg)
{
  std::lock_guard<std::mutex> lock(mtx_);  
  for(auto& connection : connections_) {
    if (connection->connected()) {
      connection->addMsg(msg);
    }
  }
}

void
TCPServer::addMsg(float msg)
{
  std::lock_guard<std::mutex> lock(mtx_);  
  for(auto& connection : connections_) {
    if (connection->connected()) {
      connection->addMsg(std::to_string(msg));
    }
  }
}

void
TCPServer::prune()
{
  while (not done_) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    connections_.remove_if([](std::shared_ptr<TCPConnection> cnx){ return not cnx->connected(); });
  }
}

/*
 * Wait for and accept incoming connections
 */
void
TCPServer::wait()
{
  std::cout << "Start TCPServer::wait thread handler" << std::endl;
  auto c = sizeof(struct sockaddr_in);
  int new_socket;
  sockaddr_in client;  
  while (not done_ and (new_socket = accept(sock_desc_, (struct sockaddr *)&client, (socklen_t*)&c))) {
    if ( new_socket < 0 ) {
      throw std::runtime_error(std::string("Accept failed")); // TODO: fix err code
    }
    std::cout << "Adding socket... " << new_socket << std::endl;

    {
      std::lock_guard<std::mutex> lock(mtx_);
      connections_.push_back(std::make_shared<TCPConnection>(new_socket));
    }
  }
  std::cout << "End TCPServer::wait thread handler" << std::endl;  
}
