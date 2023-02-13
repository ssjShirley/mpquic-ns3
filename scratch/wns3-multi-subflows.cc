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

NS_LOG_COMPONENT_DEFINE("wns3-multi-subflows");

void ThroughputMonitor (FlowMonitorHelper *fmhelper, Ptr<FlowMonitor> flowMon)
{
    std::map<FlowId, FlowMonitor::FlowStats> flowStats = flowMon->GetFlowStats();
    Ptr<Ipv4FlowClassifier> classing = DynamicCast<Ipv4FlowClassifier> (fmhelper->GetClassifier());
    for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator stats = flowStats.begin (); stats != flowStats.end (); ++stats)
    {
        // Ipv4FlowClassifier::FiveTuple fiveTuple = classing->FindFlow (stats->first);
        // if (stats->first == 1 || stats->first == 3 || stats->first == 4 || stats->first == 5 || stats->first == 6 || stats->first == 1 ||stats->first == 7 || stats->first == 8 || stats->first == 9){
            std::cout << " Flow: " << stats->first  << " Seconds: " << stats->second.timeLastRxPacket.GetSeconds()  << " Received Bytes: " << stats->second.rxBytes << " Throughput: " << stats->second.rxBytes * 8.0 / (stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstTxPacket.GetSeconds())/1024/1024  << std::endl;
        // }
    }
    Simulator::Schedule(Seconds(0.005),&ThroughputMonitor, fmhelper, flowMon);
}

int
main (int argc, char *argv[])
{
    int sf = 8;
    CommandLine cmd;
    cmd.AddValue ("SubflowNumber", "in use subflow numbers (1, 2, 4 or 8)", sf);
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
    LogComponentEnable ("wns3-multi-subflows", log_precision);
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


    Config::SetDefault ("ns3::QuicSocketBase::SocketSndBufSize",UintegerValue (40000000));
    Config::SetDefault ("ns3::QuicStreamBase::StreamSndBufSize",UintegerValue (40000000));
    Config::SetDefault ("ns3::QuicSocketBase::SocketRcvBufSize",UintegerValue (40000000));
    Config::SetDefault ("ns3::QuicStreamBase::StreamRcvBufSize",UintegerValue (40000000));


    // Config::SetDefault ("ns3::QuicSocketBase::EnableMultipath",BooleanValue(true));
    // Config::SetDefault ("ns3::QuicSocketBase::CcType",IntegerValue(QuicSocketBase::OLIA));
    // Config::SetDefault ("ns3::QuicL4Protocol::SocketType",TypeIdValue (MpQuicCongestionOps::GetTypeId ()));
    Config::SetDefault ("ns3::MpQuicScheduler::SchedulerType", IntegerValue(MpQuicScheduler::ROUND_ROBIN));   

    NodeContainer nodes;
    nodes.Create (2);
    auto n1 = nodes.Get (0);
    auto n2 = nodes.Get (1);


    Time simulationEndTime = Seconds (5);

    int start_time = 1;

    QuicHelper stack;
    stack.InstallQuic (nodes);

    std::string rate = "10Mbps";
    std::string delay = "10ms";
    std::vector<NetDeviceContainer> netDevices(sf);

    std::vector<Ipv4InterfaceContainer> ipv4Ints;
    for(int i=0; i < sf; i++)
    {
        // Creation of the point to point link between hots
        PointToPointHelper p2plink;
        p2plink.SetDeviceAttribute ("DataRate", StringValue(rate));
        p2plink.SetChannelAttribute("Delay", StringValue(delay));

        netDevices[i] = p2plink.Install(nodes);
        
        // Attribution of the IP addresses
        std::stringstream netAddr;
        netAddr << "10.1." << (i+1) << ".0";
        std::string str = netAddr.str();

        Ipv4AddressHelper ipv4addr;
        ipv4addr.SetBase(str.c_str(), "255.255.255.0");
        Ipv4InterfaceContainer interface = ipv4addr.Assign(netDevices[i]);
        ipv4Ints.insert(ipv4Ints.end(), interface);

        p2plink.EnablePcap ("prueba" , nodes, true);
    }

    uint16_t port = 9;  // well-known echo port number
    
    Ptr<UniformRandomVariable> x = CreateObject<UniformRandomVariable> ();
    uint32_t myRandomNo = x->GetInteger (100,109);
    uint32_t maxBytes = myRandomNo * 80000;
    // uint32_t maxBytes = 0;
    BulkSendHelper source ("ns3::QuicSocketFactory",
                            InetSocketAddress (ipv4Ints[0].GetAddress (1), port));
    // Set the amount of data to send in bytes.  Zero is unlimited.
    source.SetAttribute ("MaxBytes", UintegerValue (maxBytes));
    ApplicationContainer sourceApps = source.Install (nodes.Get (0));
    sourceApps.Start (Seconds (start_time));
    sourceApps.Stop (simulationEndTime);

    //
    // Create a PacketSinkApplication and install it on node 1
    //
    PacketSinkHelper sink ("ns3::QuicSocketFactory",
                            InetSocketAddress (Ipv4Address::GetAny (), port));
    ApplicationContainer sinkApps = sink.Install (nodes.Get (1));
    sinkApps.Start (Seconds (0.0));
    sinkApps.Stop (simulationEndTime);

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

