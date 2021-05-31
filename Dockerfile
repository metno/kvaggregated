ARG REGISTRY
ARG BASE_IMAGE_TAG=latest

FROM ${REGISTRY}kvcpp-dev:${BASE_IMAGE_TAG}

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


FROM ${REGISTRY}kvcpp-runtime:${BASE_IMAGE_TAG}

RUN apt-get update && apt-get install -y sqlite3

COPY --from=0 /usr/local/bin/kvAgregated /usr/local/bin/
COPY --from=0 /usr/local/bin/kvAgregateDbInit /usr/local/bin/

RUN mkdir -p -m777 /cache/db && kvAgregateDbInit /cache/db/database.sqlite && chmod 666 /cache/db/database.sqlite
VOLUME /cache

ENTRYPOINT ["kvAgregated"]
CMD ["--proxy-database-name", "/cache/db/database.sqlite", "--log-to-stdout"]
