/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2022 Pan Lab, Department of Computer Science, University of Victoria
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Authors: Shengjie Shu <shengjies@uvic.ca>
 */

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

#ifndef MP_QUIC_SUBFLOW_H
#define MP_QUIC_SUBFLOW_H

using namespace std;

namespace ns3 {

class MpQuicSubFlow : public Object
{
public:

    typedef enum
    {
      PENDING,       //!< subflow is initialed, cannot send data yet
      ACTIVE         //!< sublfow is fully active
    } SubflowStates_t;
    
    static TypeId GetTypeId (void);

    MpQuicSubFlow ();
    ~MpQuicSubFlow ();

    uint16_t m_flowId;
    Address m_localAddr;
    Address m_peerAddr;

    TracedValue<SubflowStates_t> m_subflowState;     //!< State 
    // TracedValue<uint32_t>  m_bytesInFlight {0};      //!< Bytes in flight

    void SetSegSize (uint32_t size);
    uint32_t GetSegSize (void) const;
    double GetRate();


    void UpdateCwnd (uint32_t oldValue, uint32_t newValue);

    // The following member parameters are moved from 'quic-socket-base.h'
    // Timers and Events
    EventId m_sendPendingDataEvent;             //!< Micro-delay event to send pending data
    EventId m_retxEvent;                        //!< Retransmission event
    EventId m_idleTimeoutEvent;                 //!< Event triggered upon receiving or sending a packet, when it expires the connection closes
    EventId m_drainingPeriodEvent;              //!< Event triggered upon idle timeout or immediate connection close, when it expires all closes
    TracedValue<Time> m_rto;                    //!< Retransmit timeout
    TracedValue<Time> m_drainingPeriodTimeout;  //!< Draining Period timeout
    EventId m_sendAckEvent;                     //!< Send ACK timeout event
    EventId m_delAckEvent;                      //!< Delayed ACK timeout event
    bool m_flushOnClose;                        //!< Control behavior on connection close
    bool m_closeOnEmpty;                        //!< True if the socket will close after sending the buffered packets

    // Congestion Control
    Ptr<QuicSocketState> m_tcb;                     //!< Congestion control informations
    // Ptr<TcpCongestionOps> m_congestionControl;      //!< Congestion control
    // TracedValue<Time> m_lastRtt;                    //!< Latest measured RTT
    // bool m_quicCongestionControlLegacy;             //!< Quic Congestion control if true, TCP Congestion control if false
    bool m_queue_ack;                               //!< Indicates a request for a queue ACK if true
    uint32_t m_numPacketsReceivedSinceLastAckSent;  //!< Number of packets received since last ACK sent
    uint32_t m_lastMaxData;                         //!< Last MaxData ACK
    uint32_t m_maxDataInterval;                     //!< Interval between successive MaxData frames in ACKs
    // uint32_t m_initialPacketSize;                   //!< size of the first packet to be sent durin the handshake (at least 1200 bytes, per RFC)

    // Pacing timer
    Timer m_pacingTimer       {Timer::REMOVE_ON_DESTROY}; //!< Pacing Event
    std::vector<SequenceNumber32> m_receivedPacketNumbers;  //!< Received packet number vector

    

      // void Add (SequenceNumber32 ack);
    // void UpdateRtt (SequenceNumber32 ack, Time ackDelay);

    // void CwndOnAckReceived(double alpha, double sum_rate, double max_rate, std::vector<Ptr<QuicSocketTxItem> > newAcks, uint32_t ackedBytes);
    // void UpdateCwndOlia(double sum_rate, double alpha, std::vector<Ptr<QuicSocketTxItem> > newAcks);
    // void UpdateCwndMmQuic(double sum_rate, double max_rate, std::vector<Ptr<QuicSocketTxItem> > newAcks);

    // void UpdateSsThresh(double snr,uint32_t ssh);
    // static void UpdateSsh(uint32_t ssh, int id);
    // void UpdateCwndOnPacketLost();
    // void SetInitialCwnd(uint32_t cwnd);
    // uint32_t GetMinPrevLossCwnd();
    
    // void InitialRateEvent ();
    // std::string m_ccType;
    // std::string m_cwndState;

    // uint16_t    routeId;
    // bool        connected;
    // Ipv4Address sAddr;
    // uint16_t    sPort;
    // Ipv4Address dAddr;
    // uint16_t    dPort;

    // TracedValue<uint32_t> m_cWnd;
    
    
    // uint32_t m_segmentSize;         
    // uint32_t m_ssThresh;
    // static std::vector<uint32_t> m_sst;
    // uint64_t m_bandwidth;
    // uint64_t m_bwEst;
    // uint32_t m_lossCwnd;
    // int ackSize;
    // double m_throughput;
    // TracedValue<double> m_throughputBps;
    // TracedValue<Time> m_rttTrace;

    // multiset<double> measuredRTT;
    // //list<double> measuredRTT;
    // Ptr<RttEstimator> m_rtt;
    // TracedValue<Time>     lastMeasuredRtt;
    // Time     lastMeasuredRttp;
    // Time     largestRtt;
    // std::vector<MpRttHistory> m_unackedPackets; 

    // TracedValue<SequenceNumber32> m_nextPktNum {1}; 
    // std::vector<SequenceNumber32> m_receivedSeqNumbers;
    

    // uint32_t m_lost1;
    // uint32_t m_lost2;
    // TypeId m_schedulingTypeId;                      //!< The socket type of the packet scheduler
    // Time m_defaultLatency;                          //!< The default latency bound (only used by the EDF scheduler)

private:
  TracedCallback<uint32_t, uint32_t> m_cWndTrace;
//   double m_delay;
};


}//namespace ns3
#endif //MP_QUIC_SUBFLOW_H
