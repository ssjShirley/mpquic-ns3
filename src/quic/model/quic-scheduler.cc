


#include "ns3/object.h"
#include "ns3/log.h"
#include "ns3/uinteger.h"
#include "ns3/double.h"
#include "ns3/boolean.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/nstime.h"
#include "quic-stream.h"
#include "ns3/node.h"
#include "quic-scheduler.h"
#include "ns3/string.h"

#include <algorithm>
#include <iostream>
#include <iomanip>
#include <iterator>
#include <vector>


namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("QuicScheduler");

NS_OBJECT_ENSURE_REGISTERED (QuicScheduler);


TypeId
QuicScheduler::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::QuicScheduler")
    .SetParent<Object> ()
    .SetGroupName ("Internet")
    .AddAttribute ("SchedulerType",
                   "Minimum time in the future an RTO alarm may be set for",
                   StringValue ("RR"),
                   MakeStringAccessor (&QuicScheduler::m_type),
                   MakeStringChecker ())
                   
  ;
  return tid;
}

QuicScheduler::QuicScheduler ()
  : Object ()
{
  NS_LOG_FUNCTION_NOARGS ();
  m_sendIndex = 0;
  m_pid = 0;

}

QuicScheduler::~QuicScheduler ()
{
  NS_LOG_FUNCTION_NOARGS ();
  m_sendIndex = 0;
  m_pid = 0;
}


int
QuicScheduler::GetSend()
{
  if(m_sendIndex == 7000)
  {
    SetSendDefault();
  }
  if (m_type == "weight")
  {
    return WeightedRoundRobin();
  } 
  else if (m_type == "rtt")
  {
    return RttBase();
  }
  else
  {
    return RoundRobin();
  }
}


void
QuicScheduler::SetSendDefault()
{
  m_sendIndex = 0;
}

void
QuicScheduler::SetWeightType()
{
  m_type = "weight";
}

int
QuicScheduler::RoundRobin()
{
  m_sendIndex++;

  if(m_sendIndex%2 == 0 )
  {
    return 0;
  } 
  else 
  {
    return 1;
  }
}

int
QuicScheduler::WeightedRoundRobin()
{
  // std::cout<<m_sendIndex<<" index \n";
  m_sendIndex++;
  if(m_sendIndex >=2 && m_sendIndex<= 10)
  {
    return 0;
  } 
  else 
  {
    return 1;
  }
}

int
QuicScheduler::RttBase()
{
  m_sendIndex++;
  if(m_sendIndex > 1)
  {
    return m_pid;
  }
  if(m_sendIndex%2 == 0)
  {
    return 1;
  } 
  else 
  {
    return 0;
  }
}

void
QuicScheduler::SetMinPath(int p)
{
  m_pid = p;
}


} // namespace ns3
