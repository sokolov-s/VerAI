#!/bin/bash

DIR="${BASH_SOURCE%/*}"

DOCKER=docker
DOCKER_FILE=$DIR/docker/Dockerfile
DOCKER_IMAGE="verai/builder"

BUILD_CMD="cd /tmp/build; mkdir -p out && cd out; rm -rf ./*; cmake -DBUILD_TESTS=Yes .. && make"
SRC_ROOT=$(realpath $DIR/..)

hash $DOCKER 2>/dev/null ||
		{ echo >&2 "Could not find docker, aborting"; exit 1; }

usage() {
  SCR=$(basename "$0")
  echo "Usage: $SCR verai             Build VerAI project"
  echo "       $SCR build-img         Build docker image"
  echo "       $SCR console           Just run console in the docker container"
  echo "       $SCR --help            Print this help"
  exit 1
}

op=${1:-verai}
case "$op" in
	verai)
		echo "Building VerAI project:  "
		$DOCKER run -t -v $SRC_ROOT:/tmp/build -i $DOCKER_IMAGE bash -c "$BUILD_CMD"
		;;
	console)
		$DOCKER run -t -v $SRC_ROOT:/tmp/build -i $DOCKER_IMAGE bash -c \
			"/bin/bash"
		;;
	gitlab-ci)
		$DOCKER run -v $SRC_ROOT:/tmp/build -i $DOCKER_IMAGE bash -c "$BUILD_CMD; cd .. && chown -R 999:998 out/"
		;;
	build-img)
		echo "Building docker image from $DOCKER_FILE:  "
		$DOCKER build -t $DOCKER_IMAGE -f $DOCKER_FILE .
		;;
	*)
		usage
		;;
esac
