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
#include <boost/random.hpp>
#include <boost/random/discrete_distribution.hpp>
#include <boost/random/mersenne_twister.hpp>

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
  : m_socket(0)
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
  m_socket->SubflowInsert(sFlow);
  // m_addrIdPair.insert(std::pair<Ipv4Address, uint8_t> (pIpv4, 0));
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


// //ywj
// uint8_t
// MpQuicPathManager::LookUpByAddr (Address &address)
// {
//    uint8_t sFlowIdx;
//    InetSocketAddress transport = InetSocketAddress::ConvertFrom (address);
//    Ipv4Address ipv4 = transport.GetIpv4 ();
//    uint16_t port = transport.GetPort ();
   

//    auto result = m_addrIdPair.find(ipv4);
//    if (result == m_addrIdPair.end())   //no found src in the existing addr_id map
//      {
//         m_subSocket = true;
//         //server create subflow when receive announce
//         m_subSocket = false;
//         m_quicl4->UdpConnect (transport, this);
        
//         sFlowIdx = m_subflows.size();
//         Ptr<MpQuicSubFlow> sFlow = CreateObject<MpQuicSubFlow> ();
//         sFlow->routeId   = m_subflows[m_subflows.size() - 1]->routeId + 1;
//         sFlow->dAddr    =  m_endPoint->GetLocalAddress ();
//         sFlow->dPort    = m_endPoint->GetLocalPort ();
//         sFlow->sAddr = ipv4;
//         sFlow->sPort = port;
//         m_subflows.insert(m_subflows.end(), sFlow);
//         m_addrIdPair.insert(std::pair<Ipv4Address, uint8_t> (ipv4, sFlowIdx));

//         // Set initial congestion window and Ssthresh for sub flow
//         // m_subflows[1]->m_cWnd = m_tcb->m_initialCWnd;
//         // m_subflows[1]->m_ssThresh = m_tcb->m_initialSsThresh;
//      }
//    else
//      {
//         sFlowIdx = m_addrIdPair[ipv4];

//      }
// // std::cout<<"quicsocketbase.cc map has value:"<<(int)sFlowIdx<<std::endl;
//     return sFlowIdx;
        
// }



}