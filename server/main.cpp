#include "TCPServer.hpp"
#include "SoundMonitor.hpp"

#include <csignal>
#include <functional>
#include <iostream>
#include <signal.h>
#include <string.h>
#include <thread>
#include <chrono>

std::shared_ptr<TCPServer> gServer;
std::shared_ptr<SoundMonitor> gMonitor;

bool done;

void signalHandler(int signal) {
  std::cout << "sigHandler" << std::endl;
  done = true;
  gServer->terminate();
  gMonitor->stop();
  exit(signal);
}

int main(int argc, char *argv[]) {
  int portno;
  done = false;

  auto ret = signal(SIGPIPE, SIG_IGN);
  if (ret == SIG_ERR) return errno;
    
  if (argc < 2) {
    std::cout << "Usage: TCPServer <portno>" << std::endl;
    return 0;
  }
  else {
    portno = std::stoi(argv[1]);
  }

  gServer = std::make_shared<TCPServer>(portno);
  gMonitor = std::make_shared<SoundMonitor>();  

  gMonitor->configTavg(std::chrono::milliseconds{250});
  (void) std::signal(SIGINT, signalHandler);
  
  gServer->listen();

  std::thread tServer(&TCPServer::wait, &(*gServer));
  std::thread tMonitor(&SoundMonitor::monitor, &(*gMonitor));  

  int count = 0;
  while (not done) {
    std::string msg(std::to_string(gMonitor->spl()));
    gServer->addMsg(std::move(msg));
    std::this_thread::sleep_for(std::chrono::milliseconds(600));
  }

  tServer.join();
  tMonitor.join();
  
  std::cout << "exit" << std::endl;
  return 0;
  
}
