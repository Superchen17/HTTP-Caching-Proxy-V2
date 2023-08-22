FROM ubuntu:20.04

RUN apt update \
  && apt -y install cmake \
  && apt -y install build-essential

WORKDIR /usr/src/proxy

COPY . /usr/src/proxy/
