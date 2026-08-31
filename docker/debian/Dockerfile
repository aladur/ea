FROM debian:stable-slim

ARG GIT_COMMIT
RUN apt-get -y update && apt-get -y install \
     sudo \
     xxd \
     vim \
     git \
     build-essential \
     cmake \
     libboost-program-options-dev \
     libicu-dev && \
    useradd --create-home --shell /bin/bash user && \
    echo "user ALL=(ALL) NOPASSWD:ALL" >> /etc/sudoers

COPY --chmod=+x ../buildAndTestEa.sh /tmp/buildAndTestEa.sh
USER user
WORKDIR /home/user
RUN /tmp/buildAndTestEa.sh $GIT_COMMIT
CMD ["/bin/bash"]

