FROM --platform=linux/amd64 fedora:latest

# Install build dependencies
RUN dnf install -y ninja-build \
    cmake \
    gcc \
    g++ \
    git

# Install Qt 6
RUN dnf install -y qt6-qtbase-devel qt6-qtserialport-devel

# Install OpenCV
RUN dnf install -y opencv-devel
