#!/bin/bash

DIRECTORY=`dirname $0`
SCRIPT_DIR=`pwd`/${DIRECTORY}

docker build -t avr-docker ${SCRIPT_DIR}/software/build_docker/
docker run -v${SCRIPT_DIR}/software/:/src avr-docker /bin/sh -c 'cd /src && make clean && make && make blob.hex'

