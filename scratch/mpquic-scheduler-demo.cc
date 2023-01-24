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

static void
CwndChange (Ptr<OutputStreamWrapper> stream, uint32_t oldCwnd, uint32_t newCwnd)
{
    *stream->GetStream () << Simulator::Now ().GetSeconds () << "\t" << oldCwnd << "\t" << newCwnd << std::endl;
}

// static void
// RewardChange (Ptr<OutputStreamWrapper> stream1, uint32_t oldReward, uint32_t newReward)
// {
//     *stream1->GetStream () << Simulator::Now ().GetSeconds () << "\t" << oldReward << "\t" << newReward << std::endl;
// }

static void
Traces(uint32_t serverId, std::string pathVersion, std::string finalPart)
{
    AsciiTraceHelper asciiTraceHelper;

    std::ostringstream path0CW;
    path0CW << "/NodeList/" << serverId << "/$ns3::QuicL4Protocol/SocketList/0/QuicSocketBase/CongestionWindow";
    NS_LOG_INFO("Matches cw " << Config::LookupMatches(path0CW.str().c_str()).GetN());

    std::ostringstream path1CW;
    path1CW << "/NodeList/" << serverId << "/$ns3::QuicL4Protocol/SocketList/0/QuicSocketBase/CongestionWindow1";
    NS_LOG_INFO("Matches cw " << Config::LookupMatches(path1CW.str().c_str()).GetN());

    std::ostringstream reward;
    reward << "/NodeList/" << serverId << "/$ns3::QuicL4Protocol/SocketList/0/QuicSocketBase/MabRewardTrace";
    // reward << "/NodeList/" << serverId << "/$ns3::QuicL5Protocol/StreamList/*/QuicStreamBase/RxBufferTrace";
    NS_LOG_INFO("Matches cw " << Config::LookupMatches(reward.str().c_str()).GetN());

    std::ostringstream file0CW;
    file0CW << pathVersion << "QUIC-cwnd-change-0" << "" << finalPart;
    std::ostringstream file1CW;
    file1CW << pathVersion << "QUIC-cwnd-change-1"<< "" << finalPart;
    std::ostringstream fileReward;
    fileReward << pathVersion << "QUIC-reward"<< "" << finalPart;


    Ptr<OutputStreamWrapper> stream = asciiTraceHelper.CreateFileStream (file0CW.str ().c_str ());
    Config::ConnectWithoutContext (path0CW.str ().c_str (), MakeBoundCallback(&CwndChange, stream));

    Ptr<OutputStreamWrapper> stream0 = asciiTraceHelper.CreateFileStream (file1CW.str ().c_str ());
    Config::ConnectWithoutContext (path1CW.str ().c_str (), MakeBoundCallback(&CwndChange, stream0));

    Ptr<OutputStreamWrapper> stream1 = asciiTraceHelper.CreateFileStream (fileReward.str ().c_str ());
    Config::ConnectWithoutContext (reward.str ().c_str (), MakeBoundCallback(&CwndChange, stream1));

}

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
        if (stats->first == 1 || stats->first == 3){
            // std::cout << " Flow: " << stats->first  << " Seconds: " << stats->second.timeLastRxPacket.GetSeconds()  << " Tx Bytes: " << stats->second.txBytes << " Received Bytes: " << stats->second.rxBytes << " First seconds: " << stats->second.timeFirstTxPacket.GetSeconds() << std::endl;
            *stream->GetStream () << stats->first  << "\t" << stats->second.timeLastRxPacket.GetSeconds() << "\t" << stats->second.rxBytes << "\t" << stats->second.rxBytes*8/1024/1024/(stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstRxPacket.GetSeconds()) << std::endl;
        }
    }
    Simulator::Schedule(Seconds(0.005),&ThroughputMonitor, fmhelper, flowMon, stream);
}

int
main (int argc, char *argv[])
{
    int sf = 2;
    int simSeconds = 20;
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
//  LogComponentEnable ("MpQuicScheduler", log_precision);

    Config::SetDefault ("ns3::QuicSocketBase::SocketSndBufSize",UintegerValue (40000000));
    Config::SetDefault ("ns3::QuicStreamBase::StreamSndBufSize",UintegerValue (40000000));
    Config::SetDefault ("ns3::QuicSocketBase::SocketRcvBufSize",UintegerValue (40000000));
    Config::SetDefault ("ns3::QuicStreamBase::StreamRcvBufSize",UintegerValue (40000000));
    Config::SetDefault ("ns3::QuicSocketBase::EnableMultipath",BooleanValue(true));
    Config::SetDefault ("ns3::QuicSocketBase::CcType",IntegerValue(QuicSocketBase::OLIA));
    Config::SetDefault ("ns3::QuicL4Protocol::SocketType",TypeIdValue (MpQuicCongestionOps::GetTypeId ()));
    // Config::SetDefault ("ns3::MpQuicScheduler::SchedulerType", IntegerValue(MpQuicScheduler::ROUND_ROBIN));   
    // Config::SetDefault ("ns3::MpQuicScheduler::SchedulerType", IntegerValue(MpQuicScheduler::MIN_RTT));   
    // Config::SetDefault ("ns3::MpQuicScheduler::SchedulerType", IntegerValue(MpQuicScheduler::MAB));   
    Config::SetDefault ("ns3::MpQuicScheduler::SchedulerType", IntegerValue(MpQuicScheduler::BLEST));   
    Config::SetDefault ("ns3::MpQuicScheduler::MabRate", UintegerValue(50000));   

    NodeContainer nodes;
    nodes.Create (2);
    auto n1 = nodes.Get (0);
    auto n2 = nodes.Get (1);


    Time simulationEndTime = Seconds (simSeconds);

    int start_time = 1;

    QuicHelper stack;
    stack.InstallQuic (nodes);

    // std::string rate = "10Mbps";
    // std::string delay = "10ms";
    // std::vector<NetDeviceContainer> netDevices(sf);

    std::vector<std::string> rate(2);
    std::vector<std::string> delay(2);
    std::vector<NetDeviceContainer> netDevices(2);

    // rate[0]="50Mbps";
    // delay[0]="20ms";
    // rate[1]="10Mbps";
    // delay[1]="140ms";

    // rate[0]="10Mbps";
    // delay[0]="200ms";
    // rate[1]="50Mbps";
    // delay[1]="20ms";


rate[0]="1Mbps";
delay[0]="10ms";
rate[1]="10Mbps";
delay[1]="10ms";


    std::vector<Ipv4InterfaceContainer> ipv4Ints;
    for(int i=0; i < sf; i++)
    {
        // Creation of the point to point link between hots
        PointToPointHelper p2plink;
        p2plink.SetDeviceAttribute ("DataRate", StringValue(rate[i]));
        p2plink.SetChannelAttribute("Delay", StringValue(delay[i]));

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
    
    // Ptr<UniformRandomVariable> x = CreateObject<UniformRandomVariable> ();
    // uint32_t myRandomNo = x->GetInteger (100,500);
    uint32_t maxBytes = 1024*1024;
    // uint32_t maxBytes = 8*1024*1024+myRandomNo;

    BulkSendHelper source ("ns3::QuicSocketFactory",
                            InetSocketAddress (ipv4Ints[0].GetAddress (1), port));
    // Set the amount of data to send in bytes.  Zero is unlimited.
    source.SetAttribute ("MaxBytes", UintegerValue (maxBytes));
    // source.SetAttribute ("SendSize", UintegerValue (1400));
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

    Simulator::Schedule (Seconds (start_time+0.0000001), &Traces, nodes.Get(0)->GetId(),
    "./schedulerClient", ".txt");
    // Simulator::Schedule (Seconds (start_time+0.0000001), &TracesServer, nodes.Get(1)->GetId(),
    // "./schedulerServer", ".txt");

    AsciiTraceHelper asciiTraceHelper;
    std::ostringstream fileName;
    fileName <<  "./schedulerClient" << "QUIC-rx" << "" << ".txt";
    Ptr<OutputStreamWrapper> stream = asciiTraceHelper.CreateFileStream (fileName.str ().c_str ());
  

    FlowMonitorHelper flowmon;
    Ptr<FlowMonitor> monitor = flowmon.InstallAll ();
    ThroughputMonitor(&flowmon, monitor, stream); 
    
    Simulator::Stop (simulationEndTime);
    std::cout << "\n\n#################### STARTING RUN ####################\n\n";
    Simulator::Run ();

    // std::cout<< "\n\n#################### RUN FINISHED ####################\n\n\n";

    
    
    flowmon.SerializeToXmlFile("flow", false, false);

    monitor->CheckForLostPackets ();
    Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmon.GetClassifier ());
    FlowMonitor::FlowStatsContainer stats = monitor->GetFlowStats ();

    for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin (); i != stats.end (); ++i)
    {
        Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);
        if (i->first == 1 || i->first == 3){
        std::cout << "Flow " << i->first  << " (" << t.sourceAddress << " -> " << t.destinationAddress << ")\n";
        // std::cout << "  Tx Bytes:  " << i->second.txBytes << "\n"; 
        // std::cout << "  Tx time: " << (i->second.timeLastTxPacket - i->second.timeFirstTxPacket).GetSeconds()<<"\n";  
        // std::cout << " First tx seconds: " << i->second.timeFirstTxPacket.GetSeconds() << std::endl;
        // std::cout << " First rx seconds: " << i->second.timeFirstRxPacket.GetSeconds() << std::endl;
        // std::cout << " Last tx Seconds: " << i->second.timeLastTxPacket.GetSeconds()  << std::endl;
        std::cout << " Last rx Seconds: " << i->second.timeLastRxPacket.GetSeconds()  << std::endl;
        // std::cout << " Tx Bytes: " << i->second.txBytes << std::endl;
        std::cout << " Rx Bytes: " << i->second.rxBytes << std::endl;
        // std::cout << " Dropped Bytes vector size: " << i->second.bytesDropped.size() << std::endl;
        // std::cout << " Dropped Packets vector size: " << i->second.packetsDropped.size() << std::endl;
        // std::cout << " Rx Duration(s): " << (i->second.timeLastRxPacket.GetSeconds() - i->second.timeFirstRxPacket.GetSeconds()) << std::endl;
        // std::cout << " RX Goodput: " << i->second.rxBytes * 8.0 / 1024 / 1024 /(i->second.timeLastRxPacket.GetSeconds() - i->second.timeFirstRxPacket.GetSeconds()) << " Mbps\n"; 
        std::cout << " DelaySum(s): " << i->second.delaySum.GetSeconds() << std::endl;
        std::cout << " rxPackets: " << i->second.rxPackets << std::endl;
        // std::cout << " Avg. Delay: " << i->second.delaySum.GetSeconds()/i->second.rxPackets << std::endl;


        // std::cout << " jitterSum: " << i->second.jitterSum.GetSeconds() << std::endl;
        // std::cout << " lastDelay: " << i->second.lastDelay.GetSeconds() << std::endl;
        // std::cout << " lostPackets number: " << i->second.lostPackets << std::endl;
        // std::cout << " timesForwarded: " << i->second.timesForwarded << std::endl;
        }
        
    }
    Simulator::Destroy ();

    std::cout
        << "\n\n#################### SIMULATION END ####################\n\n\n";

    return 0;
}

