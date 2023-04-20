
A Multipath Extension to the QUIC Module in ns-3
================================


## MPQUIC code base
This repository contains in the `src/quic` for a multipath extension to the QUIC protocol for ns-3.

[src/quic/Readme](https://github.com/ssjShirley/mpquic-ns3/blob/mpquic-1.2/src/quic/README.md) details the code structure and the implementation align the [IETF draft](https://datatracker.ietf.org/doc/draft-ietf-quic-multipath/04/).


<!-- The implementation is described in [this paper](https://arxiv.org/abs/1902.06121). -->

<!-- Please use the issue tracker for bugs/questions. -->


## Install

### Prerequisites ###

Our implementation requires basic environment settings for ns-3. For Ubuntu22.04 users, please use the following cmd to install the prerequisities. For users with different OS, please refer to [the ns-3 documentation](https://www.nsnam.org/documentation/) for more detailed instructions on how to set up your system to install ns-3.

```bash
apt-get update 
apt-get install -y build-essential python3 python3-dev git libeigen3-dev python3-pandas python3-numpy python3-matplotlib font-manager
```

### Downloading ####

Please download the repository by https, ssh, or other options.

```bash
git clone git@github.com:ssjShirley/mpquic-ns3.git mpquic
```

### Compilation ###

Please compile and build mpquic with the following cmd:

```bash
cd mpquic
./waf configure
./waf build
```

### View Experimental Results for WNS3 ###

Following jupyter files present the Figures 7 to 13 in the paper.

1. [`scalable-draw-figure7.ipynb`](https://github.com/ssjShirley/mpquic-ns3/blob/mpquic-1.2/wns3-draw-figures/scalable-draw-figure7.ipynb)
2. [`cwnd-draw-figure8.ipynb`](https://github.com/ssjShirley/mpquic-ns3/blob/mpquic-1.2/wns3-draw-figures/cwnd-draw-figure8.ipynb)
3. [`schedule-draw-figure9-10.ipynb`](https://github.com/ssjShirley/mpquic-ns3/blob/mpquic-1.2/wns3-draw-figures/schedule-draw-figure9-10.ipynb)
4. [`scheduler-cpr-two-path-figure11-13.ipynb`](https://github.com/ssjShirley/mpquic-ns3/blob/mpquic-1.2/wns3-draw-figures/scheduler-cpr-two-path-figure11-13.ipynb)


### Reproduce Experimental Results for WNS3 ###

Please use the following script to reproduce the experimental results in WNS3 paper. It might take few hours to generate all required data.
```bash
./wns3-2023-all-exp-figures.sh
```


#### Reproduce Results with Docker ####

For docker users, please load the image by
```bash
docker pull ssjshirley/mpquic-ns3:latest
```

Or, you can build the image with `Dockerfile` by
```bash
docker build -t ssjshirley/mpquic-ns3 .
```

To run the docker image and the data and figures will be stored in `$PWD/results`. (This may take few hours.)
```bash
docker run -it -v $PWD/results:/root/workspace/mpquic/results-wns3 ssjshirley/mpquic-ns3:latest
```


## Original QUIC code base
The ns-3 QUIC module can be found at https://github.com/signetlabdei/quic.

The implementation is described in [this paper](https://arxiv.org/abs/1902.06121).
