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
 *          
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
#include "ns3/mp-quic-congestion-ops.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("quic-tester");
// connect to a number of traces
static void
CwndChange (Ptr<OutputStreamWrapper> stream, uint32_t oldCwnd, uint32_t newCwnd)
{
  *stream->GetStream () << Simulator::Now ().GetSeconds () << "\t" << oldCwnd << "\t" << newCwnd << std::endl;
}

// static void
// RttChange (Ptr<OutputStreamWrapper> stream, Time oldRtt, Time newRtt)
// {
//   *stream->GetStream () << Simulator::Now ().GetSeconds () << "\t" << oldRtt.GetSeconds () << "\t" << newRtt.GetSeconds () << std::endl;
// }

// static void
// Rx (Ptr<OutputStreamWrapper> stream, Ptr<const Packet> p, const QuicHeader& q, Ptr<const QuicSocketBase> qsb)
// {
//   std::cout<<"rxrxrx"<<"\n";
//   *stream->GetStream () << Simulator::Now ().GetSeconds () << "\t" << p->GetSize() << std::endl;
// }

static void
Traces(uint32_t serverId, std::string pathVersion, std::string finalPart)
{
  AsciiTraceHelper asciiTraceHelper;

  std::ostringstream pathCW;
  pathCW << "/NodeList/" << serverId << "/$ns3::QuicL4Protocol/SocketList/*/QuicSocketBase/CongestionWindow";
  NS_LOG_INFO("Matches cw " << Config::LookupMatches(pathCW.str().c_str()).GetN());

  std::ostringstream path0CW;
  path0CW << "/NodeList/" << serverId << "/$ns3::QuicL4Protocol/SocketList/*/QuicSocketBase/SubflowList/*/MpQuicSubflow/CongestionWindow";
  NS_LOG_INFO("Matches cw " << Config::LookupMatches(path0CW.str().c_str()).GetN());

  std::ostringstream path1CW;
  path1CW << "/NodeList/" << serverId << "/$ns3::QuicL4Protocol/SocketList/*/QuicSocketBase/SubflowWindow1";
  NS_LOG_INFO("Matches cw " << Config::LookupMatches(path1CW.str().c_str()).GetN());

  std::ostringstream fileCW;
  fileCW << pathVersion << "QUIC-cwnd-change"  << serverId << "" << finalPart;
  std::ostringstream file0CW;
  file0CW << pathVersion << "QUIC-cwnd-change-p0-"  << serverId << "" << finalPart;
  std::ostringstream file1CW;
  file1CW << pathVersion << "QUIC-cwnd-change-p1-"  << serverId << "" << finalPart;

  // std::ostringstream pathRTT;
  // pathRTT << "/NodeList/" << serverId << "/$ns3::QuicL4Protocol/SocketList/0/QuicSocketBase/RTT";

  // std::ostringstream fileRTT;
  // fileRTT << pathVersion << "QUIC-rtt"  << serverId << "" << finalPart;

  // std::ostringstream pathRCWnd;
  // pathRCWnd<< "/NodeList/" << serverId << "/$ns3::QuicL4Protocol/SocketList/0/QuicSocketBase/RWND";

  // std::ostringstream fileRCWnd;
  // fileRCWnd<<pathVersion << "QUIC-rwnd-change"  << serverId << "" << finalPart;

  // std::ostringstream fileName;
  // fileName << pathVersion << "QUIC-rx-data" << serverId << "" << finalPart;

  // std::ostringstream pathRx;
  // pathRx << "/NodeList/" << serverId << "/$ns3::QuicL4Protocol/SocketList/*/QuicSocketBase/Rx";
  // NS_LOG_INFO("Matches rx " << Config::LookupMatches(pathRx.str().c_str()).GetN());

  // Ptr<OutputStreamWrapper> stream = asciiTraceHelper.CreateFileStream (fileName.str ().c_str ());
  // Config::ConnectWithoutContext (pathRx.str ().c_str (), MakeBoundCallback (&Rx, stream));

  Ptr<OutputStreamWrapper> stream = asciiTraceHelper.CreateFileStream (fileCW.str ().c_str ());
  Config::ConnectWithoutContext (pathCW.str ().c_str (), MakeBoundCallback(&CwndChange, stream));

  Ptr<OutputStreamWrapper> stream0 = asciiTraceHelper.CreateFileStream (file0CW.str ().c_str ());
  Config::ConnectWithoutContext (path0CW.str ().c_str (), MakeBoundCallback(&CwndChange, stream0));

  Ptr<OutputStreamWrapper> stream1 = asciiTraceHelper.CreateFileStream (file1CW.str ().c_str ());
  Config::ConnectWithoutContext (path1CW.str ().c_str (), MakeBoundCallback(&CwndChange, stream1));

  // Ptr<OutputStreamWrapper> stream2 = asciiTraceHelper.CreateFileStream (fileRTT.str ().c_str ());
  // Config::ConnectWithoutContext (pathRTT.str ().c_str (), MakeBoundCallback(&RttChange, stream2));

  // Ptr<OutputStreamWrapper> stream4 = asciiTraceHelper.CreateFileStream (fileRCWnd.str ().c_str ());
  // Config::ConnectWithoutContextFailSafe (pathRCWnd.str ().c_str (), MakeBoundCallback(&CwndChange, stream4));



}

int
main (int argc, char *argv[])
{
  CommandLine cmd;
  cmd.Parse (argc, argv);

  std::cout
      << "\n\n#################### SIMULATION SET-UP ####################\n\n\n";
  
  //  Ptr<RateErrorModel> em = CreateObjectWithAttributes<RateErrorModel> (
  //     "RanVar", StringValue ("ns3::UniformRandomVariable[Min=0.0|Max=1.0]"),
  //     "ErrorRate", DoubleValue (0.0001));

  LogLevel log_precision = LOG_LEVEL_LOGIC;
  Time::SetResolution (Time::NS);
   LogComponentEnableAll (LOG_PREFIX_TIME);
   LogComponentEnableAll (LOG_PREFIX_FUNC);
   LogComponentEnableAll (LOG_PREFIX_NODE);
  //  LogComponentEnable ("QuicEchoClientApplication", log_precision);
  //  LogComponentEnable ("QuicEchoServerApplication", log_precision);
  //  LogComponentEnable ("quic-tester", log_precision);
//  LogComponentEnable ("QuicHeader", log_precision);
    LogComponentEnable ("QuicSocketBase", log_precision);
 // LogComponentEnable ("QuicStreamBase", LOG_LEVEL_LOGIC);
//  LogComponentEnable ("Socket", log_precision);
//  LogComponentEnable ("Application", log_precision);
//  LogComponentEnable ("Node", log_precision);
//  LogComponentEnable ("InternetStackHelper", log_precision);
//  LogComponentEnable ("QuicSocketFactory", log_precision);
//  LogComponentEnable ("ObjectFactory", log_precision);
//  //LogComponentEnable ("TypeId", log_precision);
  LogComponentEnable ("QuicL4Protocol", log_precision);
//  LogComponentEnable ("QuicL5Protocol", log_precision);
//  //LogComponentEnable ("ObjectBase", log_precision);
//  LogComponentEnable ("QuicEchoHelper", log_precision);
    // LogComponentEnable ("QuicSocketTxScheduler", log_precision);
//  LogComponentEnable ("QuicSocketRxBuffer", log_precision);
//  LogComponentEnable ("QuicHeader", log_precision);
//  LogComponentEnable ("QuicSubheader", log_precision);
//  LogComponentEnable ("Header", log_precision);
//  LogComponentEnable ("PacketMetadata", log_precision);


  Config::SetDefault ("ns3::QuicSocketBase::EnableMultipath",BooleanValue(true));
  Config::SetDefault ("ns3::QuicL4Protocol::SocketType",TypeIdValue (MpQuicCongestionOps::GetTypeId ()));
  Config::SetDefault ("ns3::MpQuicScheduler::SchedulerType", IntegerValue(MpQuicScheduler::ROUND_ROBIN));   


  NodeContainer nodes;
  nodes.Create (2);
  auto n1 = nodes.Get (0);
  auto n2 = nodes.Get (1);

  int sf = 2;
  Time simulationEndTime = Seconds (100);

  QuicHelper stack;
  stack.InstallQuic (nodes);

  std::vector<std::string> rate(2);
  std::vector<std::string> delay(2);

  rate[0]="10Mbps";
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

      NetDeviceContainer netDevices;
      netDevices = p2plink.Install(nodes);

      std::cout<<"netdevice 0 "<<netDevices.Get(0)
              <<"netdevice 1 "<<netDevices.Get(1)
              <<"\n";

      // Attribution of the IP addresses
      std::stringstream netAddr;
      netAddr << "10.1." << (i+1) << ".0";
      std::string str = netAddr.str();

      Ipv4AddressHelper ipv4addr;
      ipv4addr.SetBase(str.c_str(), "255.255.255.0");
      Ipv4InterfaceContainer interface = ipv4addr.Assign(netDevices);
      ipv4Ints.insert(ipv4Ints.end(), interface);

      p2plink.EnablePcap ("prueba" , nodes, true);
  }

  QuicEchoServerHelper echoServer (9);

  ApplicationContainer serverApps = echoServer.Install (nodes.Get (1));
  serverApps.Start (Seconds (0));
  serverApps.Stop (Seconds (20.0));

  QuicEchoClientHelper echoClient (ipv4Ints[0].GetAddress (1), 9);
  // echoClient.SetAttribute ("PacketSize", UintegerValue(1000));
  echoClient.SetAttribute ("MaxPackets", UintegerValue (1));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  //echoClient.SetAttribute ("MaxStreamData", UintegerValue (1024));

  ApplicationContainer clientApps = echoClient.Install (nodes.Get (0));

  echoClient.SetFill (clientApps.Get (0),100, 10000);
  clientApps.Start (Seconds (1));
  clientApps.Stop (Seconds (20.0));

  Simulator::Schedule (Seconds (1.0000001), &Traces, n2->GetId(),
        "./server", ".txt");
  Simulator::Schedule (Seconds (1.0000001), &Traces, n1->GetId(),
        "./client", ".txt");

  Packet::EnablePrinting ();
  Packet::EnableChecking ();

  FlowMonitorHelper flowmon;
  Ptr<FlowMonitor> monitor = flowmon.InstallAll ();

  Simulator::Stop (Seconds (20.0));
  std::cout << "\n\n#################### STARTING RUN ####################\n\n";
  Simulator::Run ();
  flowmon.SerializeToXmlFile("flow", false, false);

  monitor->CheckForLostPackets ();
  Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmon.GetClassifier ());
  FlowMonitor::FlowStatsContainer stats = monitor->GetFlowStats ();

  std::ofstream outfile;
  outfile.open("wmp.txt");
  for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin (); i != stats.end (); ++i)
    {
      Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);

      outfile << "Flow " << i->first  << " (" << t.sourceAddress << " -> " << t.destinationAddress << ")\n";
      outfile << "  Tx Packets: " << i->second.txPackets << "\n";
      outfile << "  Tx Bytes:   " << i->second.txBytes << "\n";
      outfile << "  TxOffered:  " << i->second.txBytes * 8.0 / simulationEndTime.GetSeconds () / 1000 / 1000  << " Mbps\n";
      outfile << "  Rx Packets: " << i->second.rxPackets << "\n";
      outfile << "  Rx Bytes:   " << i->second.rxBytes << "\n";
      outfile << "  Throughput: " << i->second.rxBytes * 8.0 / simulationEndTime.GetSeconds () / 1000 / 1000  << " Mbps\n";
      outfile << "  Tx time: " << i->second.timeLastTxPacket - i->second.timeFirstTxPacket<<"\n";
      outfile << "  Rx time: " << i->second.timeLastRxPacket - i->second.timeFirstRxPacket<<"\n";
      outfile << "delay sum" << i->second.delaySum<<"\n";
                    
    }
  
   
  outfile.close();
  std::cout
      << "\n\n#################### RUN FINISHED ####################\n\n\n";
  Simulator::Destroy ();

  std::cout
      << "\n\n#################### SIMULATION END ####################\n\n\n";
  return 0;
}

