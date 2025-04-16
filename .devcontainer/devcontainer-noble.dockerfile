#ARG REGISTRY=registry.met.no/obs/kvalobs/kvbuild/staging/
ARG REGISTRY="registry.met.no/met/obsklim/bakkeobservasjoner/data-og-kvalitet/kvalobs/kvbuild/staging-noble/"
ARG BASE_IMAGE_TAG=latest


FROM ${REGISTRY}kvcpp-dev:${BASE_IMAGE_TAG}
ARG DEBIAN_FRONTEND='noninteractive'
ARG USER=vscode

# Build dependencies
RUN apt-get update && \
  apt-get install -y xmlto libgmock-dev language-pack-nb-base

RUN locale-gen en_US.UTF-8

RUN useradd -ms /bin/bash ${USER}

ENV USER=${USER}
