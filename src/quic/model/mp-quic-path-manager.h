#ifndef MpQUICPATHMANAGER_H
#define MpQUICPATHMANAGER_H

// #include "ns3/stream.h"
#include "ns3/node.h"
#include <boost/random.hpp>
#include <boost/random/discrete_distribution.hpp>
#include <boost/random/mersenne_twister.hpp>

#include "quic-socket-base.h"
#include "mp-quic-typedefs.h"

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
  Ptr<MpQuicSubFlow> AddSubflow(Address address, Address from, int16_t pathId);
  Ptr<MpQuicSubFlow> AddSubflowWithPeerAddress(Address localAddress, Address peerAddress, int16_t pathId);
 
  // uint8_t LookUpByAddr (Address &address);
  void SetSocket(Ptr<QuicSocketBase> sock);
  
  
  

private:
  Ptr<QuicSocketBase> m_socket;
  // std::map <Ipv4Address, uint8_t> m_addrIdPair;
 
 
};

} // namespace ns3

#endif /* MP_QUIC_PATH_MANAGER_H */


