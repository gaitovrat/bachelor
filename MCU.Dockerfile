FROM ubuntu:latest

ARG MCUX_NAME="mcuxpressoide-11.9.0_2144.x86_64"
RUN apt-get update && apt-get install -y git 
COPY ${MCUX_NAME}.deb.bin ${MCUX_NAME}.deb.bin
