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

#ifndef MpQUICPATHMANAGER_H
#define MpQUICPATHMANAGER_H

#include "ns3/node.h"

#include "quic-socket-base.h"
#include "mp-quic-subflow.h"

namespace ns3 {

/**
 * \ingroup quic
 *
 * \see MpQuicPathManagerBase
 * \brief (abstract) base class of all MpQuicPathManagers
 *
 * This class constitutes a basic implementation of a Quic Stream.
 *
 */
class MpQuicPathManager : public Object
{
public:
  /**
   * Get the type ID.
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);

  MpQuicPathManager (void);
  virtual ~MpQuicPathManager (void);


  Ptr<MpQuicSubFlow> InitialSubflow0 (Address localAddress, Address peerAddress);
  Ptr<MpQuicSubFlow> AddSubflow(Address address, Address from, uint8_t pathId);
  Ptr<MpQuicSubFlow> AddSubflowWithPeerAddress(Address localAddress, Address peerAddress, uint8_t pathId);
 
  void SetSocket(Ptr<QuicSocketBase> sock);
  void SetSegSize(uint32_t size);
  uint32_t GetSegSize() const;
  void SetInitialSSThresh (uint32_t threshold);
  uint32_t GetInitialSSThresh (void) const;
  
  

private:
  Ptr<QuicSocketBase> m_socket;
  uint32_t m_segSize;
  uint32_t m_initialSsThresh;
 
 
};

} // namespace ns3

#endif /* MP_QUIC_PATH_MANAGER_H */


