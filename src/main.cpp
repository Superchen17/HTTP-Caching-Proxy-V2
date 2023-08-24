#include <cstdlib>
#include <memory>
#include <iostream>

#include "proxy-server.h"
#include "logger.h"
#include "fifo-cache.h"

int main(int argc, char** argv){

  // ./http-proxy <server-port> <logger-file-path>
  int serverPort = 8080;
  std::string logFilePath = "/var/log/http-proxy/log.txt";

  if(argc >= 2){
    serverPort = std::stoi(argv[1]);
    if(serverPort < 0){
      std::cout << "port number must be >= 0" <<std::endl;
      return EXIT_FAILURE;
    }
  }

  if(argc >= 3){
    logFilePath = std::string(argv[2]);
  }

  Logger logger = Logger(Logger::AllDebugLevels::PRINT_AND_PERSIST, logFilePath);
  std::unique_ptr<Cache<Request, Response, Request::RequestHash> > cache(new FifoCache<Request, Response, Request::RequestHash>());
  std::unique_ptr<TcpServer> server(new ProxyServer(NULL, std::to_string(serverPort).c_str(), 128, logger, 65536, *cache));
  server->run();
  return EXIT_SUCCESS;
}
