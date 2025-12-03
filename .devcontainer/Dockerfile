# ARG REGISTRY=registry.met.no/obs/kvalobs/kvbuild/staging/
ARG REGISTRY
ARG BASE_IMAGE_TAG=latest
#ARG BASE_IMAGE_TAG=kafka

FROM ${REGISTRY}kvcpp-dev:${BASE_IMAGE_TAG}

#FROM focal-kvbuild:latest

# Build dependencies
RUN apt-get update && \
    apt-get install -y \
    libsqlite3-dev \
    xmlto libgmock-dev language-pack-nb-base

RUN locale-gen en_US.UTF-8

RUN useradd -ms /bin/bash vscode
