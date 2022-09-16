#!/bin/bash

# This script will build docker image and launch container "kegcip_builder"
# kegcip_builder is designed to have all tool to compile docker images and toolchain

set -e -o pipefail

KEGCIP_ROOTPATH=$(dirname $(dirname $(readlink -e $0)))

# Enable X server redirection (for "make xconfig" for instance)
xhost +

# Have a placeholder for bash history
touch /home/$USER/.docker_bash_history

# Build image each time
cd $(dirname $0)

docker build --network=host --build-arg=uid=$(id -u) -t kegcip_builder:latest .

if [ $? -ne 0 ]; then
    echo "Error during image build!"
    exit 1;
fi

echo "KEGCIP_ROOTPATH is ${KEGCIP_ROOTPATH}"

# Run container
docker run --rm -ti \
    -e DISPLAY=unix$DISPLAY \
    -v ${KEGCIP_ROOTPATH}:/workspaces/KegCip \
    -v /home/${USER}/.docker_bash_history:/home/docker_user/.bash_history \
    -v /tmp/.X11-unix:/tmp/.X11-unix:ro \
    -v $XDG_RUNTIME_DIR:$XDG_RUNTIME_DIR \
    -v /dev:/dev \
	-v /home/${USER}/.ssh:/home/docker_user/.ssh:ro \
    --net=host \
    --privileged \
    kegcip_builder:latest \
    /bin/bash
