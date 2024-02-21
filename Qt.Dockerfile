FROM fedora:latest

# Install Qt6
RUN dnf install -y qt6-qtbase-devel qt6-qtserialport-devel ninja-build cmake gcc g++ 
