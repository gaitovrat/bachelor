FROM fedora:latest

# Install Qt6
RUN dnf install -y qt6-qtbase-devel qt6-qtserialport-devel ninja-build cmake gcc g++ 

# Install latex
RUN dnf install -y texlive-scheme-basic

# Install MCUX
ARG MCUX_NAME="mcuxpressoide-11.9.0_2144.x86_64"

RUN dnf install -y dpkg
COPY ${MCUX_NAME}.deb.bin ${MCUX_NAME}.deb.bin
RUN sh ${MCUX_NAME}.deb.bin --noexec --target /mcux
RUN cd /mcux && dpkg-deb -x ${MCUX_NAME}.deb ./
