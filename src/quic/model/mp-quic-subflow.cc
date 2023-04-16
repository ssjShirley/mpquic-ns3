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


#include <stdint.h>
#include <iostream>
#include "ns3/buffer.h"
#include "ns3/address-utils.h"
#include "ns3/log.h"
#include "mp-quic-subflow.h"
#include <stdlib.h>
#include <queue>
#include "ns3/traced-value.h"
#include "ns3/trace-source-accessor.h"
#include <ns3/object-base.h>
#include "ns3/simulator.h"
#include "time.h"
#include "ns3/string.h"

#include "quic-socket-tx-scheduler.h"
#include "quic-socket-base.h"

#include <algorithm>
#include <math.h>
#include <cmath>

NS_LOG_COMPONENT_DEFINE ("MpQuicSubFlow");
namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (MpQuicSubFlow);


TypeId
MpQuicSubFlow::GetTypeId (void)
{
    static TypeId tid = TypeId ("ns3::MpQuicSubFlow")
        .SetParent (Object::GetTypeId ())

        .AddTraceSource ("CWindow",
                     "The QUIC connection's congestion window",
                     MakeTraceSourceAccessor (&MpQuicSubFlow::m_cWndTrace),
                     "ns3::TracedValueCallback::Uint32")
        ;
      return tid;
}

MpQuicSubFlow::MpQuicSubFlow()
    : m_flowId (0),
      m_lastMaxData(0),
      m_maxDataInterval(10),
      m_rounds(1)
{

    m_numPacketsReceivedSinceLastAckSent = 0;
    m_queue_ack = false;
    m_receivedPacketNumbers = std::vector<SequenceNumber32> ();

    //For congestion control
    m_tcb = CreateObject<QuicSocketState> ();
    m_tcb->m_cWnd = m_tcb->m_initialCWnd;
    m_tcb->m_ssThresh = m_tcb->m_initialSsThresh;
    m_tcb->m_pacingRate = m_tcb->m_maxPacingRate;

    // connect callbacks
    bool ok;
    ok = m_tcb->TraceConnectWithoutContext ("CongestionWindow",
                                            MakeCallback (&MpQuicSubFlow::UpdateCwnd, this));
    NS_ASSERT_MSG (ok == true, "Failed connection to CWND trace");

}

MpQuicSubFlow::~MpQuicSubFlow()
{
    m_flowId     = 0;
}



void
MpQuicSubFlow::SetSegSize (uint32_t size)
{
  NS_LOG_FUNCTION (this << size);

  m_tcb->m_segmentSize = size;
  m_tcb->m_initialCWnd = 2 * size;
  m_tcb->m_kMinimumWindow = 2 * size;
}

uint32_t
MpQuicSubFlow::GetSegSize (void) const
{
  return m_tcb->m_segmentSize;
}

double
MpQuicSubFlow::GetRate()
{
    if (m_tcb->m_lastRtt.Get().GetSeconds() == 0){
        return 0;
    }
    return m_tcb->m_cWnd/m_tcb->m_segmentSize/m_tcb->m_lastRtt.Get().GetSeconds();
} 

void
MpQuicSubFlow::UpdateCwnd (uint32_t oldValue, uint32_t newValue)
{
  m_cWndTrace (oldValue, newValue);
}


} // namespace ns3
