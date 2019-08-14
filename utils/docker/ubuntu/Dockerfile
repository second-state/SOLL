# Default ARG will build a clean environment for testing.
ARG BASE=ubuntu:18.04
FROM ${BASE}

RUN apt update \
    && apt install -y \
        git \
        software-properties-common \
        wget

RUN wget --quiet -O - https://deb.nodesource.com/gpgkey/nodesource.gpg.key | apt-key add - \
    && apt-add-repository 'deb https://deb.nodesource.com/node_10.x xenial main'

RUN apt update \
    && apt install -y \
        lld-8 \
        llvm-8-dev \
        nodejs
RUN rm -rf /var/lib/apt/lists/*

# This line requires ewasm-testbench in the current context.
#COPY index.js /usr/local/bin/soll-ewasm-env.js
