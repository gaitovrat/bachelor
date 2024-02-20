FROM fedora:latest

# Install Qt6
RUN dnf install -y qt6-qtbase-devel qt6-qtserialport-devel ninja-build cmake gcc g++ 

# Install latex
RUN dnf install -y texlive-scheme-basic

# Install MCUX
ARG MCUX_NAME="mcuxpressoide-10.1.1_606.x86_64"
COPY ${MCUX_BIN}.deb.bin ${MCUX_BIN}.deb.bin
RUN sh ${MCUX_BIN}.deb.bin --noexec --target /mcux
RUN dnf install -y dpkg
RUN cd /mcux && dpkg-deb -x ${MCUX_BIN}.deb ./
