


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


TypeId
MpQuicScheduler::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::MpQuicScheduler")
    .SetParent<Object> ()
    .SetGroupName ("Internet")
    .AddAttribute ("SchedulerType",
                   "Minimum time in the future an RTO alarm may be set for",
                   IntegerValue (MIN_RTT),
                   MakeIntegerAccessor (&MpQuicScheduler::m_schedulerType),
                   MakeIntegerChecker<int16_t> ())
                   
  ;
  return tid;
}

MpQuicScheduler::MpQuicScheduler ()
  : Object (),
  m_socket(0),
  m_lastUsedPathId(0)
{
  NS_LOG_FUNCTION_NOARGS ();
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
  uint16_t minPid = 0;
  Time minRtt = m_subflows[0]->m_tcb->m_lastRtt;
  for (uint16_t pid = 1; pid < m_subflows.size(); pid++)
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


} // namespace ns3
