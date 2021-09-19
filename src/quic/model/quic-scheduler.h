
#ifndef QUICSCHEDULER_H
#define QUICSCHEDULER_H

// #include "ns3/stream.h"
#include "ns3/node.h"



namespace ns3 {

/**
 * \ingroup quic
 *
 * \see QuicSchedulerBase
 * \brief (abstract) base class of all QuicSchedulers
 *
 * This class constitutes a basic implementation of a Quic Stream.
 *
 */
class QuicScheduler : public Object
{
public:
  /**
   * Get the type ID.
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);

  QuicScheduler (void);
  virtual ~QuicScheduler (void);


  int GetSend();
  void SetSendDefault();
  void SetWeightType();
  void SetMinPath(int p);

private:
  int WeightedRoundRobin();
  int RoundRobin();
  int RttBase();


  int m_sendIndex;
  std::string m_type;
  int m_pid;

 
};

} // namespace ns3

#endif /* QUIC_STREAM_H */


