#include <cstdlib>
#include <memory>

#include "proxy-server.h"
#include "logger.h"

int main(int argc, char** argv){
  Logger logger = Logger(AllDebugLevels::PRINT_ONLY);
  std::unique_ptr<TcpServer> server(new ProxyServer(NULL, "8080", 128, logger, 65536));
  server->run();
  return EXIT_SUCCESS;
}
