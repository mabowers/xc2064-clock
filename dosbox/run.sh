#!/bin/bash

# get directory that this script exists in
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# import common docker functions
source $SCRIPT_DIR/../docker/common.sh

# make sure we're running inside the docker
check_inside_docker

# start dosbox, set machine to "svga_paradise", mount C drive, run AUTOEXEC.BAT
dosbox -machine svga_paradise -c "MOUNT C /workspace/dosbox/c" -c "C:" -c "AUTOEXEC.BAT"
