
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
      MIN_RTT            
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
 
};

} // namespace ns3

#endif /* MP_QUIC_SCHEDULER_H */


