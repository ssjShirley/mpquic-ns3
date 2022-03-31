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


#include <stdint.h>
#include <iostream>
#include "ns3/buffer.h"
#include "ns3/address-utils.h"
#include "ns3/log.h"
#include "mp-quic-subflow.h"
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

NS_LOG_COMPONENT_DEFINE ("MpQuicSubFlow");
namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (MpQuicSubFlow);


TypeId
MpQuicSubFlow::GetTypeId (void)
{
    static TypeId tid = TypeId ("ns3::MpQuicSubFlow")
        .SetParent (Object::GetTypeId ())

        .AddTraceSource ("CWindow",
                     "The QUIC connection's congestion window",
                     MakeTraceSourceAccessor (&MpQuicSubFlow::m_cWndTrace),
                     "ns3::TracedValueCallback::Uint32")
        // .AddAttribute ("CCType",
        //            "congestion control type",
        //            StringValue ("new"),
        //            MakeStringAccessor (&MpQuicSubFlow::m_ccType),
        //            MakeStringChecker ())
        // .AddAttribute ("delay",
        //            "congestion control type",
        //            DoubleValue (0.04),
        //            MakeDoubleAccessor (&MpQuicSubFlow::m_delay),
        //            MakeDoubleChecker<double> (0))
        // .AddTraceSource ("SubflowCwnd",
        //                "An integer value to trace.",
        //                MakeTraceSourceAccessor (&MpQuicSubFlow::m_cWnd),
        //                "ns3::TracedValueCallback::Int32")
        // .AddTraceSource ("Throughput",
        //                "An integer to trace.",
        //                MakeTraceSourceAccessor (&MpQuicSubFlow::m_throughputBps),
        //                "ns3::TracedValueCallback::double")
        // .AddTraceSource ("RTT",
        //                "An integer to trace.",
        //                MakeTraceSourceAccessor (&MpQuicSubFlow::lastMeasuredRtt),
        //                "ns3::Time::TracedValueCallback")
        ;
      return tid;
}

MpQuicSubFlow::MpQuicSubFlow()
    : m_flowId (0),
      m_lastMaxData(0),
      m_maxDataInterval(10),
      m_rounds(1)
{

    m_numPacketsReceivedSinceLastAckSent = 0;
    m_queue_ack = false;
    m_receivedPacketNumbers = std::vector<SequenceNumber32> ();

    //For congestion control
    m_tcb = CreateObject<QuicSocketState> ();
    m_tcb->m_cWnd = m_tcb->m_initialCWnd;
    m_tcb->m_ssThresh = m_tcb->m_initialSsThresh;
    m_tcb->m_pacingRate = m_tcb->m_maxPacingRate;

    // connect callbacks
    bool ok;
    ok = m_tcb->TraceConnectWithoutContext ("CongestionWindow",
                                            MakeCallback (&MpQuicSubFlow::UpdateCwnd, this));
    NS_ASSERT_MSG (ok == true, "Failed connection to CWND trace");

    // ok = m_tcb->TraceConnectWithoutContext ("SlowStartThreshold",
    //                                         MakeCallback (&QuicSocketBase::UpdateSsThresh, this));
    // NS_ASSERT_MSG (ok == true, "Failed connection to SSTHR trace");

    // ok = m_tcb->TraceConnectWithoutContext ("CongState",
    //                                         MakeCallback (&QuicSocketBase::UpdateCongState, this));
    // NS_ASSERT_MSG (ok == true, "Failed connection to CongState trace");

    // ok = m_tcb->TraceConnectWithoutContext ("NextTxSequence",
    //                                         MakeCallback (&QuicSocketBase::UpdateNextTxSequence, this));
    // NS_ASSERT_MSG (ok == true, "Failed connection to TxSequence trace");

    // ok = m_tcb->TraceConnectWithoutContext ("HighestSequence",
    //                                         MakeCallback (&QuicSocketBase::UpdateHighTxMark, this));
    // NS_ASSERT_MSG (ok == true, "Failed connection to highest sequence trace");
}

// std::vector<uint32_t> MpQuicSubFlow::m_sst = boost::assign::list_of(50000)(50000);

MpQuicSubFlow::~MpQuicSubFlow()
{
    m_flowId     = 0;
}



void
MpQuicSubFlow::SetSegSize (uint32_t size)
{
  NS_LOG_FUNCTION (this << size);

  m_tcb->m_segmentSize = size;
  // Update minimum congestion window
  m_tcb->m_initialCWnd = 2 * size;
  m_tcb->m_kMinimumWindow = 2 * size;
  // m_tcb->m_initialSsThresh = 4 * size;
}

uint32_t
MpQuicSubFlow::GetSegSize (void) const
{
  return m_tcb->m_segmentSize;
}

double
MpQuicSubFlow::GetRate()
{
    if (m_tcb->m_lastRtt.Get().GetSeconds() == 0){
        return 0;
    }
    return m_tcb->m_cWnd/m_tcb->m_segmentSize/m_tcb->m_lastRtt.Get().GetSeconds();
} 

void
MpQuicSubFlow::UpdateCwnd (uint32_t oldValue, uint32_t newValue)
{
  m_cWndTrace (oldValue, newValue);
}


// void
// MpQuicSubFlow::InitialRateEvent () {
//     int x = 2;
//     for (int i=x; i < 6*x+1; i = i+x) {
//         for (int j = 0;j<5;j++) {
//             if (routeId == 0) {
//                 Simulator::Schedule (Seconds (2*i-x+j*0.4-1), &MpQuicSubFlow::UpdateSsh, (10-j*2)*1000000*m_delay/8, 0);
//                 // std::cout<<(2*i-x+j*0.4) <<" 0 "<<(10-j*2)*1000000*0.04/8<<"\n";
//             } else {
//                 Simulator::Schedule (Seconds (2*i-x+j*0.4-1), &MpQuicSubFlow::UpdateSsh, (2+j*2)*1000000*m_delay/8, 1);
//                 // std::cout<<(2*i-x+j*0.4) <<" 1 "<<(2+j*2)*1000000*0.04/8<<"\n";
//             }  
//         }
//         if (routeId == 0) {
//             Simulator::Schedule (Seconds (2*i-1), &MpQuicSubFlow::UpdateSsh, 10*1000000*m_delay/8, 0);
//             //  std::cout<< (2*i) <<" 0 "<<10*1000000*0.04/8<<"\n";
//         } else {
//             Simulator::Schedule (Seconds (2*i-1), &MpQuicSubFlow::UpdateSsh, 10*1000000*m_delay/8, 1);
//             //  std::cout<< (2*i) <<" 1 "<<10*1000000*0.04/8<<"\n";
//         }
//     }

//     // for (int j = 0;j<5;j++) {
//     //   Simulator::Schedule (Seconds (4+j*0.8), &MpQuicSubFlow::UpdateSsh, (10-j*2)*1000000*m_delay/8, 0);
//     //   Simulator::Schedule (Seconds (4+j*0.8), &MpQuicSubFlow::UpdateSsh, (2+j*2)*1000000*m_delay/8, 1);
//     // }
//     // Simulator::Schedule (Seconds (8), &MpQuicSubFlow::UpdateSsh,10*1000000*m_delay/8, 0);
//     // Simulator::Schedule (Seconds (8), &MpQuicSubFlow::UpdateSsh, 10*1000000*m_delay/8, 1);

//     // for (int j = 0;j<5;j++) {
//     //   Simulator::Schedule (Seconds (14+j*0.8), &MpQuicSubFlow::UpdateSsh, (10-j*2)*1000000*m_delay/8, 0);
//     //   Simulator::Schedule (Seconds (14+j*0.8), &MpQuicSubFlow::UpdateSsh, (2+j*2)*1000000*m_delay/8, 1);
//     // }
//     // Simulator::Schedule (Seconds (18), &MpQuicSubFlow::UpdateSsh,10*1000000*m_delay/8, 0);
//     // Simulator::Schedule (Seconds (18), &MpQuicSubFlow::UpdateSsh, 10*1000000*m_delay/8, 1);
    
// }

// void
// MpQuicSubFlow::Add (SequenceNumber32 ack) {
//     m_unackedPackets.insert (m_unackedPackets.end(),MpRttHistory (ack, Simulator::Now ()));
// }


// void
// MpQuicSubFlow::UpdateRtt (SequenceNumber32 ack, Time ackDelay)
// {
//     Time m = Time (0.0);
//     lastMeasuredRttp = lastMeasuredRtt;
//     if (!m_unackedPackets.empty ())
//     {
//         std::vector<MpRttHistory>::iterator it;
//         for (it = m_unackedPackets.begin (); it != m_unackedPackets.end (); ++it)
//         {
//             MpRttHistory item = *it;
//             if (item.seq == ack){
//                 m = Simulator::Now () - item.time; // Elapsed time
//             }
//         }
//     }

     
//     if (!m.IsZero ())
//     {
//         lastMeasuredRtt = m - ackDelay;

//         // m_rtt->Measurement (m);                // Log the measurement
//         // lastMeasuredRtt = m_rtt->GetEstimate ();// - ackDelay;
//         // if (lastMeasuredRtt.Get().GetDouble() < 0){
//         //     lastMeasuredRtt = m_rtt->GetEstimate ();
//         // }
//     }
    
//     // std::cout<<Simulator::Now ().GetSeconds ()<<"flowid "<<routeId<<" seq "<<ack<<" measure: "<<lastMeasuredRtt.Get()<<"\n";
//     m_rttTrace = lastMeasuredRtt;
//     if (lastMeasuredRtt>largestRtt){
//         largestRtt = lastMeasuredRtt;
//     }

//     std::cout<<Simulator::Now ().GetSeconds ()<<"flowid "<<routeId<<" seq "<<ack<<" measure: "<<m <<" measure: "<<lastMeasuredRtt <<"\n";
   
// }


// void
// MpQuicSubFlow::CwndOnAckReceived(double alpha, double sum_rate, double max_rate, std::vector<Ptr<QuicSocketTxItem> > newAcks, uint32_t ackedBytes)
// {
//     if (newAcks.size() == 0){
//         return;
//     }
//     ackSize = newAcks.size();
    // m_lost2 += ackedBytes;
//     m_throughput = ackSize*1460;
//     m_throughputBps = m_throughput*8/lastMeasuredRtt.Get().GetSeconds();
//     // std::cout<<routeId<<": "<<m_throughputBps<<"bps "<<m_throughput<<std::endl;
//     if (m_ccType == "OLIA") 
//     {
//         UpdateCwndOlia(sum_rate,alpha,newAcks);
//     } else {
//         UpdateCwndMmQuic(sum_rate, max_rate, newAcks);
//     }
// }

// void
// MpQuicSubFlow::UpdateCwndOlia(double sum_rate, double alpha, std::vector<Ptr<QuicSocketTxItem> > newAcks)
// {
//     Ptr<QuicSocketTxItem> lastAcked = newAcks.at (0);
//     NS_LOG_LOGIC ("Processing acknowledged packets");
   
//     for (auto it = newAcks.rbegin (); it != newAcks.rend (); ++it)
//     {
//         if (m_cWnd.Get() <= 4*m_segmentSize)
//         {
//             m_cwndState = "Slow_Start";
//         } 
//         if (m_cWnd.Get() >= 16*m_segmentSize)
//         {
           
//             m_cwndState = "Congestion_Avoidance";
//         } 

//         if ((*it)->m_acked)
//         {
//             if (m_cwndState == "Slow_Start")
//             {
//                 m_cWnd = m_cWnd + m_segmentSize;
//             } 
//             else 
//             {
//                 double increase = (m_cWnd/1460/pow(lastMeasuredRtt.Get().GetSeconds(),2))/pow(sum_rate,2)+alpha/(m_cWnd/1460);
//                 m_cWnd = m_cWnd + fabs(increase)*m_segmentSize;
//                 std::cout<<"Congestion sst"<<m_ssThresh<<" cwnd "<<m_cWnd<<"\n";
//             }
//         }
//     }
// }


// void
// MpQuicSubFlow::UpdateCwndMmQuic(double sum_rate, double max_rate, std::vector<Ptr<QuicSocketTxItem> > newAcks)
// {
//     if (m_cWnd.Get() > 60000)
//     {
//         return;
//     }
//     for (auto it = newAcks.rbegin (); it != newAcks.rend (); ++it)
//     {
//         if (m_cWnd.Get() <= 4*m_segmentSize)
//         {
//             m_cwndState = "Slow_Start";
//         } 
//         if (m_cWnd.Get() >= m_sst[routeId])
//         {
//             // std::cout<<Simulator::Now ()<<" path: "<<routeId<<" avod"<<m_sst[routeId]<<" cwnd "<<m_cWnd<<"\n";
//             m_cwndState = "Congestion_Avoidance";
//         } 

//         if ((*it)->m_acked)
//         {
//             if (m_cwndState == "Slow_Start")
//             {
//                 // std::cout<<Simulator::Now ()<<" path: "<<routeId<<" Slow sst"<<m_sst[routeId]<<" cwnd "<<m_cWnd<<"\n";
//                 m_cWnd = m_cWnd + m_segmentSize;
//             } 
//             else 
//             {
//                 // std::cout<<Simulator::Now ()<<" path: "<<routeId<<" Congestion sst"<<m_sst[routeId]<<" cwnd "<<m_cWnd<<"\n";
//                 double increase = 3*max_rate/(2.0*lastMeasuredRtt.Get().GetSeconds()*pow(sum_rate,2.5));
//                 // std::cout<<"increase: "<<increase<<std::endl;
//                 m_cWnd = m_cWnd + fabs(increase)*m_segmentSize;
//             }

//         }
//     }
// } 

// void 
// MpQuicSubFlow::UpdateSsh(uint32_t ssh, int id)
// {
//      m_sst[id] = ssh;
//     //  std::cout<<Simulator::Now ()<<"sstsst"<<ssh<<"\n";
// }

// void
// MpQuicSubFlow::UpdateSsThresh(double snr, uint32_t ssh)
// {
//     // m_ssThresh = 25000;
//     m_ssThresh = ssh;

   
// }

// void
// MpQuicSubFlow::UpdateCwndOnPacketLost()
// {
//     m_lost1 = m_lost2;
//     m_lost2 = 0;
// }




// uint32_t
// MpQuicSubFlow::GetMinPrevLossCwnd()
// {
//     return std::min(m_lossCwnd,std::min(m_cWnd.Get(),m_sst[routeId]));
// }

// void
// MpQuicSubFlow::SetInitialCwnd(uint32_t cwnd)
// {
//     if (m_ccType == "OLIA") 
//     {
//         m_cWnd = 2*m_segmentSize;
//     }
//     else{
//         m_cWnd = 4*m_segmentSize;//cwnd
//     }
// } 


} // namespace ns3
