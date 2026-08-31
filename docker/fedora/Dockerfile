FROM fedora:latest

ARG GIT_COMMIT
RUN dnf upgrade -y && \
    dnf install -y \
     sudo \
     xxd \
     vim \
     git \
     make \
     cmake \
     gcc \
     gcc-g++ \
     boost-devel \
     libicu-devel && \
     useradd --create-home --shell /bin/bash user && \
     echo "user ALL=(ALL) NOPASSWD:ALL" >> /etc/sudoers

COPY --chmod=+x ../buildAndTestEa.sh /tmp/buildAndTestEa.sh
USER user
WORKDIR /home/user
RUN /tmp/buildAndTestEa.sh $GIT_COMMIT
CMD ["/bin/bash"]

