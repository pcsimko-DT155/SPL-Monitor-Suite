#include "TCPServer.hpp"
#include "SoundMonitor.hpp"

#include <csignal>
#include <functional>
#include <iostream>
#include <thread>
#include <chrono>

bool done;

namespace {
  std::function<void(int)> sigint_handler;
  void signalHandler(int signal) { sigint_handler(signal); }
} // namespace

int main(int argc, char *argv[]) {
  int portno = 0;
  done = false;
  
  auto ret = signal(SIGPIPE, SIG_IGN);
  if (ret == SIG_ERR) { return errno; }
    
  if (argc < 2) {
    std::cout << "Usage: TCPServer <portno>" << std::endl;
    return 0;
  }

  portno = std::stoi(argv[1]);
  auto server  = std::make_shared<TCPServer>(portno);  
  auto monitor = std::make_shared<SoundMonitor>();  
  monitor->configTavg(std::chrono::milliseconds{250});

  (void) std::signal(SIGINT, signalHandler);

  sigint_handler = [&](int signal) {
    done = true;
    server->stop();
    monitor->stop();
    exit(signal);
  };
  
  server->listen();

  std::thread tServer(&TCPServer::wait, &(*server));
  std::thread tMonitor(&SoundMonitor::monitor, &(*monitor));  

  int count = 0;
  while (not done) {
    server->addMsg(monitor->spl());
    std::this_thread::sleep_for(std::chrono::milliseconds(600));
  }

  tServer.join();
  tMonitor.join();
  
  std::cout << "exit" << std::endl;
  return 0;
  
}
