FROM ubuntu:latest

ARG MCUX_NAME="mcuxpressoide-11.9.0_2144.x86_64"
COPY ${MCUX_NAME}.deb.bin ${MCUX_NAME}.deb.bin
