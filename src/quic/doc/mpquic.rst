Multipath QUIC Module Documentation
----------------------------

.. include:: replace.txt
.. highlight:: cpp

.. heading hierarchy:
   ------------- Chapter
   ************* Section (#.#)
   ============= Subsection (#.#.#)
   ############# Paragraph (no number)

This is an outline for the modified module ``src/quic``. Specifically, this 
document discribes the added feature of multipath functions over the original 
quic module in |ns3|. 

The introductory paragraph is for describing what this code is trying to
model.

Model Description
*****************

The source code for the modified module lives in the directory ``src/quic``.

Add here a basic description of what is being modified.

Design
======

The existing quic module has the basic functions to supporting single path transmission between client and server, which includes the basic transmission features, like quic headers and frames, establishment with handshake, reliability with acknowledgment, flow control and different congestion control algorithms. Based on these valuable functions, we aim to implement the multipath features, including the multiple path establishment, path management, realiable transmission with multipath acknowledgment, path scheduler, and congestion control algorithm. 

To realize this goal, we need to add new headers and frames that work for multipath, modify the original data flow and make it run over the path management layer, solve the reordering issue for different paths, and implement scheduling and congestion control algorithms that fit in the multipath scenarios.


Scope and Limitations
=====================

This module is currently able to support multiple paths connection between client and server. As a transport layer protocol, it is able to utilized bulk-send and other |ns3| supported applications. It is also able to build upon different topologies as long as the quic stack is installed on the sender and recever side.

It likes the other transport layer protocols in |ns3|, supporting most of the transmission and multipath features, including handshake negotiation and transport parameters , path setup and removal, multipath operation with multiple packet number spaces, path scheduling, and congestion control.

Since the IETF MPQUIC is still under discussion, our current version 1.2 tries to align the latest IETF draft (2023.03). Here are some minor different features that we will consider for our future work. IETF introduces a ``PATH_STATUS`` frame that informs the peer to send packets in the preference. Our implementation is able to have the path status locally, however, we do not support transmitting the path status between peers currently. In the path close stage, IETF includes a ``RETIRE_CONNECTION_ID`` frame to indicate to the receiving peer that the sender will not send any packets associated with the Connection ID used on that path anymore, which is not included in our current version. On the other hand, our implementation of MPQUIC currently only supports transmitting ``MP_ACK`` with the path given by the coming packets. An ``MP_ACK`` frame that can be returned via a different path is considered as our future work. Additionally, IETF indicates the packet protection with TLS and AEAD for connection ID, while the security features are not considered in our current stage. We will continue to improve our implementation to align with the latest IETF Draft.


References [Will update later]
==========

Add academic citations here, such as if you published a paper on this
model, or if readers should read a particular specification or other work.

Usage
*****

Helpers
=======

quic_helper is used to install the internet stack on the end-host. This should be utilized when you want to run a mpquic connection between end-hosts.

Attributes
==========

Here introduces some of the usable attributes. For the full attributes list, please check each .cc file.

1. QuicSocketBase::EnableMultipath enables the multipath features only when it is true.
2. QuicSocketBase::CcType supports different congestion control algorithms.
3. MpQuicScheduler::SchedulerType supports different scheduling algorithms.  


Output
======
It is able to trace the received bytes for each node, the receiver buffer size, the congestion window, and the transmission complete time. 

Examples
========
The sample scripts are located in scratch/

Take ``wns3-mpquic-two-path.cc`` as an example. line 18-26 shows the network topology. ThroughputModitor in line 52 traces the total received bytes of the node and the simulation time. 

ModifyLinkRate in line 66 is used to set the point-to-point bandwidth and delay along the time of the simulation. It is used in line 315 and 316.

line 98-115 are used for set different experiment scenarios from cmd. ``exp-wns3-two-path-scheduler.sh`` shows how to set them.

line 177-277 are used to create the network topology.

We use the MpquicBulkSendHelper to bind the application to the sender side in line 285 and use PacketSinkHelper to set the receiver side. 

Troubleshooting
===============

To report a bug, please open a GitHub Issue.
