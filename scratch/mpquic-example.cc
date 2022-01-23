/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "ns3/core-module.h"
// #include "ns3/mpquic-helper.h"
// #include "ns3/mpquic-socket-factory.h"
#include "ns3/point-to-point-module.h"
#include "ns3/internet-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
// #include "ns3/error-model.h"
// #include "ns3/tcp-header.h"
// #include "ns3/udp-header.h"
// #include "ns3/enum.h"
// #include "ns3/event-id.h"
// #include "ns3/flow-monitor-helper.h"
// #include "ns3/ipv4-global-routing-helper.h"
// #include "ns3/traffic-control-module.h"

using namespace ns3;


int 
main (int argc, char *argv[])
{
  bool verbose = true;
  

  CommandLine cmd (__FILE__);
  cmd.AddValue ("verbose", "Tell application to log if true", verbose);

  cmd.Parse (argc,argv);

  LogLevel log_precision = LOG_LEVEL_LOGIC;
  Time::SetResolution (Time::NS);
   LogComponentEnableAll (LOG_PREFIX_TIME);
   LogComponentEnableAll (LOG_PREFIX_FUNC);
   LogComponentEnableAll (LOG_PREFIX_NODE);
   LogComponentEnable ("MpquicSocketBase", log_precision);
  //  LogComponentEnable ("MpquicL4Protocol", log_precision);
  //  LogComponentEnable ("MpquicStreamBase", log_precision);
  //  LogComponentEnable ("MpquicL5Protocol", log_precision);
  //  LogComponentEnable ("BulkSendApplication", log_precision);
  //  LogComponentEnable ("PacketSink", log_precision);
  // LogComponentEnable ("QuicSocketBase", log_precision);
  //  LogComponentEnable ("QuicL4Protocol", log_precision);
  //  LogComponentEnable ("MpquicSocketTxBuffer", log_precision);
  //  LogComponentEnable ("QuicL5Protocol", log_precision);
  

  Config::SetDefault ("ns3::QuicSocketBase::IdleTimeout", TimeValue (Seconds (300)));
  // Config::SetDefault ("ns3::QuicSocketBase::MaxStreamIdUni", UintegerValue (1));

  int sf = 2;
  std::vector<std::string> rate(2);
  std::vector<std::string> delay(2);
  std::vector<NetDeviceContainer> netDevices(2);


  NodeContainer nodes;
  nodes.Create (2);
  auto n1 = nodes.Get (0);
  auto n2 = nodes.Get (1);

  // MpquicHelper stack;
  // stack.InstallMpquic (nodes);

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

      // NetDeviceContainer netDevices;
      netDevices[i] = p2plink.Install(nodes);

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


  // PointToPointHelper pointToPoint;
  // pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("10Mbps"));
  // pointToPoint.SetChannelAttribute ("Delay", StringValue ("50ms"));

  // NetDeviceContainer devices;
  // devices = pointToPoint.Install (nodes);


  
  
  // Ipv4AddressHelper address;
  // address.SetBase ("10.1.1.0", "255.255.255.0");

  // Ipv4InterfaceContainer interfaces = address.Assign (devices);

  // pointToPoint.EnablePcap ("prueba_mpquic" , nodes, true);


  // uint16_t port = 9;  // well-known echo port number

  // uint32_t maxBytes = 10000;
  // BulkSendHelper source ("ns3::MpquicSocketFactory",
  //                        InetSocketAddress (ipv4Ints[0].GetAddress(1), port));
  // // Set the amount of data to send in bytes.  Zero is unlimited.
  // source.SetAttribute ("MaxBytes", UintegerValue (maxBytes));
  // ApplicationContainer sourceApps;
  // sourceApps.Add(source.Install (nodes.Get (0)));
  // sourceApps.Start (Seconds (1.0));
  // sourceApps.Stop (Seconds (5.0));


  // // Create a PacketSinkApplication
  // PacketSinkHelper sink ("ns3::MpquicSocketFactory",
  //                        InetSocketAddress (Ipv4Address::GetAny (), port));
  // ApplicationContainer sinkApps;
  // sinkApps.Add(sink.Install (nodes.Get(1)));
  // sinkApps.Start (Seconds (0.0));
  // sinkApps.Stop (Seconds (5.0));


  Simulator::Stop (Seconds (5.0));
  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}


