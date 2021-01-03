FROM ubuntu:focal
LABEL maintainer="micha.hoiting@gmail.com"
ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update -y
RUN apt-get install -y curl python3 python3-distutils
RUN python3 -c "$(curl -fsSL https://raw.githubusercontent.com/platformio/platformio/master/scripts/get-platformio.py)"
RUN ln -s ~/.platformio/penv/bin/platformio /usr/local/bin/platformio; \
    ln -s ~/.platformio/penv/bin/pio /usr/local/bin/pio; \
    ln -s ~/.platformio/penv/bin/piodebuggdb /usr/local/bin/piodebuggdb
RUN pio platform install --with-package tool-dfuutil gd32v
RUN pio platform update
RUN apt-get clean && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*
RUN cd /tmp && \
    mkdir src && \
    printf "[env:sipeed-longan-nano]\nplatform=gd32v\nboard=sipeed-longan-nano\nframework=gd32vf103-sdk\n" > platformio.ini && \
    printf "int main(void) {return 0;}\n" > src/main.c && \
    pio run && \
    rm -rf /tmp/*
RUN pio system info
RUN pio platform show gd32v
