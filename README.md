# HTTP / HTTPS Proxy Server V2
A multithreaded HTTP/HTTPS proxy server. 

## Features
- HTTP Version
  - supported `HTTP/1.1`
- Request
  - maximum size of 65536 bytes
  - supported methods: `GET`, `POST`, `CONNECT`
  - supported headers: `Accept-Encoding`
- Response
  - supported headers: `Content-Length`, `Date`, `Last-Modified`, `Etag`, `Expires`, `Transfer-Encoding`, `Cache-Control`
  - supported transfer encodings: `chunked`
- Caching
  - response caching with FIFO replacement policy
  - supported methods: `GET`
  - supported status codes: `200 OK`
  - supported `Cache-Control` entries for cacheability and validation: `private`, `public` `no-store`, `no-cache`, `must-revalidate`, `max-age`

## Requirements
- OS: Linux / MacOS
- C++ verison: C++20
- tools: cmake3.7+
- Packages: pthread, GoogleTest, gcov, lcov

## Run It Locally
The process runs on port `8080`
```bash
mkdir build
cd build
cmake ..
make -j
./unit_tests # optional, unit testing
./http-proxy 
```

## Production Deployment
The docker container exposes port `8080`
```bash
docker-compose build
docker-compose up -d
```

## Improvements Upon V1
- refactored codebase for better readability and extensibility
- added GoogleTest unit testing infrastructure
- migrated build infrastructure from `make` to `cmake`
- used `C++20` for easier support for multithreading, locking and memory management, and access to high-level libraries
- improved error handling and memory management

## Known Issues
- running in docker results in network degradation, `CONNECT` requests are intermittently unreachable
