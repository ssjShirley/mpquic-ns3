
The Network Simulator for multipath QUIC
================================

## Code Structure:

New File Created:

1. mp-quic-typedefs
2. quic-scheduler


Quic File Modified:

1. quic-socket-base
2. quic-l4-protocol
3. quic-header
4. quic-subheader

## New File Created

1. mp-quic-typedefs

2. quic-scheduler


## Quic File Modified

1. quic-socket-base

new valuables:  

    //for multipath  
    std::map <Ipv4Address, uint8_t> m_addrIdPair;  
    std::vector <Ptr<MpQuicSubFlow>> m_subflows;  

    //for mobility
    bool vnResponse = 0; 
    static std::vector<double> ue_sinr;
    static std::vector<double> ue_Bmin;

    //for scheduler
    Ptr<QuicScheduler> m_scheduler;

new methods:

    //for multipath
    uint8_t LookUpByAddr (Address &address);
    
    //for mobility
    static void NotifyConnectionEstablishedEnb (std::string context, uint64_t imsi, uint16_t cellid, uint16_t rnti);
    static void NotifyHandoverEndOkEnb (std::string context, uint64_t imsi, uint16_t cellid, uint16_t rnti);
    static void ReportUeSinr (std::string context, uint16_t cellId, uint16_t rnti, double sinrLinear, uint8_t componentCarrierId);
    
    //for mm congestion control
    int FindMinRttPath();
    double GetOliaApha(int pathId);
    
    //for scheduler
    void CreateScheduler ();

modified methods:

    //connection initial
    QuicSocketBase::Connect (const Address & address)
        initial m_subflows[0]
    QuicSocketBase::SendInitialHandshake (uint8_t type, const QuicHeader &quicHeader, Ptr<Packet> packet)

    // send packet
    QuicSocketBase::SendDataPacket (SequenceNumber32 packetNumber, uint32_t maxSize, bool withAck)
        send data packet with pathId
    QuicSocketBase::SendConnectionClosePacket (uint16_t errorCode, std::string phrase)
    QuicSocketBase::AbortConnection (uint16_t transportErrorCode, const char* reasonPhrase,bool applicationClose)

    // send ack, recv ack
    QuicSocketBase::SendAck ()
        send ack with pathId
    QuicSocketBase::OnSendingAckFrame (int pathId)
    QuicSocketBase::OnReceivedAckFrame (QuicSubheader &sub)

    //recv data 
    QuicSocketBase::ReceivedData (Ptr<Packet> p, const QuicHeader& quicHeader, Address &address)



2. quic-header 

    add announce type for subflow creation
    add path Id

3. quic-subheader

    add path Id for ack frame 

4. quic-l4-protocol

modified method:

    //add subflow in send&recv process
    QuicL4Protocol::ForwardUp (Ptr<Socket> sock)
    QuicL4Protocol::SendPacket (Ptr<QuicSocketBase> socket, Ptr<Packet> pkt, const QuicHeader &outgoing) const







