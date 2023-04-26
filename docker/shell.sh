#!/bin/bash

# get directory that the script exists in
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# import common functions
source $SCRIPT_DIR/common.sh

# ensure we are running Docker in rootless mode
check_docker_rootless

# check if Docker image exists
if ! docker inspect "$DOCKER_IMAGE:$DOCKER_VERSION" &> /dev/null; then
    echo "The Docker image '$DOCKER_IMAGE:$DOCKER_VERSION' doesn't exist!"
    echo -e "Please run ${COLOR_BOLD}./create.sh${COLOR_NORMAL} to build it :)"
    exit -1
fi

# kill any existing instance of this container
docker stop $DOCKER_CONTAINER 2>/dev/null
docker rm   $DOCKER_CONTAINER 2>/dev/null

# allow non-network local connections to connect to X server,
# (this allows GUI apps to run inside Docker)
xhost +local:root >/dev/null

# start shell
docker run -it --rm \
	--hostname $DOCKER_HOSTNAME \
	--name $DOCKER_CONTAINER \
	-e DISPLAY=$DISPLAY \
	-v $SCRIPT_DIR/..:/workspace \
	-v /tmp/.X11-unix:/tmp/.X11-unix \
	$DOCKER_IMAGE:$DOCKER_VERSION \
	bash -c "cd /workspace; bash --rcfile /workspace/docker/.bashrc_docker"
