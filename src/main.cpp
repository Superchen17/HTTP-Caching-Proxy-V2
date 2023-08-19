#include <cstdlib>
#include <memory>

#include "proxy-server.h"
#include "logger.h"
#include "fifo-cache.h"

int main(int argc, char** argv){
  Logger logger = Logger(AllDebugLevels::PRINT_ONLY);
  std::unique_ptr<Cache<Request, Response, Request::RequestHash> > cache(new FifoCache<Request, Response, Request::RequestHash>());
  std::unique_ptr<TcpServer> server(new ProxyServer(NULL, "8080", 128, logger, 65536, *cache));
  server->run();
  return EXIT_SUCCESS;
}
