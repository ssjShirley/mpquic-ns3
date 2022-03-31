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
      Validating,       //!< subflow is initialed, cannot send data yet
      Active,           //!< sublfow is fully active
      Closeing,         //!< subflow is closing
      Closed            //!< subflow is fully closed
    } SubflowStates_t;
    
    static TypeId GetTypeId (void);

    MpQuicSubFlow ();
    ~MpQuicSubFlow ();

    uint16_t m_flowId;
    Address m_localAddr;
    Address m_peerAddr;

    TracedValue<SubflowStates_t> m_subflowState;     //!< State 

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

    uint32_t m_rounds;

private:
  TracedCallback<uint32_t, uint32_t> m_cWndTrace;

};


}//namespace ns3
#endif //MP_QUIC_SUBFLOW_H
