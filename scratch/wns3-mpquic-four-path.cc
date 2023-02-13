/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2019 SIGNET Lab, Department of Information Engineering, University of Padova
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
 *     n0 ----------- n2
 *    /        P0       \
 *   / n1 ----------- n8 \
 *  / /        P1       \ \
 * n4                     n5
 *  \ \                 / /
 *   \ n6 ----------- n9 /
 *    \        P2       /
 *     n3 ----------- n7
 *             P3
 * 
 * 
 * Authors: Alvise De Biasio <alvise.debiasio@gmail.com>
 *          Federico Chiariotti <whatever@blbl.it>
 *          Michele Polese <michele.polese@gmail.com>
 *          Davide Marcato <davidemarcato@outlook.com>
 *          Shengjie Shu <shengjies@uvic.ca>
 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/quic-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/config-store-module.h"
#include "ns3/random-variable-stream.h"
#include <iostream>
#include "ns3/flow-monitor-module.h"
#include "ns3/gnuplot.h"

#include <boost/assign/list_of.hpp>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("wns3-mpquic-four-path");

// static void
// CwndChange (Ptr<OutputStreamWrapper> stream, uint32_t oldCwnd, uint32_t newCwnd)
// {
//     *stream->GetStream () << Simulator::Now ().GetSeconds () << "\t" << oldCwnd << "\t" << newCwnd << std::endl;
// }

// static void
// RewardChange (Ptr<OutputStreamWrapper> stream1, uint32_t oldReward, uint32_t newReward)
// {
//     *stream1->GetStream () << Simulator::Now ().GetSeconds () << "\t" << oldReward << "\t" << newReward << std::endl;
// }

// static void
// RTTChange (Ptr<OutputStreamWrapper> stream, Time oldValue, Time newValue)
// {
//     *stream->GetStream () << Simulator::Now ().GetSeconds () << "\t" << oldValue.GetSeconds() << "\t" << newValue.GetSeconds()<< std::endl;
// }

// static void
// Traces(uint32_t serverId, std::string pathVersion, std::string finalPart)
// {
//     AsciiTraceHelper asciiTraceHelper;

//     std::ostringstream path0CW;
//     path0CW << "/NodeList/" << serverId << "/$ns3::QuicL4Protocol/SocketList/0/QuicSocketBase/CongestionWindow";
//     NS_LOG_INFO("Matches cw " << Config::LookupMatches(path0CW.str().c_str()).GetN());

//     std::ostringstream path1CW;
//     path1CW << "/NodeList/" << serverId << "/$ns3::QuicL4Protocol/SocketList/0/QuicSocketBase/CongestionWindow1";
//     NS_LOG_INFO("Matches cw " << Config::LookupMatches(path1CW.str().c_str()).GetN());

//     std::ostringstream path0rtt;
//     path0rtt << "/NodeList/" << serverId << "/$ns3::QuicL4Protocol/SocketList/0/QuicSocketBase/RTT0";
//     NS_LOG_INFO("Matches cw " << Config::LookupMatches(path0rtt.str().c_str()).GetN());

//     std::ostringstream path1rtt;
//     path1rtt << "/NodeList/" << serverId << "/$ns3::QuicL4Protocol/SocketList/0/QuicSocketBase/RTT1";
//     NS_LOG_INFO("Matches cw " << Config::LookupMatches(path1rtt.str().c_str()).GetN());

//     std::ostringstream reward;
//     reward << "/NodeList/" << serverId << "/$ns3::QuicL4Protocol/SocketList/0/QuicSocketBase/MabRewardTrace";
//     // reward << "/NodeList/" << serverId << "/$ns3::QuicL5Protocol/StreamList/*/QuicStreamBase/RxBufferTrace";
//     NS_LOG_INFO("Matches cw " << Config::LookupMatches(reward.str().c_str()).GetN());

//     std::ostringstream file0CW;
//     file0CW << pathVersion << "-cwnd-change-0" << "" << finalPart;
//     std::ostringstream file1CW;
//     file1CW << pathVersion << "-cwnd-change-1"<< "" << finalPart;
//     std::ostringstream file0rtt;
//     file0rtt << pathVersion << "-rtt-change-0" << "" << finalPart;
//     std::ostringstream file1rtt;
//     file1rtt << pathVersion << "-rtt-change-1"<< "" << finalPart;
//     std::ostringstream fileReward;
//     fileReward << pathVersion << "-reward"<< "" << finalPart;


//     Ptr<OutputStreamWrapper> stream = asciiTraceHelper.CreateFileStream (file0CW.str ().c_str ());
//     Config::ConnectWithoutContext (path0CW.str ().c_str (), MakeBoundCallback(&CwndChange, stream));

//     Ptr<OutputStreamWrapper> stream0 = asciiTraceHelper.CreateFileStream (file1CW.str ().c_str ());
//     Config::ConnectWithoutContext (path1CW.str ().c_str (), MakeBoundCallback(&CwndChange, stream0));

//     Ptr<OutputStreamWrapper> stream1 = asciiTraceHelper.CreateFileStream (fileReward.str ().c_str ());
//     Config::ConnectWithoutContext (reward.str ().c_str (), MakeBoundCallback(&CwndChange, stream1));

//     Ptr<OutputStreamWrapper> stream2 = asciiTraceHelper.CreateFileStream (file0rtt.str ().c_str ());
//     Config::ConnectWithoutContext (path0rtt.str ().c_str (), MakeBoundCallback(&RTTChange, stream2));

//     Ptr<OutputStreamWrapper> stream3 = asciiTraceHelper.CreateFileStream (file1rtt.str ().c_str ());
//     Config::ConnectWithoutContext (path1rtt.str ().c_str (), MakeBoundCallback(&RTTChange, stream3));

// }

// static void
// TracesServer(uint32_t serverId, std::string pathVersion, std::string finalPart)
// {
//     AsciiTraceHelper asciiTraceHelper;
 
//     std::ostringstream queueSize;
//     queueSize << "/NodeList/" << serverId << "/$ns3::QuicL5Protocol/StreamList/1/QuicStreamBase/RxBufferTrace";
//     NS_LOG_INFO("Matches cw " << Config::LookupMatches(queueSize.str().c_str()).GetN());

//     std::ostringstream file0CW;
//     file0CW << pathVersion << "QUIC-stram-rxbuffer" << "" << finalPart;

//     Ptr<OutputStreamWrapper> stream = asciiTraceHelper.CreateFileStream (file0CW.str ().c_str ());
//     Config::ConnectWithoutContext (queueSize.str ().c_str (), MakeBoundCallback(&CwndChange, stream));

// }

void ThroughputMonitor (FlowMonitorHelper *fmhelper, Ptr<FlowMonitor> flowMon, Ptr<OutputStreamWrapper> stream)
{
    std::map<FlowId, FlowMonitor::FlowStats> flowStats = flowMon->GetFlowStats();
    Ptr<Ipv4FlowClassifier> classing = DynamicCast<Ipv4FlowClassifier> (fmhelper->GetClassifier());
    for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator stats = flowStats.begin (); stats != flowStats.end (); ++stats)
    {
        // Ipv4FlowClassifier::FiveTuple fiveTuple = classing->FindFlow (stats->first);
        if (stats->first == 1 || (stats->first >= 3 && stats->first <= 5)){
            // std::cout << " Flow: " << stats->first  << " Seconds: " << stats->second.timeLastRxPacket.GetSeconds()  << " Tx Bytes: " << stats->second.txBytes << " Received Bytes: " << stats->second.rxBytes << " First seconds: " << stats->second.timeFirstTxPacket.GetSeconds() << std::endl;
            *stream->GetStream () << stats->first  << "\t" << stats->second.timeLastRxPacket.GetSeconds() << "\t" << stats->second.rxBytes << "\t" << stats->second.rxPackets << "\t" << stats->second.lastDelay.GetMilliSeconds() << "\t" << stats->second.rxBytes*8/1024/1024/(stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstRxPacket.GetSeconds())  << std::endl;
        }
    }
    Simulator::Schedule(Seconds(0.001),&ThroughputMonitor, fmhelper, flowMon, stream);
}


void
ModifyLinkRate(NetDeviceContainer *ptp, DataRate lr, Time delay) {
    StaticCast<PointToPointNetDevice>(ptp->Get(0))->SetDataRate(lr);
    StaticCast<PointToPointChannel>(StaticCast<PointToPointNetDevice>(ptp->Get(0))->GetChannel())->SetAttribute("Delay", TimeValue(delay));
}


int
main (int argc, char *argv[])
{
    int schedulerType = MpQuicScheduler::ROUND_ROBIN;
    double rate0 = 10.0;
    double rate1 = 10.0;
    double delay0 = 50.0;
    double delay1 = 50.0;
    string myRandomNo = "500000";
    string lossrate = "0.0000";
    int bVar = 2;
    int bLambda = 100;
    int mrate = 52428800;
    int ccType = QuicSocketBase::OLIA;
    int mselect = 3;
    int seed = 1;
    TypeId ccTypeId = MpQuicCongestionOps::GetTypeId ();
    CommandLine cmd;


    cmd.AddValue ("SchedulerType", "in use scheduler type (0 - ROUND_ROBIN, 1 - MIN_RTT, 2 - BLEST, 3 - ECF, 4 - Peekaboo)", schedulerType);
    cmd.AddValue ("BVar", "e.g. 100", bVar);
    cmd.AddValue ("BLambda", "e.g. 100", bLambda);
    cmd.AddValue ("MabRate", "e.g. 100", mrate);
    cmd.AddValue ("Rate0", "e.g. 5Mbps", rate0);
    cmd.AddValue ("Rate1", "e.g. 50Mbps", rate1);
    cmd.AddValue ("Delay0", "e.g. 80ms", delay0);
    cmd.AddValue ("Delay1", "e.g. 20ms", delay1);
    cmd.AddValue ("Size", "e.g. 80", myRandomNo);
    cmd.AddValue ("Seed", "e.g. 80", seed);
    cmd.AddValue ("LossRate", "e.g. 0.0001", lossrate);
    cmd.AddValue ("Select", "e.g. 0.0001", mselect);
    cmd.AddValue ("CcType", "in use congestion control type (0 - QuicNewReno, 1 - OLIA)", ccType);
    cmd.Parse (argc, argv);

    NS_LOG_INFO("\n\n#################### SIMULATION SET-UP ####################\n\n\n");
    
    LogLevel log_precision = LOG_LEVEL_LOGIC;
    Time::SetResolution (Time::NS);
    LogComponentEnableAll (LOG_PREFIX_TIME);
    LogComponentEnableAll (LOG_PREFIX_FUNC);
    LogComponentEnableAll (LOG_PREFIX_NODE);
//  LogComponentEnable ("QuicEchoClientApplication", log_precision);
//  LogComponentEnable ("QuicEchoServerApplication", log_precision);
    LogComponentEnable ("wns3-mpquic-four-path", log_precision);
//  LogComponentEnable ("QuicHeader", log_precision);
//  LogComponentEnable ("QuicSocketBase", log_precision);
//  LogComponentEnable ("QuicStreamBase", LOG_LEVEL_LOGIC);
//  LogComponentEnable ("Socket", log_precision);
//  LogComponentEnable ("Application", log_precision);
//  LogComponentEnable ("Node", log_precision);
//  LogComponentEnable ("InternetStackHelper", log_precision);
//  LogComponentEnable ("QuicSocketFactory", log_precision);
//  LogComponentEnable ("ObjectFactory", log_precision);
//  LogComponentEnable ("TypeId", log_precision);
//  LogComponentEnable ("QuicL4Protocol", log_precision);
//  LogComponentEnable ("QuicL5Protocol", log_precision);
//  LogComponentEnable ("ObjectBase", log_precision);
//  LogComponentEnable ("QuicEchoHelper", log_precision);
//  LogComponentEnable ("QuicSocketTxScheduler", log_precision);
//  LogComponentEnable ("QuicSocketRxBuffer", log_precision);
//  LogComponentEnable ("QuicHeader", log_precision);
//  LogComponentEnable ("QuicSubheader", log_precision);
//  LogComponentEnable ("Header", log_precision);
//  LogComponentEnable ("PacketMetadata", log_precision);
    RngSeedManager::SetSeed (seed); 

    if (ccType == QuicSocketBase::OLIA){
        ccTypeId = MpQuicCongestionOps::GetTypeId ();
    }
    if(ccType == QuicSocketBase::QuicNewReno){
        ccTypeId = QuicCongestionOps::GetTypeId ();
    }

    Config::SetDefault ("ns3::QuicSocketBase::SocketSndBufSize",UintegerValue (40000000));
    Config::SetDefault ("ns3::QuicStreamBase::StreamSndBufSize",UintegerValue (40000000));
    Config::SetDefault ("ns3::QuicSocketBase::SocketRcvBufSize",UintegerValue (40000000));
    Config::SetDefault ("ns3::QuicStreamBase::StreamRcvBufSize",UintegerValue (40000000));


    Config::SetDefault ("ns3::QuicSocketBase::EnableMultipath",BooleanValue(true));
    Config::SetDefault ("ns3::QuicSocketBase::CcType",IntegerValue(ccType));
    Config::SetDefault ("ns3::QuicL4Protocol::SocketType",TypeIdValue (ccTypeId));
    Config::SetDefault ("ns3::MpQuicScheduler::SchedulerType", IntegerValue(schedulerType));   
    Config::SetDefault ("ns3::MpQuicScheduler::BlestVar", UintegerValue(bVar));   
    Config::SetDefault ("ns3::MpQuicScheduler::BlestLambda", UintegerValue(bLambda));   
    Config::SetDefault ("ns3::MpQuicScheduler::MabRate", UintegerValue(mrate)); 
    Config::SetDefault ("ns3::MpQuicScheduler::Select", UintegerValue(mselect)); 

    
    Ptr<RateErrorModel> em = CreateObjectWithAttributes<RateErrorModel> (
    "RanVar", StringValue ("ns3::UniformRandomVariable[Min=0.0|Max=1.0]"),
    "ErrorRate", DoubleValue (stod(lossrate)));


    Ptr<UniformRandomVariable> rateVal0 = CreateObject<UniformRandomVariable> ();
    rateVal0->SetAttribute ("Min", DoubleValue (rate0));
    rateVal0->SetAttribute ("Max", DoubleValue (rate1));

    Ptr<UniformRandomVariable> delayVal0 = CreateObject<UniformRandomVariable> ();
    delayVal0->SetAttribute ("Min", DoubleValue (delay0));
    delayVal0->SetAttribute ("Max", DoubleValue (delay1));

    int simulationEndTime = 20;
    int start_time = 1;

    // Ptr<UniformRandomVariable> x = CreateObject<UniformRandomVariable> ();
    // uint32_t myRandomNo = x->GetInteger (80,90);
    // uint32_t maxBytes = stoi(myRandomNo) * 104857.6;
    uint32_t maxBytes = stoi(myRandomNo);
    // uint32_t maxBytes = 10000;
    
    NS_LOG_INFO ("Create nodes.");
    NodeContainer c;
    c.Create (10);
    NodeContainer n4n0 = NodeContainer (c.Get (4), c.Get (0));
    NodeContainer n0n2 = NodeContainer (c.Get (0), c.Get (2));
    NodeContainer n2n5 = NodeContainer (c.Get (2), c.Get (5));

    NodeContainer n1n8 = NodeContainer (c.Get (1), c.Get (8));
    
    NodeContainer n4n3 = NodeContainer (c.Get (4), c.Get (3));
    NodeContainer n3n7 = NodeContainer (c.Get (3), c.Get (7));
    NodeContainer n7n5 = NodeContainer (c.Get (7), c.Get (5));

    NodeContainer n6n9 = NodeContainer (c.Get (6), c.Get (9));

    NodeContainer n4n1 = NodeContainer (c.Get (4), c.Get (1));
    NodeContainer n8n5 = NodeContainer (c.Get (8), c.Get (5));

    NodeContainer n4n6 = NodeContainer (c.Get (4), c.Get (6));
    NodeContainer n9n5 = NodeContainer (c.Get (9), c.Get (5));


 
    InternetStackHelper internet;
    internet.Install (c.Get (0));
    internet.Install (c.Get (1));
    internet.Install (c.Get (2));
    internet.Install (c.Get (3));
    internet.Install (c.Get (6));
    internet.Install (c.Get (7));
    internet.Install (c.Get (8));
    internet.Install (c.Get (9));

    QuicHelper stack;
    stack.InstallQuic (c.Get (4));
    stack.InstallQuic (c.Get (5));


    // We create the channels first without any IP addressing information
    NS_LOG_INFO ("Create channels.");
    PointToPointHelper p2p;
    p2p.SetDeviceAttribute ("DataRate", StringValue (std::to_string(rateVal0->GetValue())+"Mbps"));
    p2p.SetChannelAttribute ("Delay", StringValue (std::to_string(delayVal0->GetValue())+"ms"));
    NetDeviceContainer d1d8 = p2p.Install (n1n8);
    d1d8.Get (1)->SetAttribute ("ReceiveErrorModel", PointerValue (em));
    NetDeviceContainer d0d2 = p2p.Install (n0n2);
    d0d2.Get (1)->SetAttribute ("ReceiveErrorModel", PointerValue (em));

    // p2p.SetDeviceAttribute ("DataRate", StringValue (rate1));
    // p2p.SetChannelAttribute ("Delay", StringValue (delay1));
    NetDeviceContainer d6d9 = p2p.Install (n6n9);
    d6d9.Get (1)->SetAttribute ("ReceiveErrorModel", PointerValue (em));
    NetDeviceContainer d3d7 = p2p.Install (n3n7);
    d3d7.Get (1)->SetAttribute ("ReceiveErrorModel", PointerValue (em));



    //background traffic
    p2p.SetDeviceAttribute ("DataRate", StringValue ("100Mbps"));
    p2p.SetChannelAttribute ("Delay", StringValue ("0ms"));
    NetDeviceContainer d4d1 = p2p.Install (n4n1);
    NetDeviceContainer d4d0 = p2p.Install (n4n0);
    NetDeviceContainer d8d5 = p2p.Install (n8n5);
    NetDeviceContainer d4d6 = p2p.Install (n4n6);
    NetDeviceContainer d9d5 = p2p.Install (n9n5);
    NetDeviceContainer d2d5 = p2p.Install (n2n5);
    NetDeviceContainer d4d3 = p2p.Install (n4n3);
    NetDeviceContainer d7d5 = p2p.Install (n7n5);
    
    // Later, we add IP addresses.
    NS_LOG_INFO ("Assign IP Addresses.");
    Ipv4AddressHelper ipv4;
    ipv4.SetBase ("10.1.4.0", "255.255.255.0");
    Ipv4InterfaceContainer i4i1 = ipv4.Assign (d4d1);

    ipv4.SetBase ("10.1.9.0", "255.255.255.0");
    Ipv4InterfaceContainer i1i8 = ipv4.Assign (d1d8);

    ipv4.SetBase ("10.1.5.0", "255.255.255.0");
    Ipv4InterfaceContainer i8i5 = ipv4.Assign (d8d5);
    
    ipv4.SetBase ("10.1.6.0", "255.255.255.0");
    Ipv4InterfaceContainer i4i6 = ipv4.Assign (d4d6);

    ipv4.SetBase ("10.1.10.0", "255.255.255.0");
    Ipv4InterfaceContainer i6i9 = ipv4.Assign (d6d9);

    ipv4.SetBase ("10.1.7.0", "255.255.255.0");
    Ipv4InterfaceContainer i9i5 = ipv4.Assign (d9d5);
    

    //background
    ipv4.SetBase ("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer i4i0 = ipv4.Assign (d4d0);

    ipv4.SetBase ("10.1.2.0", "255.255.255.0");
    Ipv4InterfaceContainer i0i2 = ipv4.Assign (d0d2);
    
    ipv4.SetBase ("10.1.3.0", "255.255.255.0");
    Ipv4InterfaceContainer i2i5 = ipv4.Assign (d2d5);

    ipv4.SetBase ("10.1.8.0", "255.255.255.0");
    Ipv4InterfaceContainer i4i3 = ipv4.Assign (d4d3);

    ipv4.SetBase ("10.1.11.0", "255.255.255.0");
    Ipv4InterfaceContainer i3i7 = ipv4.Assign (d3d7);

    ipv4.SetBase ("10.1.12.0", "255.255.255.0");
    Ipv4InterfaceContainer i7i5 = ipv4.Assign (d7d5);


    Ptr<Ipv4> ipv4_n4 = c.Get(4)->GetObject<Ipv4> ();
    Ipv4StaticRoutingHelper ipv4RoutingHelper; 
    Ptr<Ipv4StaticRouting> staticRouting_n4 = ipv4RoutingHelper.GetStaticRouting (ipv4_n4); 
    staticRouting_n4->AddHostRouteTo (Ipv4Address ("10.1.5.2"), Ipv4Address ("10.1.9.2") ,1); 
    staticRouting_n4->AddHostRouteTo (Ipv4Address ("10.1.7.2"), Ipv4Address ("10.1.10.2") ,2); 
    staticRouting_n4->AddHostRouteTo (Ipv4Address ("10.1.3.2"), Ipv4Address ("10.1.2.2") ,3); 
    staticRouting_n4->AddHostRouteTo (Ipv4Address ("10.1.11.2"), Ipv4Address ("10.1.12.2") ,4); 

    Ptr<Ipv4> ipv4_n5 = c.Get(5)->GetObject<Ipv4> ();
    Ptr<Ipv4StaticRouting> staticRouting_n5 = ipv4RoutingHelper.GetStaticRouting (ipv4_n5); 
    staticRouting_n5->AddHostRouteTo (Ipv4Address ("10.1.4.1"), Ipv4Address ("10.1.9.1") ,1); 
    staticRouting_n5->AddHostRouteTo (Ipv4Address ("10.1.6.1"), Ipv4Address ("10.1.10.1") ,2); 
    staticRouting_n5->AddHostRouteTo (Ipv4Address ("10.1.1.1"), Ipv4Address ("10.1.2.1") ,3); 
    staticRouting_n5->AddHostRouteTo (Ipv4Address ("10.1.8.1"), Ipv4Address ("10.1.11.1") ,4); 

    // Create router nodes, initialize routing database and set up the routing
    // tables in the nodes.
    Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
    
    // //background 1
    // uint16_t port0 = 11;  // well-known echo port number
    // BulkSendHelper source0 ("ns3::TcpSocketFactory",
    //                     InetSocketAddress (i1i2.GetAddress (1), port0));
    // // Set the amount of data to send in bytes.  Zero is unlimited.
    // source0.SetAttribute ("MaxBytes", UintegerValue (0));
    // ApplicationContainer sourceApps0 = source0.Install (c.Get (0));

    // for (int i = 1; i < simulationEndTime; i++) {
    //     sourceApps0.Start (Seconds (i));
    //     sourceApps0.Stop (Seconds (i+0.3));
    //     sourceApps0.Start (Seconds (i+0.6));
    //     sourceApps0.Stop (Seconds (i+0.9));
    // }
    
    // PacketSinkHelper sink0 ("ns3::TcpSocketFactory",
    //                     InetSocketAddress (Ipv4Address::GetAny (), port0));
    // ApplicationContainer sinkApps0 = sink0.Install (c.Get (2));
    // sinkApps0.Start (Seconds (0.0));
    // sinkApps0.Stop (Seconds(simulationEndTime));

 
    // //background2
    // uint16_t port1 = 13;  // well-known echo port number
    // // uint32_t maxBytes1 = 0;
    // BulkSendHelper source1 ("ns3::TcpSocketFactory",
    //                         InetSocketAddress (i6i7.GetAddress (1), port1));
    // // Set the amount of data to send in bytes.  Zero is unlimited.
    // source1.SetAttribute ("MaxBytes", UintegerValue (0));
    // ApplicationContainer sourceApps1 = source1.Install (c.Get (3));
    // for (int i = 1; i < simulationEndTime; i++) {
    //     sourceApps1.Start (Seconds (i+0.1));
    //     sourceApps1.Stop (Seconds (i+0.4));
    //     sourceApps1.Start (Seconds (i+0.7));
    //     sourceApps1.Stop (Seconds (i+1));
    // }
    // PacketSinkHelper sink1 ("ns3::TcpSocketFactory",
    //                         InetSocketAddress (Ipv4Address::GetAny (), port1));
    // ApplicationContainer sinkApps1 = sink1.Install (c.Get (7));
    // sinkApps1.Start (Seconds (0.0));
    // sinkApps1.Stop (Seconds(simulationEndTime));

  
    uint16_t port2 = 9;  // well-known echo port number
    
    MpquicBulkSendHelper source ("ns3::QuicSocketFactory",
                            InetSocketAddress (i8i5.GetAddress (1), port2));
    // Set the amount of data to send in bytes.  Zero is unlimited.
    source.SetAttribute ("MaxBytes", UintegerValue (maxBytes));
    ApplicationContainer sourceApps = source.Install (c.Get (4));
    sourceApps.Start (Seconds (start_time));
    sourceApps.Stop (Seconds(simulationEndTime));

    //
    // Create a PacketSinkApplication and install it on node 1
    //
    PacketSinkHelper sink2 ("ns3::QuicSocketFactory",
                            InetSocketAddress (Ipv4Address::GetAny (), port2));
    ApplicationContainer sinkApps2 = sink2.Install (c.Get (5));
    sinkApps2.Start (Seconds (0.0));
    sinkApps2.Stop (Seconds(simulationEndTime));


    // std::ostringstream file;
    // file<<"./scheduler" << schedulerType;
    // Simulator::Schedule (Seconds (start_time+0.0000001), &Traces, c.Get (4)->GetId(),
    // file.str (), ".txt");

    AsciiTraceHelper asciiTraceHelper;
    std::ostringstream fileName;
    fileName <<  "./scheduler" << schedulerType << "-rx" << ".txt";
    Ptr<OutputStreamWrapper> stream = asciiTraceHelper.CreateFileStream (fileName.str ());
  

    FlowMonitorHelper flowmon;
    Ptr<FlowMonitor> monitor = flowmon.InstallAll ();
    ThroughputMonitor(&flowmon, monitor, stream); 
    
    
    for (double i = 1; i < simulationEndTime; i = i+0.1){
        Simulator::Schedule (Seconds (i), &ModifyLinkRate, &d1d8, DataRate(std::to_string(rateVal0->GetValue())+"Mbps"),  Time::FromInteger(delayVal0->GetValue(), Time::MS));
        Simulator::Schedule (Seconds (i), &ModifyLinkRate, &d6d9, DataRate(std::to_string(rateVal0->GetValue())+"Mbps"),Time::FromInteger(delayVal0->GetValue(), Time::MS));
        Simulator::Schedule (Seconds (i), &ModifyLinkRate, &d3d7, DataRate(std::to_string(rateVal0->GetValue())+"Mbps"),Time::FromInteger(delayVal0->GetValue(), Time::MS));
        Simulator::Schedule (Seconds (i), &ModifyLinkRate, &d0d2, DataRate(std::to_string(rateVal0->GetValue())+"Mbps"),Time::FromInteger(delayVal0->GetValue(), Time::MS));
    }



    Simulator::Stop (Seconds(simulationEndTime));
    NS_LOG_INFO("\n\n#################### STARTING RUN ####################\n\n");
    Simulator::Run ();


    // flowmon.SerializeToXmlFile("flow", false, false);

    monitor->CheckForLostPackets ();
    Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmon.GetClassifier ());
    FlowMonitor::FlowStatsContainer stats = monitor->GetFlowStats ();

    for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin (); i != stats.end (); ++i)
    {
        Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);
        if (i->first == 1 || (i->first >= 3 && i->first <= 5)){

        NS_LOG_INFO("Flow " << i->first  << " (" << t.sourceAddress << " -> " << t.destinationAddress << ")"
        << "\n Last rx Seconds: " << i->second.timeLastRxPacket.GetSeconds()
        << "\n Rx Bytes: " << i->second.rxBytes
        << "\n DelaySum(s): " << i->second.delaySum.GetSeconds()
        << "\n rxPackets: " << i->second.rxPackets);
        }
    }

    NS_LOG_INFO("\nfile size: "<<maxBytes<< "Bytes, scheduler type " <<schedulerType<<
                "\npath 0: rate "<< rate0 <<", delay "<< delay0 << 
                "\npath 1: rate " << rate1 << ", delay " << delay1 );

    // std::cout
    //     << "\n\n#################### RUN FINISHED ####################\n\n\n";
    Simulator::Destroy ();

    // std::cout
    //     << "\n\n#################### SIMULATION END ####################\n\n\n";
    return 0;
}

