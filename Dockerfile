FROM ubuntu:20.04

RUN apt update \
  && apt -y install cmake \
  && apt -y install build-essential

# create a non-root user
RUN addgroup --gid 1000 http-proxy && \
  adduser --uid 1000 --gid 1000 --disabled-password --gecos "" http-proxy && \
  echo 'nonroot ALL=(ALL) NOPASSWD: ALL' >> /etc/sudoers

WORKDIR /home/http-proxy/app

COPY . /home/http-proxy/app
RUN chown -R http-proxy:http-proxy /home/http-proxy/app && \
  chmod -R 755 /home/http-proxy/app

RUN mkdir -p /var/log/http-proxy
RUN chown -R http-proxy:http-proxy /var/log/http-proxy && \
  chmod -R 755 /var/log/http-proxy

USER http-proxy

RUN rm -rf build && \
  mkdir build && \
  cd build && \
  cmake -DBUILD_TESTS=OFF .. && \
  make -j
