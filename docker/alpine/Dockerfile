FROM alpine:latest

ARG GIT_COMMIT
RUN apk update && apk upgrade && \
    apk add \
     sudo \
     xxd \
     vim \
     git \
     cmake \
     build-base \
     boost-dev \
     icu-dev && \
    adduser --disabled-password --shell /bin/ash user && \
    echo "user ALL=(ALL) NOPASSWD:ALL" >> /etc/sudoers

COPY --chmod=+x ../buildAndTestEa.sh /tmp/buildAndTestEa.sh
USER user
WORKDIR /home/user
RUN /tmp/buildAndTestEa.sh $GIT_COMMIT
CMD ["/bin/bash"]

