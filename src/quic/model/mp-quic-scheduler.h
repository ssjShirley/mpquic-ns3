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


class MpQuicScheduler : public Object
{
public:

  typedef enum
    {
      ROUND_ROBIN,       
      MIN_RTT,
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
    
  
private:
  Ptr<QuicSocketBase> m_socket;
  uint8_t m_lastUsedPathId;
  
  
  std::vector <Ptr<MpQuicSubFlow>> m_subflows;
  SchedulerType_t m_schedulerType;


  void RoundRobin();
  void MinRtt();
  void Mab();

  std::vector <uint32_t> m_rewards;
  uint32_t m_rounds;
 
};

} // namespace ns3

#endif /* MP_QUIC_SCHEDULER_H */


