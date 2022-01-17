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


#include "mp-quic-path-manager.h"
#include "mp-quic-typedefs.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("MpQuicPathManager");

NS_OBJECT_ENSURE_REGISTERED (MpQuicPathManager);


TypeId
MpQuicPathManager::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::MpQuicPathManager")
    .SetParent<Object> ()
    .SetGroupName ("Internet")                  
  ;
  return tid;
}

MpQuicPathManager::MpQuicPathManager ()
  : m_socket(0),
  m_segSize(0),
  m_initialSsThresh(0)
{
  NS_LOG_FUNCTION_NOARGS ();
 
}

MpQuicPathManager::~MpQuicPathManager ()
{
  NS_LOG_FUNCTION_NOARGS ();
  
}

Ptr<MpQuicSubFlow>
MpQuicPathManager::InitialSubflow0 (Address localAddress, Address peerAddress)
{
  NS_LOG_FUNCTION(this);

  Ptr<MpQuicSubFlow> sFlow = CreateObject<MpQuicSubFlow> ();
  sFlow->m_flowId    = 0; 
  sFlow->m_peerAddr  = peerAddress;
  sFlow->m_localAddr = localAddress;
  sFlow->m_subflowState = MpQuicSubFlow::ACTIVE;
  sFlow->SetSegSize(m_segSize);
  m_socket->SubflowInsert(sFlow);
  return sFlow;

}

Ptr<MpQuicSubFlow>
MpQuicPathManager::AddSubflow(Address localAddress, Address peerAddress, int16_t pathId)
{
    
  NS_LOG_FUNCTION(this);
  Ptr<MpQuicSubFlow> sFlow = CreateObject<MpQuicSubFlow> ();
  sFlow->m_flowId    = pathId; 
  sFlow->m_localAddr = localAddress;
  sFlow->m_peerAddr = peerAddress;

  sFlow->m_subflowState = MpQuicSubFlow::PENDING;
  sFlow->SetSegSize(m_segSize);

  m_socket->SubflowInsert(sFlow);
  m_socket->AddPath(localAddress, peerAddress, pathId);
  m_socket->SendAddAddress(localAddress, pathId);
  
  return sFlow;

}


Ptr<MpQuicSubFlow>
MpQuicPathManager::AddSubflowWithPeerAddress(Address localAddress, Address peerAddress, int16_t pathId)
{
  NS_LOG_FUNCTION(this);
  Ptr<MpQuicSubFlow> sFlow = CreateObject<MpQuicSubFlow> ();
  sFlow->m_flowId     = pathId; 
  sFlow->m_localAddr  = localAddress;
  sFlow->m_peerAddr   = peerAddress;
  sFlow->m_subflowState = MpQuicSubFlow::PENDING;
  sFlow->SetSegSize(m_segSize);
  m_socket->SubflowInsert(sFlow);
  m_socket->SendPathChallenge(pathId);
  return sFlow;
}


void
MpQuicPathManager::SetSocket(Ptr<QuicSocketBase> sock)
{
  NS_LOG_FUNCTION (this);
  m_socket = sock;
}

void 
MpQuicPathManager::SetSegSize(uint32_t size)
{
  NS_LOG_FUNCTION(this);
  m_segSize = size;
}

uint32_t 
MpQuicPathManager::GetSegSize() const
{
  NS_LOG_FUNCTION(this);
  return m_segSize;
}

void
MpQuicPathManager::SetInitialSSThresh (uint32_t threshold)
{
  m_initialSsThresh = threshold;
}

uint32_t
MpQuicPathManager::GetInitialSSThresh (void) const
{
  return m_initialSsThresh;
}


}