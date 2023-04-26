#!/bin/bash
echo " "
echo "╔═════════════════════════════╗"
echo "║  Docker image build script  ║"
echo "╚═════════════════════════════╝"
#NOTE: To clean everything: docker system prune -a

# get directory that script exists in
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
echo -e "\nScript directory: $SCRIPT_DIR\n"

# source common script
source $SCRIPT_DIR/common.sh

# ensure we are running Docker in rootless mode
check_docker_rootless

# navigate to script directory, preserve original directory
pushd ${SCRIPT_DIR} > /dev/null

# confirm user wants to rebuild Docker image
echo -e "${COLOR_BOLD}You are about to rebuild the Docker image from scratch.${COLOR_NORMAL}"
echo -e "${COLOR_BOLD}This may take a little while.${COLOR_NORMAL}"
echo ""
if ! confirm ; then
  exit -1
fi
echo " "

# build the Docker image
echo -e "\nBuilding Docker image..."
docker build --rm --file Dockerfile --tag ${DOCKER_IMAGE}:${DOCKER_VERSION} --build-arg UID=$UID .

# restore original directory
popd > /dev/null
