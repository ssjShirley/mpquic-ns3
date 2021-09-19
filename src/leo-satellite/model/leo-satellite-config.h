/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * LEO Satellite Constellation Config
 * Creates and maintains all satellites and links within a satellite communication network
 *
 * ENSC 427: Communication Networks
 * Spring 2020
 * Team 11
 */
#ifndef LEO_SATELLITE_CONFIG_H
#define LEO_SATELLITE_CONFIG_H

#include "ns3/vector.h"
#include "ns3/object.h"
#include "ns3/ptr.h"
#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/leo-satellite-mobility.h"
#include "ns3/ground-station-mobility.h"
#include <vector>
#include "ns3/mobility-module.h"
#include "ns3/csma-module.h"
#include <cmath>
#include "ns3/internet-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/applications-module.h"

namespace ns3 {

class LeoSatelliteConfig : public Object
{
public:
  /**
   * Register this type with the TypeId system.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);

  LeoSatelliteConfig (uint32_t num_planes, uint32_t num_satellites_per_plane, double altitude);

  virtual ~LeoSatelliteConfig ();
  virtual TypeId GetInstanceTypeId (void) const;
  
  void UpdateLinks (); //update the intersatellite links

  NodeContainer ground_stations; //node container to hold ground stations
  std::vector<Ipv4InterfaceContainer> ground_station_interfaces;

private:
  uint32_t num_planes;
  uint32_t num_satellites_per_plane;
  double m_altitude;

  std::vector<NodeContainer> plane; //node container for each plane
  std::vector<NetDeviceContainer> intra_plane_devices; //contains net devices for all P2P links for all planes
  std::vector<NetDeviceContainer> inter_plane_devices;
  std::vector<Ptr<CsmaChannel>> inter_plane_channels;
  std::vector<uint32_t> inter_plane_channel_tracker; //this will have the node from the adjacent plane that is currently connected
  std::vector<NetDeviceContainer> ground_station_devices; 
  std::vector<Ptr<CsmaChannel>> ground_station_channels;
  std::vector<uint32_t> ground_station_channel_tracker;
  std::vector<Ipv4InterfaceContainer> intra_plane_interfaces;
  std::vector<Ipv4InterfaceContainer> inter_plane_interfaces;
  
};
  
}

#endif /* LEO_SATELLITE_CONFIG_H */
