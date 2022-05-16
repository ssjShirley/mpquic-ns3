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

#include "mp-quic-scheduler.h"



namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("MpQuicScheduler");

NS_OBJECT_ENSURE_REGISTERED (MpQuicScheduler);
// NS_OBJECT_ENSURE_REGISTERED (MpQuicSchedulerState);

// TypeId
// MpQuicSchedulerState::GetTypeId (void)
// {
//   static TypeId tid =
//     TypeId ("ns3::MpQuicSchedulerState")
//     .SetParent<Object> ()
//     .SetGroupName ("Internet")
//     .AddTraceSource ("MabReward",
//                      "The average reward get by mab",
//                      MakeTraceSourceAccessor (&MpQuicSchedulerState::m_reward),
//                      "ns3::TracedValueCallback::Uint32") 
// ;
//   return tid;
// }

// MpQuicSchedulerState::MpQuicSchedulerState ()
//   : Object ()
// {

// }

// MpQuicSchedulerState::MpQuicSchedulerState (const MpQuicSchedulerState &other)
//   : Object (other),
//     m_reward(other.m_reward)
// {
// }


TypeId
MpQuicScheduler::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::MpQuicScheduler")
    .SetParent<Object> ()
    .SetGroupName ("Internet")
    .AddAttribute ("SchedulerType",
                   "define the type of the scheduler",
                   IntegerValue (MIN_RTT),
                   MakeIntegerAccessor (&MpQuicScheduler::m_schedulerType),
                   MakeIntegerChecker<int16_t> ())
    .AddAttribute ("MabRate",
                   "define the rate of the MAB scheduler",
                   UintegerValue (10000),
                   MakeUintegerAccessor (&MpQuicScheduler::m_rate),
                   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("BlestLambda",
                   "define the lambda of the BLEST",
                   UintegerValue (1000),
                   MakeUintegerAccessor (&MpQuicScheduler::m_lambda),
                   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("BlestVar",
                   "define the lambda of the BLEST",
                   UintegerValue (100),
                   MakeUintegerAccessor (&MpQuicScheduler::m_bVar),
                   MakeUintegerChecker<uint16_t> ())
    .AddTraceSource ("MabReward",
                     "The average reward get by mab",
                     MakeTraceSourceAccessor (&MpQuicScheduler::m_reward),
                     "ns3::TracedValueCallback::Uint32")               
     
  ;
  return tid;
}

MpQuicScheduler::MpQuicScheduler ()
  : Object (),
  m_socket(0),
  m_lastUsedPathId(0),
  m_rounds(0),
  m_reward(0)
{
  NS_LOG_FUNCTION_NOARGS ();
  m_rewards.push_back(0);
  // m_state = CreateObject<MpQuicSchedulerState> ();
  // m_state->TraceConnectWithoutContext ("MabReward", MakeCallback (&MpQuicScheduler::UpdateReward, this));
}

MpQuicScheduler::~MpQuicScheduler ()
{
  NS_LOG_FUNCTION_NOARGS ();
}



uint8_t 
MpQuicScheduler::GetNextPathIdToUse()
{
  m_subflows = m_socket->GetActiveSubflows();
  if (m_subflows.empty())
  {
    m_lastUsedPathId = 0;
    return 0;
  }
  switch (m_schedulerType)
  {
    case ROUND_ROBIN:
      RoundRobin();
      break;

    case MIN_RTT:
      MinRtt();
      break;
    
    case BLEST:
      Blest();
      break;

    case MAB:
      Mab();
      break;

    default:
      RoundRobin();
      break;
      
  }

  return m_lastUsedPathId;
}

void
MpQuicScheduler::RoundRobin()
{
  m_lastUsedPathId = (m_lastUsedPathId + 1) % m_subflows.size();
}

void
MpQuicScheduler::MinRtt()
{
  NS_LOG_FUNCTION (this);
  uint8_t minPid = 0;
  Time minRtt = m_subflows[0]->m_tcb->m_lastRtt;
  for (uint8_t pid = 1; pid < m_subflows.size(); pid++)
  {
    if (m_subflows[pid]->m_tcb->m_lastRtt < minRtt){
      minRtt = m_subflows[pid]->m_tcb->m_lastRtt;
      minPid = pid;
    }
  }
  m_lastUsedPathId = minPid;
}

void
MpQuicScheduler::SetSocket(Ptr<QuicSocketBase> sock)
{
  NS_LOG_FUNCTION (this);
  m_socket = sock;
}


void
MpQuicScheduler::Blest() //only allow two subflows
{
  NS_LOG_FUNCTION (this);
  if (m_subflows.size() <= 1){
    m_lastUsedPathId = 0;
    return;
  }
  
  if (m_subflows[1]->m_tcb->m_lastRtt.Get().GetSeconds() == 0) {
    m_lastUsedPathId = (m_lastUsedPathId + 1) % m_subflows.size();
    return;
  }
  // MinRtt();
  
  Time rttS;
  Time rttF;
  uint8_t fastPathId = 1;
  uint8_t slowPathId = 1;
  uint32_t mss = m_socket->GetSegSize();

  m_lastUsedPathId = (m_lastUsedPathId + 1) % m_subflows.size();

  if (m_subflows[0]->m_tcb->m_lastRtt > m_subflows[1]->m_tcb->m_lastRtt){
    rttS = m_subflows[0]->m_tcb->m_lastRtt;
    rttF = m_subflows[1]->m_tcb->m_lastRtt;
    slowPathId = 0;
    fastPathId = 1;
  } else {
    rttS = m_subflows[1]->m_tcb->m_lastRtt;
    rttF = m_subflows[0]->m_tcb->m_lastRtt;
    slowPathId = 1;
    fastPathId = 0;
  }

  if (m_lastUsedPathId == slowPathId) {
    double_t rtts = rttS.GetSeconds()/rttF.GetSeconds();
    double_t cwndF = m_subflows[fastPathId]->m_tcb->m_cWnd/mss;
    double_t X = mss * (cwndF + (rtts-1)/2) * rtts;
    double_t comp = m_socket->GetTxAvailable() - (m_socket->BytesInFlight(slowPathId)+mss);
    m_lambda = m_lambda + m_bVar;
    if(X * m_lambda > comp) { //not send on slow path
      m_lastUsedPathId = fastPathId;
    } 
  }
  
  // std::cout <<"In use pid: " << unsigned(m_lastUsedPathId) <<std::endl;
}


void
MpQuicScheduler::Mab()
{
  NS_LOG_FUNCTION (this);
  uint8_t rPid = 0;
  uint32_t mss = m_socket->GetSegSize();
  Time rtt = m_subflows[0]->m_tcb->m_lastRtt;
  uint32_t reward = mss/rtt.GetSeconds(); // * 1/std::sqrt(m_lostPackets)
  m_rewards[0] = (m_rewards[0]*(m_subflows[0]->m_rounds) + reward)/(m_subflows[0]->m_rounds+1);
  double delta = 1+ m_rounds * std::log(m_rounds) * std::log(m_rounds);
  double_t normal = std::sqrt(2*std::log(delta)/m_subflows[0]->m_rounds);
  uint32_t maxAction = m_rewards[0] + normal*m_rate;
  for (uint8_t pid = 1; pid < m_subflows.size(); pid++)
  {
    mss = m_socket->GetSegSize();
    rtt = m_subflows[pid]->m_tcb->m_lastRtt;
    reward = mss/rtt.GetSeconds(); // * 1/std::sqrt(m_lostPackets)
    if(m_rewards.size() < m_subflows.size()){
      m_rewards.push_back(0);
    }
    m_rewards[pid] = (m_rewards[pid]*(m_subflows[pid]->m_rounds) + reward)/(m_subflows[pid]->m_rounds+1);
    
    normal = std::sqrt(2*std::log(delta)/m_subflows[pid]->m_rounds);
    uint32_t newAction = m_rewards[pid] + normal*m_rate;
    if (m_rewards[pid] == 0 && newAction == 1) { //when new path just established
      rPid = pid;
    }
    // if (rtt.GetSeconds() != 0){
    //   rPid = pid;
    // }
    if (newAction >= maxAction){
      maxAction = newAction;
      rPid = pid;
    }
  }
  m_lastUsedPathId = rPid;
  m_subflows[m_lastUsedPathId]->m_rounds++;
  
  m_reward = ((m_reward*m_rounds)+m_rewards[rPid])/(m_rounds+1);
  m_rounds++;
  // std::cout <<"In use pid: " << unsigned(m_lastUsedPathId) <<std::endl;

  // NS_LOG_INFO("In use pid: " << unsigned(m_lastUsedPathId));
}


void
MpQuicScheduler::SetNumOfLostPackets(uint16_t lost){
  m_lostPackets = lost;
}



} // namespace ns3
