FROM focal-kvbuild:latest

RUN apt-get update && \
    apt-get install -y xmlto libgmock-dev

WORKDIR /src/

COPY src/ src/ 
COPY test/ test/ 
COPY mk/ mk/ 
COPY m4/ m4/ 
COPY doc/ doc/
COPY configure.ac Makefile.am kvAgregateDbInit.sh ./

RUN autoreconf -i && ./configure && make all check install
