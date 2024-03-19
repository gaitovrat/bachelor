FROM --platform=linux/amd64 ubuntu:latest

RUN apt-get update && apt-get upgrade -y
RUN apt-get install -y build-essential cmake ninja-build gcc-arm-none-eabi
ENV TOOLCHAIN_PATH="/usr/bin"
