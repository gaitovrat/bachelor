FROM --platform=linux/amd64 ubuntu:latest

ENV TOOLCHAIN_PATH=/usr/local/mcuxpressoide-11.9.0_2144/ide/plugins/com.nxp.mcuxpresso.tools.linux_11.9.0.202307121226/tools/bin

RUN apt-get update && apt-get upgrade -y
RUN apt-get install -y build-essential cmake ninja-build gcc-arm-none-eabi

COPY ./mcuxpressoide-11.9.0_2144.x86_64.deb.bin /tmp/mcuxpressoide-11.9.0_2144.x86_64.deb.bin
RUN chmod +x /tmp/mcuxpressoide-11.9.0_2144.x86_64.deb.bin
RUN /tmp/mcuxpressoide-11.9.0_2144.x86_64.deb.bin --target /mcux -- --acceptLicense
