#!/bin/bash

# Docker names and version
DOCKER_HOSTNAME=xc2064-linux-docker
DOCKER_IMAGE=${DOCKER_HOSTNAME}-image
DOCKER_CONTAINER=${DOCKER_HOSTNAME}-container
DOCKER_VERSION=0.1

# function to ask user to confirm before continuing
confirm() {
  read -r -p "${1:-Are you sure you want to do this? [y/N]} " answer
  case "$answer" in
    [yY][eE][sS]|[yY])
      true ;;
    *)
      false ;;
  esac
  }

# function to verify that Docker is running in rootless mode
check_docker_rootless() {
  if !(docker context inspect | grep -q "rootless"); then
    echo "ERROR: Please run Docker in rootless mode."
    echo "For more info see: https://docs.docker.com/engine/security/rootless/"
	exit -1
  fi
}

# function to verify that we're running inside the Docker
check_inside_docker() {
  if [ "$HOSTNAME" != "$DOCKER_HOSTNAME" ]; then
    echo "ERROR: Please run inside the Docker '$DOCKER_HOSTNAME'"
    exit -1
  fi
}

# bash color defines
COLOR_BOLD="\e[1m"
COLOR_NORMAL="\e[0m"
