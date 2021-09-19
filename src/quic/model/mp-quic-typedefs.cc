#include <stdint.h>
#include <iostream>
#include "ns3/buffer.h"
#include "ns3/address-utils.h"
#include "ns3/log.h"
#include "mp-quic-typedefs.h"
#include <stdlib.h>
#include <queue>
#include "ns3/traced-value.h"
#include "ns3/trace-source-accessor.h"
#include <ns3/object-base.h>
#include "ns3/simulator.h"
#include "time.h"
#include "ns3/string.h"

#include "quic-socket-tx-scheduler.h"
#include "quic-socket-base.h"

#include <algorithm>
#include <math.h>
#include <cmath>
#include <boost/assign/list_of.hpp>

NS_LOG_COMPONENT_DEFINE ("MpQuicTypeDefs");
namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (MpQuicSubFlow);


TypeId
MpQuicSubFlow::GetTypeId (void)
{
    static TypeId tid = TypeId ("ns3::MpQuicSubFlow")
        .SetParent (Object::GetTypeId ())
        .AddAttribute ("CCType",
                   "congestion control type",
                   StringValue ("new"),
                   MakeStringAccessor (&MpQuicSubFlow::m_ccType),
                   MakeStringChecker ())
        .AddAttribute ("delay",
                   "congestion control type",
                   DoubleValue (0.04),
                   MakeDoubleAccessor (&MpQuicSubFlow::m_delay),
                   MakeDoubleChecker<double> (0))
        .AddTraceSource ("SubflowCwnd",
                       "An integer value to trace.",
                       MakeTraceSourceAccessor (&MpQuicSubFlow::m_cWnd),
                       "ns3::TracedValueCallback::Int32")
        .AddTraceSource ("Throughput",
                       "An integer to trace.",
                       MakeTraceSourceAccessor (&MpQuicSubFlow::m_throughputBps),
                       "ns3::TracedValueCallback::double")
        .AddTraceSource ("RTT",
                       "An integer to trace.",
                       MakeTraceSourceAccessor (&MpQuicSubFlow::lastMeasuredRtt),
                       "ns3::Time::TracedValueCallback")
        ;
      return tid;
}

MpQuicSubFlow::MpQuicSubFlow()
    : routeId (0),
      sAddr (Ipv4Address::GetZero ()), sPort (0),
      dAddr (Ipv4Address::GetZero ()), dPort (0)
{
    m_bandwidth   = 12500*0.5;
    m_cWnd        = 5840;                  // congestion window is initialized to one segment
    // m_Bmin = 0;
    m_segmentSize = 1460;
    m_ssThresh    = 25000;              // initial value for a Quic connexion
    largestRtt = Seconds(0);
    m_rtt = new RttMeanDeviation ();
    m_nextPktNum = SequenceNumber32(0);
    m_receivedSeqNumbers = std::vector<SequenceNumber32> ();
    m_unackedPackets = std::vector<MpRttHistory> ();
    m_lost1 = 0;
    m_lost2 = 0;
    m_cwndState = "Slow_Start";
    m_lossCwnd = 12500*0.5;
    m_bwEst = 0;
    ackSize = 0;
}

std::vector<uint32_t> MpQuicSubFlow::m_sst = boost::assign::list_of(50000)(50000);

MpQuicSubFlow::~MpQuicSubFlow()
{
    routeId     = 0;
    sAddr       = Ipv4Address::GetZero ();
    m_bandwidth   = 12500*0.5;
    m_cWnd        = 1460;
    // m_Bmin = 0;
}


void
MpQuicSubFlow::Add (SequenceNumber32 ack) {
    m_unackedPackets.insert (m_unackedPackets.end(),MpRttHistory (ack, Simulator::Now ()));
    // std::cout<<"\nsize "<<m_unackedPackets.size()<<"\n";
}


void
MpQuicSubFlow::UpdateRtt (SequenceNumber32 ack, Time ackDelay)
{
    Time m = Time (0.0);
    lastMeasuredRttp = lastMeasuredRtt;
    if (!m_unackedPackets.empty ())
    {
        std::vector<MpRttHistory>::iterator it;
        for (it = m_unackedPackets.begin (); it != m_unackedPackets.end (); ++it)
        {
            MpRttHistory item = *it;
            if (item.seq == ack){
                m = Simulator::Now () - item.time; // Elapsed time
            }
        }
    }

     
    if (!m.IsZero ())
    {
        lastMeasuredRtt = m - ackDelay;

        // m_rtt->Measurement (m);                // Log the measurement
        // lastMeasuredRtt = m_rtt->GetEstimate ();// - ackDelay;
        // if (lastMeasuredRtt.Get().GetDouble() < 0){
        //     lastMeasuredRtt = m_rtt->GetEstimate ();
        // }
    }
    
    // std::cout<<Simulator::Now ().GetSeconds ()<<"flowid "<<routeId<<" seq "<<ack<<" measure: "<<lastMeasuredRtt.Get()<<"\n";
    m_rttTrace = lastMeasuredRtt;
    if (lastMeasuredRtt>largestRtt){
        largestRtt = lastMeasuredRtt;
    }

    // std::cout<<Simulator::Now ().GetSeconds ()<<"flowid "<<routeId<<" seq "<<ack<<" measure: "<<m <<" measure: "<<lastMeasuredRtt <<"\n";
   
}


void
MpQuicSubFlow::CwndOnAckReceived(double alpha, double sum_rate, double max_rate, std::vector<Ptr<QuicSocketTxItem> > newAcks, uint32_t ackedBytes)
{
    if (newAcks.size() == 0){
        return;
    }
    ackSize = newAcks.size();
    m_lost2 += ackedBytes;
    m_throughput = ackSize*1460;
    m_throughputBps = m_throughput*8/lastMeasuredRtt.Get().GetSeconds();
    // std::cout<<routeId<<": "<<m_throughputBps<<"bps "<<m_throughput<<std::endl;
    if (m_ccType == "OLIA") 
    {
        UpdateCwndOlia(sum_rate,alpha,newAcks);
    } 
}

void
MpQuicSubFlow::UpdateCwndOlia(double sum_rate, double alpha, std::vector<Ptr<QuicSocketTxItem> > newAcks)
{
    Ptr<QuicSocketTxItem> lastAcked = newAcks.at (0);
    NS_LOG_LOGIC ("Processing acknowledged packets");
   
    for (auto it = newAcks.rbegin (); it != newAcks.rend (); ++it)
    {
        if (m_cWnd.Get() <= 4*m_segmentSize)
        {
            m_cwndState = "Slow_Start";
        } 
        if (m_cWnd.Get() >= 16*m_segmentSize)
        {
            // std::cout<<"Congestion sst"<<m_ssThresh<<" cwnd "<<m_cWnd<<"\n";
            m_cwndState = "Congestion_Avoidance";
        } 

        if ((*it)->m_acked)
        {
            if (m_cwndState == "Slow_Start")
            {
                m_cWnd = m_cWnd + m_segmentSize;
            } 
            else 
            {
                double increase = (m_cWnd/1460/pow(lastMeasuredRtt.Get().GetSeconds(),2))/pow(sum_rate,2)+alpha/(m_cWnd/1460);
                m_cWnd = m_cWnd + fabs(increase)*m_segmentSize;
            }
        }
    }
}



void 
MpQuicSubFlow::UpdateSsh(uint32_t ssh, int id)
{
     m_sst[id] = ssh;
     std::cout<<Simulator::Now ()<<"sstsst"<<ssh<<"\n";
}

void
MpQuicSubFlow::UpdateSsThresh(double snr, uint32_t ssh)
{
    // m_ssThresh = 25000;
    m_ssThresh = ssh;

   
}

void
MpQuicSubFlow::UpdateCwndOnPacketLost()
{
    
    m_lost1 = m_lost2;
    m_lost2 = 0;
    if (m_ccType == "OLIA") 
    {
        m_cWnd = m_cWnd/2;
    } 
    else 
    {
        if(m_throughput < m_sst[routeId] and lastMeasuredRtt.Get().GetSeconds() > m_delay)
        {
            m_cWnd = m_cWnd/2;
            m_cwndState = "Congestion_Avoidance";
            
        }
    }
    
     
    m_lossCwnd = std::min(m_lossCwnd, m_cWnd.Get());
    // m_cWnd = m_cWnd/2;
    
}


double
MpQuicSubFlow::GetRate()
{
    if (lastMeasuredRtt.Get().GetSeconds() == 0){
        return 0;
    }
    return m_cWnd/1460/(lastMeasuredRtt.Get().GetSeconds());
} 


void
MpQuicSubFlow::SetInitialCwnd(uint32_t cwnd)
{

    m_cWnd = cwnd;

} 



MpQuicAddressInfo::MpQuicAddressInfo()
    : addrID (0), ipv4Addr (Ipv4Address::GetZero ()), mask (Ipv4Mask::GetZero())
{
}

MpQuicAddressInfo::~MpQuicAddressInfo()
{
    addrID = 0;
    ipv4Addr = Ipv4Address::GetZero ();
}

//RttHistory methods
MpRttHistory::MpRttHistory (SequenceNumber32 s, Time t)
  : seq (s),
    time (t),
    retx (false)
{
}


} // namespace ns3
