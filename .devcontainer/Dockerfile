ARG REGISTRY=registry.met.no/obs/kvalobs/kvbuild/staging/
ARG BASE_IMAGE_TAG=latest

FROM ${REGISTRY}kvcpp-dev:${BASE_IMAGE_TAG}

#FROM focal-kvbuild:latest

# Build dependencies
RUN apt-get update && \
    apt-get install -y xmlto libgmock-dev

RUN useradd -ms /bin/bash vscode
