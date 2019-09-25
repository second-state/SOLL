#!/usr/bin/env bash
NAME=${1:+$1/}soll
INTERMEDIATES=()
IMAGES=()

set -e

function docker_build
{
    local FILENAME=$1; shift
    local NAME_TAG=${NAME}:$1; shift
    echo "Building docker image \"${NAME_TAG}\" from file \"${FILENAME}\"."

    ( set -x; docker build "$@" -f "${FILENAME}" -t "${NAME_TAG}" . )

    if [[ "${NAME_TAG}" == im-* ]]; then
        INTERMEDIATES+=( "${NAME_TAG}" )
    else
        IMAGES+=( "${NAME_TAG}" )
    fi
}

# Build all images.
docker_build Dockerfile.base        ubuntu-base
docker_build Dockerfile.build-gcc   ubuntu-gcc          \
    --build-arg "BASE=${NAME}:ubuntu-base"
docker_build Dockerfile.build-clang ubuntu-clang        \
    --build-arg "BASE=${NAME}:ubuntu-base"
docker_build Dockerfile.compile     ubuntu-compile      \
    --build-arg "BASE=${NAME}:ubuntu-base"
docker_build Dockerfile.test        ubuntu-test         \
    --build-arg "BASE=${NAME}:ubuntu-base"
docker_build Dockerfile.compile     im-gcc-compile      \
    --build-arg "BASE=${NAME}:ubuntu-gcc"
docker_build Dockerfile.test        latest              \
    --build-arg "BASE=${NAME}:im-gcc-compile"

# Remove intermediate images.
for NAME_TAG in "${INTERMEDIATES[@]}"; do
    ( set -x; docker rmi "${NAME_TAG}" )
done

# Push all images.
for NAME_TAG in "${IMAGES[@]}"; do
    ( set -x; docker push "${NAME_TAG}" )
done
