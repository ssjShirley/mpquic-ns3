/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * LEO Satellite Example
 * Runs traffic through a configurable LEO satellite constellation
 *
 * ENSC 427: Communication Networks
 * Spring 2020
 * Team 11
 */

#include "ns3/core-module.h"
#include "ns3/leo-satellite-config.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-helper.h"
#include <string>
#include <sstream>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("LeoSatelliteExample");

int 
main (int argc, char *argv[])
{
  uint32_t n_planes = 3;
  uint32_t n_sats_per_plane = 4;
  double altitude = 2000;

  CommandLine cmd;
  cmd.AddValue ("n_planes", "Number of planes in satellite constellation", n_planes);
  cmd.AddValue ("n_sats_per_plane", "Number of satellites per plane in the satellite constellation", n_sats_per_plane);
  cmd.AddValue ("altitude", "Altitude of satellites in constellation in kilometers ... must be between 500 and 2000", altitude);

  cmd.Parse (argc,argv);

  LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
  LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);

  LeoSatelliteConfig sat_network(n_planes, n_sats_per_plane, altitude);
  
  UdpEchoServerHelper echoServer (9);

  ApplicationContainer serverApps = echoServer.Install(sat_network.ground_stations.Get(1));
  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (2000.0));

  UdpEchoClientHelper echoClient (sat_network.ground_station_interfaces[1].GetAddress(0), 9);
  echoClient.SetAttribute("MaxPackets", UintegerValue (20));
  echoClient.SetAttribute("Interval", TimeValue(Seconds(100.0)));
  echoClient.SetAttribute("PacketSize", UintegerValue (1024));

  ApplicationContainer clientApps = echoClient.Install (sat_network.ground_stations.Get(0));
  clientApps.Start (Seconds (2.0));
  clientApps.Stop (Seconds (2000.0));

  FlowMonitorHelper flowmonHelper;
  flowmonHelper.InstallAll();

  for(uint32_t i=0; i<19; i++)
  {
    Simulator::Stop(Seconds(100));
    Simulator::Run();
    sat_network.UpdateLinks();
  }
  Simulator::Stop(Seconds(100));
  Simulator::Run();
 
  Simulator::Destroy ();

  std::stringstream ss;
  std::string file_title;
  ss << "leo-satellite-example-"<<n_planes<<"-"<<n_sats_per_plane<<"-"<<altitude<<".flowmon";
  ss >> file_title;

  flowmonHelper.SerializeToXmlFile (file_title, false, false);
  return 0; 
}


