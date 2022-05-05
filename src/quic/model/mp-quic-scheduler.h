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


#ifndef MPQUICSCHEDULER_H
#define MPQUICSCHEDULER_H

#include "ns3/node.h"
#include "quic-socket-base.h"

namespace ns3 {

// class MpQuicSchedulerState : public Object
// {
// public:
//   static TypeId GetTypeId (void);

//   MpQuicSchedulerState ();
//   MpQuicSchedulerState (const MpQuicSchedulerState &other);
//   virtual  ~MpQuicSchedulerState (void)
//   {}
//   TracedValue<uint32_t> m_reward; 

// };




class MpQuicScheduler : public Object
{
public:

  typedef enum
    {
      ROUND_ROBIN,       
      MIN_RTT,
      BLEST,
      MAB
    } SchedulerType_t;
  
  /**
   * Get the type ID.
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);

  MpQuicScheduler (void);
  virtual ~MpQuicScheduler (void);

  uint8_t GetNextPathIdToUse();
  void SetSocket(Ptr<QuicSocketBase> sock);
    
  void UpdateReward (uint32_t oldValue, uint32_t newValue);

private:
  Ptr<QuicSocketBase> m_socket;
  uint8_t m_lastUsedPathId;
  
  
  std::vector <Ptr<MpQuicSubFlow>> m_subflows;
  SchedulerType_t m_schedulerType;


  void RoundRobin();
  void MinRtt();
  void Mab();
  void Blest();


  std::vector <uint32_t> m_rewards;
  uint32_t m_rounds;
  TracedValue<uint32_t> m_reward {0}; 
  uint16_t m_rate;
};

} // namespace ns3

#endif /* MP_QUIC_SCHEDULER_H */


