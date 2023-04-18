FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive 

RUN apt-get update && apt-get install -y \
    build-essential python3 python3-dev git libeigen3-dev python3-pandas python3-numpy python3-matplotlib font-manager

RUN mkdir ~/workspace && cd ~/workspace && \
    git clone https://github.com/ssjShirley/mpquic-ns3.git mpquic

WORKDIR /root/workspace/mpquic

RUN ./waf configure && \
    ./waf build

CMD ["/bin/bash", "/root/workspace/mpquic/wns3-2023-all-exp-figures.sh"] 