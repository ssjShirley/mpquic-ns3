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
#include "ns3/gnuplot.h"

#include <boost/assign/list_of.hpp>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("quic-tester");
// connect to a number of traces
static void
CwndChange (Ptr<OutputStreamWrapper> stream, uint32_t oldCwnd, uint32_t newCwnd)
{
  *stream->GetStream () << Simulator::Now ().GetSeconds () << "\t" << oldCwnd << "\t" << newCwnd << std::endl;
}

static void
RttChange (Ptr<OutputStreamWrapper> stream, Time oldRtt, Time newRtt)
{
  *stream->GetStream () << Simulator::Now ().GetSeconds () << "\t" << oldRtt.GetSeconds () << "\t" << newRtt.GetSeconds () << std::endl;
}

static void
SshChange (Ptr<OutputStreamWrapper> stream, uint32_t oldCwnd, uint32_t newCwnd)
{
  *stream->GetStream () << Simulator::Now ().GetSeconds () << "\t" << oldCwnd << "\t" << newCwnd << std::endl;
}

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
  pathCW << "/NodeList/" << serverId << "/$ns3::QuicL4Protocol/SocketList/0/QuicSocketBase/CongestionWindow";
  NS_LOG_INFO("Matches cw " << Config::LookupMatches(pathCW.str().c_str()).GetN());

  std::ostringstream path0CW;
  path0CW << "/NodeList/" << serverId << "/$ns3::QuicL4Protocol/SocketList/*/QuicSocketBase/CongestionWindow1";
  NS_LOG_INFO("Matches cw " << Config::LookupMatches(path0CW.str().c_str()).GetN());

//   std::ostringstream path1CW;
//   path1CW << "/NodeList/" << serverId << "/$ns3::QuicL4Protocol/SocketList/*/QuicSocketBase/SubflowWindow1";
//   NS_LOG_INFO("Matches cw " << Config::LookupMatches(path1CW.str().c_str()).GetN());

  std::ostringstream fileCW;
  fileCW << pathVersion << "QUIC-cwnd-change-0" << "" << finalPart;
  std::ostringstream file0CW;
  file0CW << pathVersion << "QUIC-cwnd-change-1"<< "" << finalPart;
//   std::ostringstream file1CW;
//   file1CW << pathVersion << "QUIC-cwnd-change-p1-"  << serverId << "" << finalPart;

  // std::ostringstream pathRTT;
  // pathRTT << "/NodeList/" << serverId << "/$ns3::QuicL4Protocol/SocketList/0/QuicSocketBase/RTT";

  std::ostringstream path0rtt;
  path0rtt << "/NodeList/" << serverId << "/$ns3::QuicL4Protocol/SocketList/*/QuicSocketBase/RTT0";

  std::ostringstream path1rtt;
  path1rtt << "/NodeList/" << serverId << "/$ns3::QuicL4Protocol/SocketList/*/QuicSocketBase/RTT1";
  
  std::ostringstream file0rtt;
  file0rtt << pathVersion << "QUIC-rtt-change-p0" << "" << finalPart;
  std::ostringstream file1rtt;
  file1rtt << pathVersion << "QUIC-rtt-change-p1" << "" << finalPart;

  std::ostringstream path0ssh;
  path0ssh << "/NodeList/" << serverId << "/$ns3::QuicL4Protocol/SocketList/*/QuicSocketBase/SlowStartThreshold";

  std::ostringstream path1ssh;
  path1ssh << "/NodeList/" << serverId << "/$ns3::QuicL4Protocol/SocketList/*/QuicSocketBase/SlowStartThreshold1";
  
  std::ostringstream file0ssh;
  file0ssh << pathVersion << "QUIC-ssh-change-p0" << "" << finalPart;
  std::ostringstream file1ssh;
  file1ssh << pathVersion << "QUIC-ssh-change-p1" << "" << finalPart;

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


  Ptr<OutputStreamWrapper> stream0rtt = asciiTraceHelper.CreateFileStream (file0rtt.str ().c_str ());
  Config::ConnectWithoutContext (path0rtt.str ().c_str (), MakeBoundCallback(&RttChange, stream0rtt));

  Ptr<OutputStreamWrapper> stream1rtt = asciiTraceHelper.CreateFileStream (file1rtt.str ().c_str ());
  Config::ConnectWithoutContext (path1rtt.str ().c_str (), MakeBoundCallback(&RttChange, stream1rtt));


  Ptr<OutputStreamWrapper> stream0ssh = asciiTraceHelper.CreateFileStream (file0ssh.str ().c_str ());
  Config::ConnectWithoutContext (path0ssh.str ().c_str (), MakeBoundCallback(&SshChange, stream0ssh));

  Ptr<OutputStreamWrapper> stream1ssh = asciiTraceHelper.CreateFileStream (file1ssh.str ().c_str ());
  Config::ConnectWithoutContext (path1ssh.str ().c_str (), MakeBoundCallback(&SshChange, stream1ssh));


  // Ptr<OutputStreamWrapper> stream2 = asciiTraceHelper.CreateFileStream (fileRTT.str ().c_str ());
  // Config::ConnectWithoutContext (pathRTT.str ().c_str (), MakeBoundCallback(&RttChange, stream2));

  // Ptr<OutputStreamWrapper> stream4 = asciiTraceHelper.CreateFileStream (fileRCWnd.str ().c_str ());
  // Config::ConnectWithoutContextFailSafe (pathRCWnd.str ().c_str (), MakeBoundCallback(&CwndChange, stream4));



}

std::vector<uint32_t> RxBytesList = boost::assign::list_of(0)(0);

void ThroughputMonitor (FlowMonitorHelper *fmhelper, Ptr<FlowMonitor> flowMon, Gnuplot2dDataset DataSet, Gnuplot2dDataset DataSet1)
	{
    // double localThrou=0;
		std::map<FlowId, FlowMonitor::FlowStats> flowStats = flowMon->GetFlowStats();
		Ptr<Ipv4FlowClassifier> classing = DynamicCast<Ipv4FlowClassifier> (fmhelper->GetClassifier());
		for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator stats = flowStats.begin (); stats != flowStats.end (); ++stats)
		{
			Ipv4FlowClassifier::FiveTuple fiveTuple = classing->FindFlow (stats->first);
			std::cout<<"Flow ID			: " << stats->first <<" ; "<< fiveTuple.sourceAddress <<" -----> "<<fiveTuple.destinationAddress<<std::endl;
			std::cout<<"Tx Packets = " << stats->second.txPackets<<std::endl;
			std::cout<<"Rx Packets = " << stats->second.rxPackets<<std::endl;
      std::cout<<"Duration		: "<<(stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstTxPacket.GetSeconds())<<std::endl;
			std::cout<<"Last Received Packet	: "<< stats->second.timeLastRxPacket.GetSeconds()<<" Seconds"<<std::endl;
			std::cout<<"total Throughput: " << stats->second.rxBytes * 8.0 / (stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstTxPacket.GetSeconds())/1024/1024  << " Mbps"<<std::endl;
      // localThrou=(stats->second.rxBytes * 8.0 / (stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstTxPacket.GetSeconds())/1024/1024);
			// updata gnuplot data
      if (stats->first == 1) {
        DataSet.Add((double)(Simulator::Now().GetSeconds()-1),(double)(stats->second.rxBytes-RxBytesList[0])*8/1024/1024);
        RxBytesList[0] = stats->second.rxBytes;
      }
      if (stats->first == 3){
        DataSet1.Add((double)(Simulator::Now().GetSeconds()-1),(double)(stats->second.rxBytes-RxBytesList[1])*8/1024/1024);
        RxBytesList[1] = stats->second.rxBytes;
      }
      
			std::cout<<"---------------------------------------------------------------------------"<<std::endl;
		}
			Simulator::Schedule(Seconds(1),&ThroughputMonitor, fmhelper, flowMon,DataSet, DataSet1);
         //if(flowToXml)
      // {
	    //   flowMon->SerializeToXmlFile ("ThroughputMonitor.xml", true, true);
      // }

	}




void
ModifyLinkRate(NetDeviceContainer *ptp, DataRate lr) {
    StaticCast<PointToPointNetDevice>(ptp->Get(0))->SetDataRate(lr);
}


int
main (int argc, char *argv[])
{
  CommandLine cmd;
  cmd.Parse (argc, argv);

  std::cout
      << "\n\n#################### SIMULATION SET-UP ####################\n\n\n";
  
   Ptr<RateErrorModel> em = CreateObjectWithAttributes<RateErrorModel> (
      "RanVar", StringValue ("ns3::UniformRandomVariable[Min=0.0|Max=1.0]"),
      "ErrorRate", DoubleValue (0.0001));
      

  LogLevel log_precision = LOG_LEVEL_LOGIC;
  Time::SetResolution (Time::NS);
   LogComponentEnableAll (LOG_PREFIX_TIME);
   LogComponentEnableAll (LOG_PREFIX_FUNC);
   LogComponentEnableAll (LOG_PREFIX_NODE);
  //  LogComponentEnable ("QuicEchoClientApplication", log_precision);
  //  LogComponentEnable ("QuicEchoServerApplication", log_precision);
   LogComponentEnable ("quic-tester", log_precision);
//  LogComponentEnable ("QuicHeader", log_precision);
//     LogComponentEnable ("QuicSocketBase", log_precision);
//  LogComponentEnable ("QuicStreamBase", LOG_LEVEL_LOGIC);
//  LogComponentEnable ("Socket", log_precision);
//  LogComponentEnable ("Application", log_precision);
//  LogComponentEnable ("Node", log_precision);
//  LogComponentEnable ("InternetStackHelper", log_precision);
//  LogComponentEnable ("QuicSocketFactory", log_precision);
//  LogComponentEnable ("ObjectFactory", log_precision);
//  //LogComponentEnable ("TypeId", log_precision);
  // LogComponentEnable ("QuicL4Protocol", log_precision);
//  LogComponentEnable ("QuicL5Protocol", log_precision);
//  //LogComponentEnable ("ObjectBase", log_precision);
//  LogComponentEnable ("QuicEchoHelper", log_precision);
    // LogComponentEnable ("QuicSocketTxScheduler", log_precision);
//  LogComponentEnable ("QuicSocketRxBuffer", log_precision);
//  LogComponentEnable ("QuicHeader", log_precision);
//  LogComponentEnable ("QuicSubheader", log_precision);
//  LogComponentEnable ("Header", log_precision);
//  LogComponentEnable ("PacketMetadata", log_precision);


  // Config::SetDefault ("ns3::QuicSocketBase::EnableMultipath",BooleanValue(true));
  // Config::SetDefault ("ns3::QuicL4Protocol::SocketType",TypeIdValue (MpQuicCongestionOps::GetTypeId ()));
  // Config::SetDefault ("ns3::MpQuicScheduler::SchedulerType", IntegerValue(MpQuicScheduler::ROUND_ROBIN));   



  NodeContainer nodes;
  nodes.Create (2);
  auto n1 = nodes.Get (0);
  auto n2 = nodes.Get (1);

  int sf = 1;
  Time simulationEndTime = Seconds (10);

  int start_time = 1;
//   int x = 2;

  QuicHelper stack;
  stack.InstallQuic (nodes);

  std::vector<std::string> rate(2);
  std::vector<std::string> delay(2);
  std::vector<NetDeviceContainer> netDevices(2);
  rate[0]="10Mbps";
  delay[0]="15ms";
  rate[1]="10Mbps";
  delay[1]="15ms";

  std::vector<Ipv4InterfaceContainer> ipv4Ints;
  for(int i=0; i < sf; i++)
  {
      // Creation of the point to point link between hots
      PointToPointHelper p2plink;
      p2plink.SetDeviceAttribute ("DataRate", StringValue(rate[i]));
      p2plink.SetChannelAttribute("Delay", StringValue(delay[i]));
      // p2plink.SetQueue ("ns3::DropTailQueue", "MaxSize", StringValue ("1000p"));

      // NetDeviceContainer netDevices;
      netDevices[i] = p2plink.Install(nodes);
      // netDevices[i].Get (1)->SetAttribute ("ReceiveErrorModel", PointerValue (em));


      std::cout<<"netdevice 0 "<<netDevices[i].Get(0)
              <<"netdevice 1 "<<netDevices[i].Get(1)
              <<"\n";
      
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

  // QuicEchoServerHelper echoServer (9);

  // ApplicationContainer serverApps = echoServer.Install (nodes.Get (1));
  // serverApps.Start (Seconds (0));
  // serverApps.Stop (simulationEndTime);

  // QuicEchoClientHelper echoClient (ipv4Ints[0].GetAddress (1), 9);
  // echoClient.SetAttribute ("PacketSize", UintegerValue(1460));
  // echoClient.SetAttribute ("MaxPackets", UintegerValue (10));
  // echoClient.SetAttribute ("Interval", TimeValue (Seconds (0.2)));
  // //echoClient.SetAttribute ("MaxStreamData", UintegerValue (1024));

  // ApplicationContainer clientApps = echoClient.Install (nodes.Get (0));

  // echoClient.SetFill (clientApps.Get (0),100, 1000);
  // clientApps.Start (Seconds (1));
  // clientApps.Stop (simulationEndTime);

  uint16_t port = 9;  // well-known echo port number

  uint32_t maxBytes = 100000;
  BulkSendHelper source ("ns3::QuicSocketFactory",
                         InetSocketAddress (ipv4Ints[0].GetAddress (1), port));
  // Set the amount of data to send in bytes.  Zero is unlimited.
  source.SetAttribute ("MaxBytes", UintegerValue (maxBytes));
  // source.SetAttribute ("SendSize", UintegerValue (1000));
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


//   Simulator::Schedule (Seconds (start_time+0.0000001), &Traces, n2->GetId(),
//         "./server", ".txt");
  Simulator::Schedule (Seconds (start_time+0.0000001), &Traces, n1->GetId(),
        "./client", ".txt");

  Packet::EnablePrinting ();
  Packet::EnableChecking ();


    std::string fileNameWithNoExtension = "FlowVSThroughput_";
    std::string graphicsFileName        = fileNameWithNoExtension + ".png";
    std::string plotFileName            = fileNameWithNoExtension + ".plt";
    std::string plotTitle               = "Throughput vs Time";
    std::string dataTitle               = "path 0";
    std::string dataTitle1               = "path 1";

    // Instantiate the plot and set its title.
    Gnuplot gnuplot (graphicsFileName);
    gnuplot.SetTitle (plotTitle);

    // Make the graphics file, which the plot file will be when it
    // is used with Gnuplot, be a PNG file.
    gnuplot.SetTerminal ("png");

    // Set the labels for each axis.
    gnuplot.SetLegend ("Time(s)", "Throughput(Mbps)");

     
   Gnuplot2dDataset dataset;
   dataset.SetTitle (dataTitle);
   dataset.SetStyle (Gnuplot2dDataset::LINES_POINTS);
   Gnuplot2dDataset dataset1;
   dataset1.SetTitle (dataTitle1);
   dataset1.SetStyle (Gnuplot2dDataset::LINES_POINTS);
  FlowMonitorHelper flowmon;
  Ptr<FlowMonitor> monitor = flowmon.InstallAll ();
  //flowMonitor declaration
  // call the flow monitor function
  // std::ofstream outfile1;
  // outfile1.open("flowmonitor.txt");
  ThroughputMonitor(&flowmon, monitor, dataset, dataset1); 
  // outfile1.close();


//   for (int i=x; i < 6*x+1; i = i+x) {
//     for (int j = 0;j<5;j++) {
//       Simulator::Schedule (Seconds (2*i-x+j*0.4), &ModifyLinkRate, &netDevices[0], DataRate(std::to_string(10-j*2)+"Mbps"));
//       std::cout<<"time: "<< Seconds (2*i-x+j*0.4) <<" path0 : rate "<<std::to_string(10-j*2)<<"Mbps"<<"\n";
//       Simulator::Schedule (Seconds (2*i-x+j*0.4), &ModifyLinkRate, &netDevices[1], DataRate(std::to_string(2+j*2)+"Mbps"));
//       std::cout<<"time: "<< Seconds (2*i-x+j*0.4) <<" path1 : rate "<<std::to_string(2+j*2)<<"Mbps"<<"\n";
//     }
//     Simulator::Schedule (Seconds (2*i), &ModifyLinkRate, &netDevices[0], DataRate(std::to_string(10)+"Mbps"));
//     std::cout<<"time: "<< Seconds (2*i) <<" path0 : rate "<<std::to_string(10)<<"Mbps"<<"\n";
//     Simulator::Schedule (Seconds (2*i), &ModifyLinkRate, &netDevices[1], DataRate(std::to_string(10)+"Mbps"));
//     std::cout<<"time: "<< Seconds (2*i) <<" path1 : rate "<<std::to_string(10)<<"Mbps"<<"\n";
//   }


  


  Simulator::Stop (simulationEndTime);
  std::cout << "\n\n#################### STARTING RUN ####################\n\n";
  Simulator::Run ();

//   //Gnuplot ...continued
//   gnuplot.AddDataset (dataset);
//   gnuplot.AddDataset (dataset1);

//   dataset.Add(0,0);
//   dataset.Add(0,0);
//   // Open the plot file.
//   std::ofstream plotFile (plotFileName.c_str());
//   // Write the plot file.
//   gnuplot.GenerateOutput (plotFile);
//   // Close the plot file.
//   plotFile.close ();

  flowmon.SerializeToXmlFile("flow", false, false);

  monitor->CheckForLostPackets ();
  Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmon.GetClassifier ());
  FlowMonitor::FlowStatsContainer stats = monitor->GetFlowStats ();

  std::ofstream outfile;
  outfile.open("wmp"+std::to_string(simulationEndTime.GetSeconds())+".txt");
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
      std::cout  <<  "  Tx Bytes:   " << i->second.txBytes << "\n";
      std::cout << "  Throughput: " << i->second.rxBytes * 8.0 / simulationEndTime.GetSeconds () / 1000 / 1000  << " Mbps\n";    
      std::cout <<  "  Tx time: " << (i->second.timeLastTxPacket - i->second.timeFirstTxPacket).GetSeconds()<<"\n";  
    
    
    }

  outfile.close();
  std::cout
      << "\n\n#################### RUN FINISHED ####################\n\n\n";
  Simulator::Destroy ();

  // Ptr<PacketSink> sink1 = DynamicCast<PacketSink> (sinkApps.Get (0));
  // std::cout << "Total Bytes Received: " << sink1->GetTotalRx () << std::endl;
  // std::cout
  //     << "\n\n#################### SIMULATION END ####################\n\n\n";
  return 0;
}

