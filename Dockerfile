FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive 

RUN apt-get update && apt-get install -y \
    build-essential python3 python3-dev git libeigen3-dev

RUN mkdir ~/workspace && cd ~/workspace && \
    git clone https://github.com/ssjShirley/mpquic-ns3.git mpquic

WORKDIR /root/workspace/mpquic

RUN ./waf configure && \
    ./waf build

CMD ["/bin/bash", "/root/workspace/mpquic/wns3-2023-all-exp-figures.sh"]