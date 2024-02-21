FROM ubuntu:latest

RUN apt-get update && apt-get install -y git 
COPY mcuxpressoide-11.9.0_2144.x86_64.deb.bin mcuxpressoide-11.9.0_2144.x86_64.deb.bin
