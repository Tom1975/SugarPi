#Download base image ubuntu 20.04
FROM ubuntu:20.04

# LABEL about the custom image
LABEL maintainer="thomas.guillemin@gmail.com"
LABEL version="0.1"
LABEL description="This is custom Docker Image for \
building SugarPi for Raspberry Pi."

# Disable Prompt During Packages Installation
ARG DEBIAN_FRONTEND=noninteractive

# Update Ubuntu Software repository
RUN apt-get update && apt-get -qq install wget && apt-get -qq install tar && apt-get -qq install xz-utils && apt-get -qq install git
RUN wget https://developer.arm.com/-/media/Files/downloads/gnu-a/9.2-2019.12/binrel/gcc-arm-9.2-2019.12-x86_64-aarch64-none-elf.tar.xz
RUN	tar -Jxvf  gcc-arm-9.2-2019.12-x86_64-aarch64-none-elf.tar.xz
RUN export PATH=$(pwd)/gcc-arm-9.2-2019.12-x86_64-aarch64-none-elf/bin:$PATH

COPY entrypoint_docker.sh /

#ensure entrypoint is executable (force flags update, because file flags may change if a commit is made under Windows)
RUN chmod +x /entrypoint_docker.sh
RUN chmod +x /build_all.sh
RUN chmod +x /build_circle.sh
RUN chmod +x /build_pi3.sh
RUN chmod +x /build_pi4.sh

