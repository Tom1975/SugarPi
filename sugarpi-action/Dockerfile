# Download base image ubuntu 20.04
FROM ubuntu:23.10

# LABEL about the custom image
LABEL maintainer="thomas.guillemin@gmail.com"
LABEL version="0.1"
LABEL description="This is custom Docker Image for \
building SugarPi for Raspberry Pi."

# Disable Prompt During Packages Installation
ARG DEBIAN_FRONTEND=noninteractive


# Update Ubuntu Software repository
RUN apt-get update && apt-get -qq install wget && apt-get -qq install tar && apt-get -qq install xz-utils && apt-get -qq install git && apt-get -qq install make && apt-get -qq install build-essential
#RUN wget https://developer.arm.com/-/media/Files/downloads/gnu-a/9.2-2019.12/binrel/gcc-arm-9.2-2019.12-x86_64-aarch64-none-elf.tar.xz
RUN wget https://developer.arm.com/-/media/Files/downloads/gnu/13.2.rel1/binrel/arm-gnu-toolchain-13.2.rel1-x86_64-arm-none-eabi.tar.xz
RUN tar -Jxvf  arm-gnu-toolchain-13.2.rel1-x86_64-arm-none-eabi.tar.xz
ENV PATH /arm-gnu-toolchain-13.2.Rel1-x86_64-arm-none-eabi/bin:$PATH

COPY entrypoint_docker.sh /
#ensure entrypoint is executable (force flags update, because file flags may change if a commit is made under Windows)
RUN chmod +x /entrypoint_docker.sh

# Code file to execute when the docker container starts up ('entrypoint_docker.sh')
ENTRYPOINT ["/entrypoint_docker.sh"]


