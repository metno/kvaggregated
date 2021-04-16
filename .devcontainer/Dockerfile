FROM focal-kvbuild:latest

# Build dependencies
RUN apt-get update && \
    apt-get install -y xmlto libgmock-dev

RUN useradd -ms /bin/bash vscode
