
Align with the IETF Draft
================================

`mpquic-1.2` implements most of the basic features mentioned in [IETF draft](https://datatracker.ietf.org/doc/draft-ietf-quic-multipath/04/).

![alt text](https://github.com/ssjShirley/mpquic-ns3/blob/mpquic-1.2/results-wns3/mpquic_uml_diagram.png)


## Features ##

1. [Handshake Negotiation and Transport Parameter](#handshake-negotiation-and-transport-parameter)
2. [Path Setup and Removal](#path-setup-and-removal)
3. [Multipath Operation with Multiple Packet Number Spaces](#multipath-operation-with-multiple-packet-numer-spaces)
4. [Implementation Considerations](#implementation-consiterations)
5. [New Frames](#new-frames)

### Handshake Negotiation and Transport Parameter ###
1. New transport parameter `enable_multipath` => m_enableMultipath in quic-sochet-base.h

### Path Setup and Removal ###
1. Path Initiation => MpQuicPathManager::InitialSubflow0 in mp-quic-path-manager.h
2. Path State Management => PATH_STATUS frame (x)
3. Path Close => QuicSocketBase::SendConnectionClosePacket in quic-sochet-base.h
4. Pate States => SubflowStates_t in mp-quic-subflow.h; QuicSocketBase::SendPathChallenge and QuicSocketBase::SendPathResponse in quic-sochet-base.h

### Multipath Operation with Multiple Packet Number Spaces ###
1. Sending Acknowledgements => QuicSocketBase::SendAck and QuicSocketBase::OnReceivedAckFrame in quic-sochet-base.h
2. Packet Protection => TLS and AEAD (x)

### Implementation Considerations ###
1. Number Spaces => m_receivedPacketNumbers in mp-quic-subflow.h
2. Congestion Control => mp-quic-congestion-ops.h
3. Computing Path RTT => m_tcb->m_lastRtt in mp-quic-subflow.h
4. Packet Scheduling => mp-quic-scheduler.h
5. Retransmissions => QuicSocketBase::DoRetransmit in quic-sochet-base.h
6. Handling different PMTU sizes => (x)
7. Keep Alive => (x)
8. Connection ID Changes and NAT Rebindings => (x)

### New Frames ###
1. ACK_MP Frame => MP_ACK in quic-subheader.h
2. PATH_ABANDON Frame => PATH_ABANDON in quic-subheader.h
3. PATH_CHALLENGE => PATH_CHALLENGE in quic-subheader.h
4. PATH_RESPONSE => PATH_RESPONSE in quic-subheader.h
5. PATH_STATUS Frame => (x)


### Edited Files ####

New File Created:
1. mp-quic-congestion-ops.cc
1. mp-quic-congestion-ops.h
2. mp-quic-path-manager.cc
2. mp-quic-path-manager.h
3. mp-quic-scheduler.cc
3. mp-quic-scheduler.h
4. mp-quic-subflow.cc
4. mp-quic-subflow.h

Quic File Modified:
1. quic-socket-base.cc
1. quic-socket-base.h
2. quic-l4-protocol.cc
2. quic-l4-protocol.h
4. quic-subheader.cc
4. quic-subheader.h
3. quic-header.h

Application:
1. mpquic-bulk-send-application.cc
1. mpquic-bulk-send-application.h
