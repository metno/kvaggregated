ARG REGISTRY
ARG BASE_IMAGE_TAG=latest

FROM ${REGISTRY}kvcpp-dev:${BASE_IMAGE_TAG}

RUN apt-get update && \
  apt-get install -y xmlto libgmock-dev


VOLUME /src
VOLUME /build
WORKDIR /src

COPY src/ src/ 
COPY test/ test/ 
COPY mk/ mk/ 
COPY m4/ m4/ 
COPY doc/ doc/
COPY configure.ac Makefile.am kvAgregateDbInit.sh ./

WORKDIR /build
#../configure --prefix=/usr --localstatedir=/var --mandir=/usr/share/man --with-boost-libdir=/usr/lib/x86_64-linux-gnu/ LDFLAGS=-Wl,-z,defs

# RUN --mount=type=cache,target=/build cd /src/ && autoreconf -if && cd /build && \
#     /src/configure CFLAGS=-g && make all && make install


RUN cd /src/ && autoreconf -if && cd /build && \
  /src/configure CFLAGS=-g && make all && make install


FROM ${REGISTRY}kvcpp-runtime:${BASE_IMAGE_TAG}
ARG kvuser=kvalobs
ARG kvuserid=5010

RUN apt-get update && apt-get install -y sqlite3 gdb

#Create a runtime user for kvalobs
RUN addgroup --gid $kvuserid $kvuser && \
  adduser --uid $kvuserid  --gid $kvuserid --disabled-password --disabled-login --gecos '' $kvuser

RUN mkdir -p /etc/kvalobs && chown ${kvuser}:${kvuser}  /etc/kvalobs
RUN mkdir -p /var/log/kvalobs && chown ${kvuser}:${kvuser}  /var/log/kvalobs
RUN mkdir -p /var/lib/kvalobs/run && chown ${kvuser}:${kvuser} /var/lib/kvalobs/run

COPY --from=0 /usr/local/bin/kvAgregated /usr/local/bin/
COPY --from=0 /usr/local/bin/kvAgregateDbInit /usr/local/bin/


RUN mkdir /cores && chmod 777 /cores
VOLUME /cores

RUN mkdir -p -m777 /cache/db && kvAgregateDbInit /cache/db/database.sqlite && chmod 666 /cache/db/database.sqlite


VOLUME /cache
VOLUME /var/log/kvalobs

USER ${kvuser}:${kvuser}

ENTRYPOINT ["kvAgregated"]
#CMD ["--proxy-database-name", "/cache/db/database.sqlite", "--log-to-stdout"]
CMD ["--proxy-database-name", "/cache/db/database.sqlite"]
