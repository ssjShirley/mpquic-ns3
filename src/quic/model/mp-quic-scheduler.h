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
#include <eigen3/Eigen/Dense>
using Eigen::MatrixXd;
using Eigen::VectorXd;

namespace ns3 {

class MpQuicScheduler : public Object
{
public:

  typedef enum
    {
      ROUND_ROBIN,       
      MIN_RTT,
      BLEST,
      ECF,
      PEEKABOO
    } SchedulerType_t;
  
  /**
   * Get the type ID.
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);

  MpQuicScheduler (void);
  virtual ~MpQuicScheduler (void);

  std::vector<double> GetNextPathIdToUse();
  void SetSocket(Ptr<QuicSocketBase> sock);
    
  void UpdateReward (uint32_t oldValue, uint32_t newValue);
  void SetNumOfLostPackets(uint16_t lost);
  void UpdateRewardMab(uint8_t pathId, uint32_t lostOut, uint32_t inflight, uint32_t round);
  uint32_t GetCurrentRound();

  void PeekabooReward(uint8_t pathId, Time lastActTime);

private:
  Ptr<QuicSocketBase> m_socket;
  uint8_t m_lastUsedPathId;
  
  
  std::vector <Ptr<MpQuicSubFlow>> m_subflows;
  SchedulerType_t m_schedulerType;


  std::vector<double> RoundRobin();
  std::vector<double> MinRtt();
  std::vector<double> Peekaboo();
  std::vector<double> MabDelay();
  std::vector<double> Blest();
  std::vector<double> Ecf();

  uint32_t m_rate;
  uint16_t m_lostPackets;
  uint16_t m_lambda;
  uint16_t m_bVar;
  uint8_t m_waiting = 0;
  uint16_t m_select;

  std::vector <double> m_cost;
  std::vector <uint32_t> m_missingRounds;
  uint32_t m_lastUpdateRounds;
  uint32_t m_e;
  std::vector <double> m_L;
  std::vector <double> m_eL;
  std::vector <double> m_p;
  std::vector <double> EPR;
  std::vector <MatrixXd> A;
  std::vector <VectorXd> b;
  VectorXd peek_x = VectorXd::Constant(6,0);
  double T_r, g = 1, R = 0, T_e;
  double rtt[8];
};

} // namespace ns3

#endif /* MP_QUIC_SCHEDULER_H */


