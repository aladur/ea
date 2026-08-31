# Query packages:
# zypper search -s <package-query>

FROM opensuse/leap:latest

ARG GIT_COMMIT
RUN zypper --non-interactive update && \
    zypper --non-interactive install --type pattern devel_basis && \
    zypper --non-interactive install \
    sudo \
    vim \
    git \
    cmake \
    gcc-c++-15 \
    libboost_program_options-devel \
    libboost_program_options1_86_0 \
    libicu-devel \
    icu && \
   useradd --create-home --shell /bin/bash user && \
   echo "user ALL=(ALL) NOPASSWD:ALL" >> /etc/sudoers

COPY --chmod=+x ../buildAndTestEa.sh /tmp/buildAndTestEa.sh
USER user
WORKDIR /home/user
RUN /tmp/buildAndTestEa.sh $GIT_COMMIT
CMD ["/bin/bash"]

