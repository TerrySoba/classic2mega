FROM ubuntu:18.04

RUN apt update && \
    apt upgrade && \
    apt install -y \
        avrdude \
        avr-libc \
        gcc-avr \
        make  && \
    apt clean

