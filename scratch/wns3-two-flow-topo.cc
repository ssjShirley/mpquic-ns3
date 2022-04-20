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
 * n0                     n2
 *   \        TCP        /
 *    n1 -------------- n8
 *   /         S0        \
 * n4                     n5
 *   \         S1        /
 *    n6 -------------- n9
 *   /        TCP        \
 * n3                     n7
 *
 * Sample script:
 *   ./waf --run "scratch/wns3-two-flow-topo.cc --SchedulerType=0 --Rate0="10Mbps" --Rate1="10Mbps"" >log.out 2>d1
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

NS_LOG_COMPONENT_DEFINE("wns3-two-flow-topo");


static void
CwndChange (Ptr<OutputStreamWrapper> stream, uint32_t oldCwnd, uint32_t newCwnd)
{
    *stream->GetStream () << Simulator::Now ().GetSeconds () << "\t" << oldCwnd << "\t" << newCwnd << std::endl;
}

static void
Traces(uint32_t serverId, std::string pathVersion, std::string finalPart)
{
    AsciiTraceHelper asciiTraceHelper;

    std::ostringstream path0CW;
    path0CW << "/NodeList/" << serverId << "/$ns3::QuicL4Protocol/SocketList/0/QuicSocketBase/CongestionWindow";
    NS_LOG_INFO("Matches cw " << Config::LookupMatches(path0CW.str().c_str()).GetN());

    std::ostringstream path1CW;
    path1CW << "/NodeList/" << serverId << "/$ns3::QuicL4Protocol/SocketList/*/QuicSocketBase/CongestionWindow1";
    NS_LOG_INFO("Matches cw " << Config::LookupMatches(path1CW.str().c_str()).GetN());

    std::ostringstream file0CW;
    file0CW << pathVersion << "QUIC-cwnd-change-0" << "" << finalPart;
    std::ostringstream file1CW;
    file1CW << pathVersion << "QUIC-cwnd-change-1"<< "" << finalPart;

    Ptr<OutputStreamWrapper> stream = asciiTraceHelper.CreateFileStream (file0CW.str ().c_str ());
    Config::ConnectWithoutContext (path0CW.str ().c_str (), MakeBoundCallback(&CwndChange, stream));

    Ptr<OutputStreamWrapper> stream0 = asciiTraceHelper.CreateFileStream (file1CW.str ().c_str ());
    Config::ConnectWithoutContext (path1CW.str ().c_str (), MakeBoundCallback(&CwndChange, stream0));

}

void ThroughputMonitor (FlowMonitorHelper *fmhelper, Ptr<FlowMonitor> flowMon)
{
    std::map<FlowId, FlowMonitor::FlowStats> flowStats = flowMon->GetFlowStats();
    Ptr<Ipv4FlowClassifier> classing = DynamicCast<Ipv4FlowClassifier> (fmhelper->GetClassifier());
    for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator stats = flowStats.begin (); stats != flowStats.end (); ++stats)
    {
        if (stats->first == 1 ) {
            std::cout << " TCP 0\t| Second:\t" << stats->second.timeLastRxPacket.GetSeconds()  << "\t, Throughput:\t" << stats->second.rxBytes * 8.0 / (stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstTxPacket.GetSeconds())/1024/1024 << std::endl;
        }
        if (stats->first == 2 ) {
            std::cout << " TCP 1\t| Second:\t" << stats->second.timeLastRxPacket.GetSeconds()  << "\t, Throughput:\t" << stats->second.rxBytes * 8.0 / (stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstTxPacket.GetSeconds())/1024/1024 << std::endl;
        }
        if (stats->first == 3 ) {
            std::cout << " S 0\t| Second:\t" << stats->second.timeLastRxPacket.GetSeconds()  << "\t, Throughput:\t" << stats->second.rxBytes * 8.0 / (stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstTxPacket.GetSeconds())/1024/1024 << std::endl;
        }
        if (stats->first == 7 ) {
            std::cout << " S 1\t| Second:\t" << stats->second.timeLastRxPacket.GetSeconds()  << "\t, Throughput:\t" << stats->second.rxBytes * 8.0 / (stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstTxPacket.GetSeconds())/1024/1024 << std::endl;
        }
    }

    Simulator::Schedule(Seconds(0.1),&ThroughputMonitor, fmhelper, flowMon);

}

int
main (int argc, char *argv[])
{
    int schedulerType = MpQuicScheduler::ROUND_ROBIN;
    string rate0 = "10Mbps";
    string rate1 = "10Mbps";
    int ccType = QuicSocketBase::OLIA;
    TypeId ccTypeId = MpQuicCongestionOps::GetTypeId ();
    CommandLine cmd;
    
    cmd.AddValue ("SchedulerType", "in use scheduler type (0 - ROUND_ROBIN, 1 - MIN_RTT)", schedulerType);
    cmd.AddValue ("Rate0", "e.g. 10Mbps", rate0);
    cmd.AddValue ("Rate1", "e.g. 50Mbps", rate1);
    cmd.AddValue ("CcType", "in use congestion control type (0 - QuicNewReno, 1 - OLIA)", ccType);
    cmd.Parse (argc, argv);

    std::cout
        << "\n\n#################### SIMULATION SET-UP ####################\n\n\n";
    
    LogLevel log_precision = LOG_LEVEL_LOGIC;
    Time::SetResolution (Time::NS);
    LogComponentEnableAll (LOG_PREFIX_TIME);
    LogComponentEnableAll (LOG_PREFIX_FUNC);
    LogComponentEnableAll (LOG_PREFIX_NODE);
//  LogComponentEnable ("QuicEchoClientApplication", log_precision);
//  LogComponentEnable ("QuicEchoServerApplication", log_precision);
    LogComponentEnable ("wns3-two-flow-topo", log_precision);
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

    
    Time simulationEndTime = Seconds (5);
    int start_time = 1;

    Ptr<UniformRandomVariable> x = CreateObject<UniformRandomVariable> ();
    uint32_t myRandomNo = x->GetInteger (100,109);
    uint32_t maxBytes = myRandomNo * 80000;
    
    NS_LOG_INFO ("Create nodes.");
    NodeContainer c;
    c.Create (10);
    NodeContainer n0n1 = NodeContainer (c.Get (0), c.Get (1));
    NodeContainer n1n8 = NodeContainer (c.Get (1), c.Get (8));
    NodeContainer n8n2 = NodeContainer (c.Get (8), c.Get (2));
    
    NodeContainer n3n6 = NodeContainer (c.Get (3), c.Get (6));
    NodeContainer n6n9 = NodeContainer (c.Get (6), c.Get (9));
    NodeContainer n9n7 = NodeContainer (c.Get (9), c.Get (7));

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
    p2p.SetDeviceAttribute ("DataRate", StringValue (rate0));
    p2p.SetChannelAttribute ("Delay", StringValue ("10ms"));
    NetDeviceContainer d1d8 = p2p.Install (n1n8);

    p2p.SetDeviceAttribute ("DataRate", StringValue (rate1));
    p2p.SetChannelAttribute ("Delay", StringValue ("10ms"));
    NetDeviceContainer d6d9 = p2p.Install (n6n9);

    //background traffic
    p2p.SetDeviceAttribute ("DataRate", StringValue ("100Mbps"));
    p2p.SetChannelAttribute ("Delay", StringValue ("0ms"));
    NetDeviceContainer d4d1 = p2p.Install (n4n1);
    NetDeviceContainer d0d1 = p2p.Install (n0n1);
    NetDeviceContainer d8d5 = p2p.Install (n8n5);
    NetDeviceContainer d4d6 = p2p.Install (n4n6);
    NetDeviceContainer d9d5 = p2p.Install (n9n5);
    NetDeviceContainer d8d2 = p2p.Install (n8n2);
    NetDeviceContainer d3d6 = p2p.Install (n3n6);
    NetDeviceContainer d9d7 = p2p.Install (n9n7);
    
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
    Ipv4InterfaceContainer i0i1 = ipv4.Assign (d0d1);

    ipv4.SetBase ("10.1.2.0", "255.255.255.0");
    Ipv4InterfaceContainer i1i2 = ipv4.Assign (d8d2);
    
    ipv4.SetBase ("10.1.3.0", "255.255.255.0");
    Ipv4InterfaceContainer i3i6 = ipv4.Assign (d3d6);

    ipv4.SetBase ("10.1.8.0", "255.255.255.0");
    Ipv4InterfaceContainer i6i7 = ipv4.Assign (d9d7);


    Ptr<Ipv4> ipv4_n4 = c.Get(4)->GetObject<Ipv4> ();
    Ipv4StaticRoutingHelper ipv4RoutingHelper; 
    Ptr<Ipv4StaticRouting> staticRouting_n4 = ipv4RoutingHelper.GetStaticRouting (ipv4_n4); 
    staticRouting_n4->AddHostRouteTo (Ipv4Address ("10.1.5.2"), Ipv4Address ("10.1.9.2") ,1); 
    staticRouting_n4->AddHostRouteTo (Ipv4Address ("10.1.7.2"), Ipv4Address ("10.1.10.2") ,2); 

    Ptr<Ipv4> ipv4_n5 = c.Get(5)->GetObject<Ipv4> ();
    Ptr<Ipv4StaticRouting> staticRouting_n5 = ipv4RoutingHelper.GetStaticRouting (ipv4_n5); 
    staticRouting_n5->AddHostRouteTo (Ipv4Address ("10.1.4.1"), Ipv4Address ("10.1.9.1") ,1); 
    staticRouting_n5->AddHostRouteTo (Ipv4Address ("10.1.6.1"), Ipv4Address ("10.1.10.1") ,2); 

    // Create router nodes, initialize routing database and set up the routing
    // tables in the nodes.
    Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
    
    //background 1
    uint16_t port0 = 11;  // well-known echo port number
    BulkSendHelper source0 ("ns3::TcpSocketFactory",
                        InetSocketAddress (i1i2.GetAddress (1), port0));
    // Set the amount of data to send in bytes.  Zero is unlimited.
    source0.SetAttribute ("MaxBytes", UintegerValue (maxBytes));
    ApplicationContainer sourceApps0 = source0.Install (c.Get (0));
    sourceApps0.Start (Seconds (1.0));
    sourceApps0.Stop (simulationEndTime);
    PacketSinkHelper sink0 ("ns3::TcpSocketFactory",
                        InetSocketAddress (Ipv4Address::GetAny (), port0));
    ApplicationContainer sinkApps0 = sink0.Install (c.Get (2));
    sinkApps0.Start (Seconds (0.0));
    sinkApps0.Stop (simulationEndTime);

 
    //background2
    uint16_t port1 = 13;  // well-known echo port number
    // uint32_t maxBytes1 = 0;
    BulkSendHelper source1 ("ns3::TcpSocketFactory",
                            InetSocketAddress (i6i7.GetAddress (1), port1));
    // Set the amount of data to send in bytes.  Zero is unlimited.
    source1.SetAttribute ("MaxBytes", UintegerValue (maxBytes));
    ApplicationContainer sourceApps1 = source1.Install (c.Get (3));
    sourceApps1.Start (Seconds (1.0));
    sourceApps1.Stop (simulationEndTime);
    PacketSinkHelper sink1 ("ns3::TcpSocketFactory",
                            InetSocketAddress (Ipv4Address::GetAny (), port1));
    ApplicationContainer sinkApps1 = sink1.Install (c.Get (7));
    sinkApps1.Start (Seconds (0.0));
    sinkApps1.Stop (simulationEndTime);

  
    //mpquic
    uint16_t port2 = 9;  // well-known echo port number
    
    BulkSendHelper source ("ns3::QuicSocketFactory",
                            InetSocketAddress (i8i5.GetAddress (1), port2));
    // Set the amount of data to send in bytes.  Zero is unlimited.
    source.SetAttribute ("MaxBytes", UintegerValue (maxBytes));
    ApplicationContainer sourceApps = source.Install (c.Get (4));
    sourceApps.Start (Seconds (start_time));
    sourceApps.Stop (simulationEndTime);

    //
    // Create a PacketSinkApplication and install it on node 1
    //
    PacketSinkHelper sink2 ("ns3::QuicSocketFactory",
                            InetSocketAddress (Ipv4Address::GetAny (), port2));
    ApplicationContainer sinkApps2 = sink2.Install (c.Get (5));
    sinkApps2.Start (Seconds (0.0));
    sinkApps2.Stop (simulationEndTime);

    Simulator::Schedule (Seconds (start_time+0.0000001), &Traces, c.Get (4)->GetId(),
        "./wns3Client", ".txt");

    Packet::EnablePrinting ();
    Packet::EnableChecking ();

    FlowMonitorHelper flowmon;
    Ptr<FlowMonitor> monitor = flowmon.InstallAll ();
    ThroughputMonitor(&flowmon, monitor); 

    Simulator::Stop (simulationEndTime);
    std::cout << "\n\n#################### STARTING RUN ####################\n\n";
    Simulator::Run ();

    std::cout
        << "\n\n#################### RUN FINISHED ####################\n\n\n";
    Simulator::Destroy ();

    std::cout
        << "\n\n#################### SIMULATION END ####################\n\n\n";
    return 0;
}

