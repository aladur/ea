# Search for archlinux packages:
# https://archlinux.org/packages/

FROM archlinux:base-devel

ARG GIT_COMMIT
RUN pacman -Syu --noconfirm && \
    pacman -S --noconfirm  -y \
     sudo \
     xxd \
     vim \
     git \
     which \
     make \
     cmake \
     gcc \
     boost \
     boost-libs \
     icu && \
    useradd --create-home --shell /bin/bash user && \
    echo "user ALL=(ALL) NOPASSWD:ALL" >> /etc/sudoers

COPY --chmod=+x ../buildAndTestEa.sh /tmp/buildAndTestEa.sh 
USER user
WORKDIR /home/user
RUN /tmp/buildAndTestEa.sh $GIT_COMMIT
CMD ["/bin/bash"]

