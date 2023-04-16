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

#include "ns3/object.h"
#include "ns3/log.h"
#include "ns3/uinteger.h"
#include "ns3/double.h"
#include "ns3/boolean.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/nstime.h"
#include "quic-stream.h"
#include "ns3/node.h"
#include "ns3/string.h"


#include <algorithm>
#include <iostream>
#include <iomanip>
#include <iterator>
#include <vector>


#include "mp-quic-path-manager.h"
#include "mp-quic-subflow.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("MpQuicPathManager");

NS_OBJECT_ENSURE_REGISTERED (MpQuicPathManager);


TypeId
MpQuicPathManager::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::MpQuicPathManager")
    .SetParent<Object> ()
    .SetGroupName ("Internet")                  
  ;
  return tid;
}

MpQuicPathManager::MpQuicPathManager ()
  : m_socket(0),
  m_segSize(0),
  m_initialSsThresh(0)
{
  NS_LOG_FUNCTION_NOARGS ();
 
}

MpQuicPathManager::~MpQuicPathManager ()
{
  NS_LOG_FUNCTION_NOARGS ();
  
}

Ptr<MpQuicSubFlow>
MpQuicPathManager::InitialSubflow0 (Address localAddress, Address peerAddress)
{
  NS_LOG_FUNCTION(this);

  Ptr<MpQuicSubFlow> sFlow = CreateObject<MpQuicSubFlow> ();
  sFlow->m_flowId    = 0; 
  sFlow->m_peerAddr  = peerAddress;
  sFlow->m_localAddr = localAddress;
  sFlow->m_subflowState = MpQuicSubFlow::Active;
  sFlow->SetSegSize(m_segSize);
  sFlow->m_tcb->m_initialSsThresh = m_initialSsThresh;
  m_socket->SubflowInsert(sFlow);
  bool ok;
  ok = sFlow->m_tcb->TraceConnectWithoutContext ("CongestionWindow", MakeCallback (&QuicSocketBase::UpdateCwnd, m_socket));
  NS_ASSERT_MSG (ok == true, "Failed connection to CWND0 trace");
  ok = sFlow->m_tcb->TraceConnectWithoutContext ("SlowStartThreshold", MakeCallback (&QuicSocketBase::UpdateSsThresh, m_socket));
  NS_ASSERT_MSG (ok == true, "Failed connection to SSTHR0 trace");
  ok = sFlow->m_tcb->TraceConnectWithoutContext ("RTT", MakeCallback (&QuicSocketBase::TraceRTT0, m_socket));
  NS_ASSERT_MSG (ok == true, "Failed connection to RTT0 trace");
  return sFlow;

}

Ptr<MpQuicSubFlow>
MpQuicPathManager::AddSubflow(Address localAddress, Address peerAddress, uint8_t pathId)
{
    
  NS_LOG_FUNCTION(this);
  Ptr<MpQuicSubFlow> sFlow = CreateObject<MpQuicSubFlow> ();
  sFlow->m_flowId    = pathId; 
  sFlow->m_localAddr = localAddress;
  sFlow->m_peerAddr = peerAddress;

  sFlow->m_subflowState = MpQuicSubFlow::Validating;
  sFlow->SetSegSize(m_segSize);
  sFlow->m_tcb->m_initialSsThresh = m_initialSsThresh;
  sFlow->m_tcb->m_cWnd = sFlow->m_tcb->m_initialCWnd;
  sFlow->m_tcb->m_ssThresh = sFlow->m_tcb->m_initialSsThresh;
  m_socket->SubflowInsert(sFlow);
  m_socket->AddPath(localAddress, peerAddress, pathId);
  m_socket->SendAddAddress(localAddress, pathId);
  
  return sFlow;

}


Ptr<MpQuicSubFlow>
MpQuicPathManager::AddSubflowWithPeerAddress(Address localAddress, Address peerAddress, uint8_t pathId)
{
  NS_LOG_FUNCTION(this);
  Ptr<MpQuicSubFlow> sFlow = CreateObject<MpQuicSubFlow> ();
  sFlow->m_flowId     = pathId; 
  sFlow->m_localAddr  = localAddress;
  sFlow->m_peerAddr   = peerAddress;
  sFlow->m_subflowState = MpQuicSubFlow::Validating;
  sFlow->SetSegSize(m_segSize);
  sFlow->m_tcb->m_initialSsThresh = m_initialSsThresh;
  sFlow->m_tcb->m_cWnd = sFlow->m_tcb->m_initialCWnd;
  sFlow->m_tcb->m_ssThresh = sFlow->m_tcb->m_initialSsThresh;
  m_socket->SubflowInsert(sFlow);
  m_socket->SendPathChallenge(pathId);
  bool ok;
  ok = sFlow->m_tcb->TraceConnectWithoutContext ("CongestionWindow", MakeCallback (&QuicSocketBase::UpdateCwnd1, m_socket));
  NS_ASSERT_MSG (ok == true, "Failed connection to CWND trace");
  ok = sFlow->m_tcb->TraceConnectWithoutContext ("SlowStartThreshold", MakeCallback (&QuicSocketBase::UpdateSsThresh1, m_socket));
  NS_ASSERT_MSG (ok == true, "Failed connection to SSTHR1 trace");
  ok = sFlow->m_tcb->TraceConnectWithoutContext ("RTT", MakeCallback (&QuicSocketBase::TraceRTT1, m_socket));
  NS_ASSERT_MSG (ok == true, "Failed connection to RTT1 trace");
  return sFlow;
}


void
MpQuicPathManager::SetSocket(Ptr<QuicSocketBase> sock)
{
  NS_LOG_FUNCTION (this);
  m_socket = sock;
}

void 
MpQuicPathManager::SetSegSize(uint32_t size)
{
  NS_LOG_FUNCTION(this);
  m_segSize = size;
}

uint32_t 
MpQuicPathManager::GetSegSize() const
{
  NS_LOG_FUNCTION(this);
  return m_segSize;
}

void
MpQuicPathManager::SetInitialSSThresh (uint32_t threshold)
{
  m_initialSsThresh = threshold;
}

uint32_t
MpQuicPathManager::GetInitialSSThresh (void) const
{
  return m_initialSsThresh;
}


}