#include <vector>
#include <map>
#include "ns3/sequence-number.h"
#include "ns3/rtt-estimator.h"
#include "ns3/ipv4-address.h"
#include "ns3/event-id.h"
#include <stdint.h>
#include <queue>
#include <list>
#include <set>

#include "ns3/object.h"
#include "ns3/uinteger.h"
#include "ns3/traced-value.h"
#include "ns3/trace-source-accessor.h"

#include "quic-socket-tx-buffer.h"
#include "quic-socket-base.h"

#ifndef MP_Quic_TYPEDEFS_H
#define MP_Quic_TYPEDEFS_H

using namespace std;

namespace ns3 {


// congestion control algorithm
typedef enum {
  Uncoupled_Quics,       // 0
  Linked_Increases,     // 1
  RTT_Compensator,      // 2
  Fully_Coupled         // 3
  } CongestionCtrl_t;

// connection phase
typedef enum {
  Slow_Start,                   // 0
  Congestion_Avoidance,         // 1
  DSACK_SS,                     // 2 DSACK Slow Start: a temporary slow start triggered after detecting spurious retransmission based on DSACK information
  RTO_Recovery                  // 3 Reconvery algorithm after RTO expiration
  } Phase_t;

typedef enum {
  Round_Robin,        // 0
  //Collision_Avoidance         // 1
  LATE,
  minRTT
  } DataDistribAlgo_t;

typedef enum {
  NoPR_Algo,    // 0
  Eifel,        // 1
  Quic_DOOR,     // 2 Detection of Out-of-Order and Response
  D_SACK,       // 3 Duplicate SACK (Selective ACKnowledgement)
  F_RTO         // 4 Forward RTO-Recovery: Algorithm for detecting spurious retransmission timeouts
  } PacketReorder_t;

typedef enum {
  Step_1,       // 0
  Step_2,       // 1
  Step_3,       // 2
  Step_4        // 3 In this step of F-RTO do a standard Fast Recovery algorithm
  } FRtoStep_t;


class MpRttHistory
{
public:

  MpRttHistory (SequenceNumber32 s, Time t);
  
public:
  SequenceNumber32  seq;  //!< First sequence number in packet sent
  // uint32_t        count;  //!< Number of bytes sent
  Time            time;   //!< Time this one was sent
  bool            retx;   //!< True if this has been retransmitted
};


class MpQuicSubFlow : public Object
{
public:
    static TypeId GetTypeId (void);

    MpQuicSubFlow ();
    ~MpQuicSubFlow ();

    void Add (SequenceNumber32 ack);
    void UpdateRtt (SequenceNumber32 ack, Time ackDelay);

    void CwndOnAckReceived(double alpha, double sum_rate, double max_rate, std::vector<Ptr<QuicSocketTxItem> > newAcks, uint32_t ackedBytes);
    void UpdateCwndOlia(double sum_rate, double alpha, std::vector<Ptr<QuicSocketTxItem> > newAcks);

    void UpdateSsThresh(double snr,uint32_t ssh);
    static void UpdateSsh(uint32_t ssh, int id);
    void UpdateCwndOnPacketLost();
    void SetInitialCwnd(uint32_t cwnd);
    double GetRate();
    std::string m_ccType;
    std::string m_cwndState;

    uint16_t    routeId;
    bool        connected;
    Ipv4Address sAddr;
    uint16_t    sPort;
    Ipv4Address dAddr;
    uint16_t    dPort;

    TracedValue<uint32_t> m_cWnd;
    

    uint32_t m_segmentSize;         
    uint32_t m_ssThresh;
    static std::vector<uint32_t> m_sst;
    uint64_t m_bandwidth;
    uint64_t m_bwEst;
    uint32_t m_lossCwnd;
    int ackSize;
    double m_throughput;
    TracedValue<double> m_throughputBps;
    TracedValue<Time> m_rttTrace;

    multiset<double> measuredRTT;
    //list<double> measuredRTT;
    Ptr<RttEstimator> m_rtt;
    TracedValue<Time>     lastMeasuredRtt;
    Time     lastMeasuredRttp;
    Time     largestRtt;
    std::vector<MpRttHistory> m_unackedPackets; 

    TracedValue<SequenceNumber32> m_nextPktNum {1}; 
    std::vector<SequenceNumber32> m_receivedSeqNumbers;
    
    Ptr<QuicSocketState> m_tcb;                     //!< Congestion control informations

    uint32_t m_lost1;
    uint32_t m_lost2;
    

private:

};

class MpQuicAddressInfo
{
public:
    MpQuicAddressInfo();
    ~MpQuicAddressInfo();

    uint8_t     addrID;
    Ipv4Address ipv4Addr;
    Ipv4Mask    mask;
};





}//namespace ns3
#endif //MP_Quic_TYPEDEFS_H
