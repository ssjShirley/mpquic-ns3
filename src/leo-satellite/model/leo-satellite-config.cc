/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * LEO Satellite Constellation Config
 * Creates and maintains all satellites and links within a satellite communication network
 *
 * ENSC 427: Communication Networks
 * Spring 2020
 * Team 11
 */

#include "leo-satellite-config.h"
#include "ns3/quic-l4-protocol.h"

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (LeoSatelliteConfig);
NS_LOG_COMPONENT_DEFINE ("LeoSatelliteConfig");

extern double CalculateDistanceGroundToSat (const Vector &a, const Vector &b);

double speed_of_light = 299792458; //in m/s

//typeid
TypeId LeoSatelliteConfig::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::LeoSatelliteConfig")
  .SetParent<Object> ()
  .SetGroupName("LeoSatellite")
  ;
  return tid;
}

LeoSatelliteConfig::~LeoSatelliteConfig ()
{
}

TypeId LeoSatelliteConfig::GetInstanceTypeId (void) const
{
  TypeId tid = this->GetTypeId();
  return tid;
}

//constructor
LeoSatelliteConfig::LeoSatelliteConfig (uint32_t num_planes, uint32_t num_satellites_per_plane, double altitude)
{
  this->num_planes = num_planes;
  this->num_satellites_per_plane = num_satellites_per_plane;
  this->m_altitude = altitude;

  uint32_t total_num_satellites = num_planes*num_satellites_per_plane;
  NodeContainer temp;
  temp.Create(total_num_satellites);

  //assign mobility model to all satellites
  MobilityHelper mobility;
  mobility.SetMobilityModel ("ns3::LeoSatelliteMobilityModel",
                             "NPerPlane", IntegerValue (num_satellites_per_plane),
                             "NumberofPlanes", IntegerValue (num_planes),
                             "Altitude", DoubleValue(altitude),
                             "Time", DoubleValue(Simulator::Now().GetSeconds()));
  mobility.Install(temp);
  
  for (NodeContainer::Iterator j = temp.Begin ();
       j != temp.End (); ++j)
    {
      Ptr<Node> object = *j;
      Ptr<MobilityModel> position = object->GetObject<MobilityModel> ();
      Vector null = Vector(0.0, 0.0, 0.0);
      position->SetPosition(null); // needs to be done to initialize
      NS_ASSERT (position != 0);
    }

  //assigning nodes to e/ plane's node container as necessary
  for (uint32_t i=0; i<num_planes; i++)
  {
     NodeContainer temp_plane;
     for(uint32_t j=0; j<num_satellites_per_plane/2; j++)
     {
       Vector pos = temp.Get(i*num_satellites_per_plane/2 + j)->GetObject<MobilityModel> ()->GetPosition();
       std::cout << Simulator::Now().GetSeconds() << ": plane # "<< i << " node # " <<j<< ": x = " << pos.x << ", y = " << pos.y << ", z = " << pos.z << std::endl;
       temp_plane.Add(temp.Get(i*num_satellites_per_plane/2 + j));
     }
     for(uint32_t j=num_satellites_per_plane/2; j> 0; j--)
     {
       Vector pos = temp.Get(total_num_satellites/2 + i*num_satellites_per_plane/2 + j - 1)->GetObject<MobilityModel> ()->GetPosition();
       std::cout << Simulator::Now().GetSeconds() << ": plane # "<< i << " node # " <<num_satellites_per_plane - j<< ": x = " << pos.x << ", y = " << pos.y << ", z = " << pos.z << std::endl;
       temp_plane.Add(temp.Get(total_num_satellites/2 + i*num_satellites_per_plane/2 + j - 1));
     }
     InternetStackHelper stack;
     stack.Install(temp_plane);
     this->plane.push_back(temp_plane);
  }

  //setting up all intraplane links
  Vector nodeAPosition = this->plane[0].Get(0)->GetObject<MobilityModel>()->GetPosition();
  Vector nodeBPosition = this->plane[0].Get(1)->GetObject<MobilityModel>()->GetPosition();
  double distance = CalculateDistance(nodeAPosition, nodeBPosition);
  double delay = (distance * 1000)/speed_of_light; //should get delay in seconds
  PointToPointHelper intraplane_link_helper;
  intraplane_link_helper.SetDeviceAttribute ("DataRate", StringValue ("5.36Gbps"));
  intraplane_link_helper.SetChannelAttribute ("Delay", TimeValue(Seconds (delay)));

  std::cout<<"Setting up intra-plane links with distance of "<<distance<<" km and delay of "<<delay<<" seconds."<<std::endl;

  for (uint32_t i=0; i<num_planes; i++)
  {
    for (uint32_t j=0; j<num_satellites_per_plane; j++)
    {
      this->intra_plane_devices.push_back(intraplane_link_helper.Install(plane[i].Get(j), plane[i].Get((j+1)%num_satellites_per_plane)));
      std::cout<<"Plane "<<i<<": channel between node "<<j<<" and node "<<(j+1)%num_satellites_per_plane<<std::endl;
    }
  }

  //setting up interplane links
  std::cout<<"Setting up inter-plane links"<<std::endl;
  for (uint32_t i=0; i<num_planes; i++)
  {
    for (uint32_t j=0; j<num_satellites_per_plane; j++)
    {
      uint32_t nodeBIndex;
      (i == num_planes - 1) ? nodeBIndex = num_satellites_per_plane - j - 1: nodeBIndex = j;
      Vector nodeAPos = this->plane[i].Get(j)->GetObject<MobilityModel>()->GetPosition();
      Vector nodeBPos = this->plane[(i+1)%num_planes].Get(nodeBIndex)->GetObject<MobilityModel>()->GetPosition();
      double distance = CalculateDistance(nodeAPos, nodeBPos);
      double delay = (distance*1000)/speed_of_light;
      CsmaHelper interplane_link_helper;
      interplane_link_helper.SetChannelAttribute("DataRate", StringValue ("5.36Gbps"));
      interplane_link_helper.SetChannelAttribute("Delay", TimeValue(Seconds(delay)));

      std::cout<<"Channel open between plane "<<i<<" satellite "<<j<<" and plane "<<(i+1)%num_planes<<" satellite "<<nodeBIndex<< " with distance "<<distance<< "km and delay of "<<delay<<" seconds"<<std::endl;

      NodeContainer temp_node_container;
      temp_node_container.Add(this->plane[i].Get(j));
      temp_node_container.Add(this->plane[(i+1)%num_planes]);
      NetDeviceContainer temp_netdevice_container;
      temp_netdevice_container = interplane_link_helper.Install(temp_node_container);
      Ptr<CsmaChannel> csma_channel;
      Ptr<Channel> channel;
      channel = temp_netdevice_container.Get(0)->GetChannel();
      csma_channel = channel->GetObject<CsmaChannel> ();
      
      for (uint32_t k=0; k<num_satellites_per_plane; k++)
      {
        if (j != k)
        {
          csma_channel->Detach(temp_netdevice_container.Get(k+1)->GetObject<CsmaNetDevice> ());
        }
      }
        
      this->inter_plane_devices.push_back(temp_netdevice_container);
      this->inter_plane_channels.push_back(csma_channel);
      this->inter_plane_channel_tracker.push_back(nodeBIndex);
    }
  }

  //setting up two ground stations for now
  std::cout << "Setting up two ground stations" << std::endl;
  ground_stations.Create(2);
  //assign mobility model to ground stations
  MobilityHelper groundMobility;
  groundMobility.SetMobilityModel ("ns3::GroundStationMobilityModel",
                             "NPerPlane", IntegerValue (num_satellites_per_plane),
                             "NumberofPlanes", IntegerValue (num_planes));
  groundMobility.Install(ground_stations);
  //Install IP stack
  InternetStackHelper stack;
  stack.SetTcp ("ns3::QuicL4Protocol");
  stack.Install(ground_stations);
  for (int j = 0; j<2; j++)
  {
    Vector temp = ground_stations.Get(j)->GetObject<MobilityModel> ()->GetPosition();
    std::cout << Simulator::Now().GetSeconds() << ": ground station # " << j << ": x = " << temp.x << ", y = " << temp.y << std::endl;
  }
  //setting up links between ground stations and their closest satellites
  std::cout<<"Setting links between ground stations and satellites"<<std::endl;
  for (uint32_t i=0; i<2; i++)
  {
    Vector gndPos = ground_stations.Get(i)->GetObject<MobilityModel> ()->GetPosition();
    uint32_t closestAdjSat = 0;
    uint32_t closestAdjSatDist = 0;
    uint32_t planeIndex;
    if (i == 0)
      planeIndex = 0;
    else
      planeIndex = floor(3*num_planes/7);
    //find closest adjacent satellite for ground station
    for (uint32_t j=0; j<this->num_satellites_per_plane; j++)
    {
      Vector pos = this->plane[planeIndex].Get(j)->GetObject<MobilityModel>()->GetPosition();
      double temp_dist = CalculateDistanceGroundToSat(gndPos,pos);
      if((temp_dist < closestAdjSatDist) || (j==0))
      {
        closestAdjSatDist = temp_dist;
        closestAdjSat = j;
      }
    }
    double delay = (closestAdjSatDist*1000)/speed_of_light;
    CsmaHelper ground_station_link_helper;
    ground_station_link_helper.SetChannelAttribute("DataRate", StringValue ("5.36Gbps"));
    ground_station_link_helper.SetChannelAttribute("Delay", TimeValue(Seconds(delay)));

    std::cout<<"Channel open between ground station " << i << " and plane " << planeIndex << " satellite "<<closestAdjSat<<" with distance "<<closestAdjSatDist<< "km and delay of "<<delay<<" seconds"<<std::endl;

    NodeContainer temp_node_container;
    temp_node_container.Add(ground_stations.Get(i));
    temp_node_container.Add(this->plane[planeIndex]);
    NetDeviceContainer temp_netdevice_container;
    temp_netdevice_container = ground_station_link_helper.Install(temp_node_container);
    Ptr<CsmaChannel> csma_channel;
    Ptr<Channel> channel;
    channel = temp_netdevice_container.Get(0)->GetChannel();
    csma_channel = channel->GetObject<CsmaChannel> ();

    for (uint32_t k=0; k<num_satellites_per_plane; k++)
    {
      if (closestAdjSat != k)
      {
        csma_channel->Detach(temp_netdevice_container.Get(k+1)->GetObject<CsmaNetDevice> ());
      }
    }
        
    this->ground_station_devices.push_back(temp_netdevice_container);
    this->ground_station_channels.push_back(csma_channel);
    this->ground_station_channel_tracker.push_back(closestAdjSat);
  }

  //Configure IP Addresses for all NetDevices
  Ipv4AddressHelper address;
  address.SetBase ("10.1.0.0", "255.255.255.0");

  //configuring IP Addresses for IntraPlane devices
  for(uint32_t i=0; i< this->intra_plane_devices.size(); i++)
  {
    address.NewNetwork();
    this->intra_plane_interfaces.push_back(address.Assign(this->intra_plane_devices[i]));
  }
  
  //configuring IP Addresses for InterPlane devices
  for(uint32_t i=0; i< this->inter_plane_devices.size(); i++)
  {
    address.NewNetwork();
    this->inter_plane_interfaces.push_back(address.Assign(this->inter_plane_devices[i]));
    for(uint32_t j=1; j<= this->num_satellites_per_plane; j++)
    {
      if(j != this->inter_plane_channel_tracker[i] + 1)
      {
        std::pair< Ptr< Ipv4 >, uint32_t > interface = this->inter_plane_interfaces[i].Get(j);
        interface.first->SetDown(interface.second);
      }
    }
  }

  //configuring IP Addresses for Ground devices
  for(uint32_t i=0; i< this->ground_station_devices.size(); i++)
  {
    address.NewNetwork();
    this->ground_station_interfaces.push_back(address.Assign(this->ground_station_devices[i]));
    for(uint32_t j=1; j<= this->num_satellites_per_plane; j++)
    {
      if(j != this->ground_station_channel_tracker[i] + 1)
      {
        std::pair< Ptr< Ipv4 >, uint32_t > interface = this->ground_station_interfaces[i].Get(j);
        interface.first->SetDown(interface.second);
      }
    }
  }

  //Populate Routing Tables
  std::cout<<"Populating Routing Tables"<<std::endl;
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
  std::cout<<"Finished Populating Routing Tables"<<std::endl;

  // Set up packet sniffing for entire network
  /*CsmaHelper csma;
  for (uint32_t i=0; i< this->inter_plane_devices.size(); i++)
  {
    csma.EnablePcap("inter-sniff", this->inter_plane_devices[i].Get(1), true);
  }
  PointToPointHelper p2p;
  for(uint32_t i=0; i< this->intra_plane_devices.size(); i++)
  {
    p2p.EnablePcap("intra-sniff", this->intra_plane_devices[i].Get(1), true);
  }
  for(uint32_t i=0; i< this->ground_station_devices.size(); i++)
  {
    p2p.EnablePcap("ground-sniff", this->ground_station_devices[i].Get(1), true);
  }*/
}

void LeoSatelliteConfig::UpdateLinks()
{
  std::cout<<std::endl<<std::endl<<std::endl<<"Updating Links"<<std::endl;

  std::vector<NodeContainer> update_links_plane = this->plane;
  NodeContainer final_plane;
  for(uint32_t j=0; j<num_satellites_per_plane; j++)
  {
    final_plane.Add(this->plane[0].Get(num_satellites_per_plane - j - 1));
  }
  update_links_plane.push_back(final_plane);
 
  for (uint32_t i=0; i<this->num_planes; i++)
  {
    Vector refSatPos;
    uint32_t refSat = 0;
    //find reference satellite (closest to equator)
    for (uint32_t j=0; j<this->num_satellites_per_plane; j++)
    {
      Vector pos = update_links_plane[i].Get(j)->GetObject<MobilityModel>()->GetPosition();
      if ((std::abs(pos.x) < std::abs(refSatPos.x)) || j == 0)
      {
        refSatPos = pos;
        refSat = j;
      }
    }

    //find the closest adjacent satellite to the reference satellite
    uint32_t closestAdjSat = 0;
    double closestAdjSatDist = 0;
    Vector adjPos; //debug
    for (uint32_t j=0; j<this->num_satellites_per_plane; j++)
    {
      Vector pos = update_links_plane[i+1].Get(j)->GetObject<MobilityModel>()->GetPosition();
      double temp_dist = CalculateDistance(refSatPos,pos);
      if((temp_dist < closestAdjSatDist) || (j==0))
      {
        closestAdjSatDist = temp_dist;
        closestAdjSat = j;
        adjPos = pos; //debug
      }
    }

    //calculate the reference increment factor for adjacent satellites in a plane
    uint32_t ref_incr;
    (refSat <= closestAdjSat) ? (ref_incr = closestAdjSat - refSat) : (ref_incr = this->num_satellites_per_plane - refSat + closestAdjSat);

    //update all adjacent satellites for this plane
    for (uint32_t j=0; j<this->num_satellites_per_plane; j++)
    {
      uint32_t access_idx = i*(this->num_satellites_per_plane) + j;
      uint32_t currAdjNodeID = this->inter_plane_channel_tracker[access_idx];
      uint32_t nextAdjNodeID = (j + ref_incr)%(this->num_satellites_per_plane);
      double nextAdjNodeDist;

      Vector constNodePos = update_links_plane[i].Get(j)->GetObject<MobilityModel>()->GetPosition();
      Vector nextAdjNodePos = update_links_plane[(i+1)/*%(this->num_planes)*/].Get(nextAdjNodeID)->GetObject<MobilityModel>()->GetPosition();

      nextAdjNodeDist = CalculateDistance(constNodePos, nextAdjNodePos);

      if (i == this->num_planes - 1)
        nextAdjNodeID = num_satellites_per_plane - nextAdjNodeID - 1;

      if(currAdjNodeID == nextAdjNodeID)
      {
        double new_delay = (nextAdjNodeDist*1000)/speed_of_light;
        this->inter_plane_channels[access_idx]->SetAttribute("Delay", TimeValue(Seconds(new_delay)));
        std::cout<<"Channel updated between plane "<<i<<" satellite "<<j<<" and plane "<<(i+1)%num_planes<<" satellite "<<nextAdjNodeID<< " with distance "<<nextAdjNodeDist<< "km and delay of "<<new_delay<<" seconds"<<std::endl;
      }
      else
      {
        this->inter_plane_channels[access_idx]->Detach(this->inter_plane_devices[access_idx].Get(currAdjNodeID+1)->GetObject<CsmaNetDevice> ());
        std::pair< Ptr< Ipv4 >, uint32_t> interface = this->inter_plane_interfaces[access_idx].Get(currAdjNodeID+1);
        interface.first->SetDown(interface.second);
        this->inter_plane_channels[access_idx]->Reattach(this->inter_plane_devices[access_idx].Get(nextAdjNodeID+1)->GetObject<CsmaNetDevice> ());
        interface = this->inter_plane_interfaces[access_idx].Get(nextAdjNodeID+1);
        interface.first->SetUp(interface.second);
        this->inter_plane_channel_tracker[access_idx] = nextAdjNodeID;
        double new_delay = (nextAdjNodeDist*1000)/speed_of_light;
        this->inter_plane_channels[access_idx]->SetAttribute("Delay", TimeValue(Seconds(new_delay)));
        std::cout<<"New channel between plane "<<i<<" satellite "<<j<<" and plane "<<(i+1)%num_planes<<" satellite "<<nextAdjNodeID<< " with distance "<<nextAdjNodeDist<< "km and delay of "<<new_delay<<" seconds"<<std::endl;
      }
    }
  }

  //updating links between ground stations and their closest satellites
  for (uint32_t i=0; i<2; i++)
  {
    Vector gndPos = ground_stations.Get(i)->GetObject<MobilityModel> ()->GetPosition();
    uint32_t closestAdjSat = 0;
    uint32_t closestAdjSatDist = 0;
    uint32_t planeIndex;
    if (i == 0)
      planeIndex = 0;
    else
      planeIndex = floor(3*num_planes/7);
    //find closest adjacent satellite for ground station
    for (uint32_t j=0; j<this->num_satellites_per_plane; j++)
    {
      Vector pos = this->plane[planeIndex].Get(j)->GetObject<MobilityModel>()->GetPosition();
      double temp_dist = CalculateDistanceGroundToSat(gndPos,pos);
      if((temp_dist < closestAdjSatDist) || (j==0))
      {
        closestAdjSatDist = temp_dist;
        closestAdjSat = j;
      }
    }

    uint32_t currAdjNodeID = this->ground_station_channel_tracker[i];
    if(currAdjNodeID == closestAdjSat)
    {
      double new_delay = (closestAdjSatDist*1000)/speed_of_light;
      this->ground_station_channels[i]->SetAttribute("Delay", TimeValue(Seconds(new_delay)));
      std::cout<<"Channel updated between ground station "<<i<<" and plane "<<planeIndex<<" satellite "<<closestAdjSat<< " with distance "<<closestAdjSatDist<< "km and delay of "<<new_delay<<" seconds"<<std::endl;
      }
      else
      {
        this->ground_station_channels[i]->Detach(this->ground_station_devices[i].Get(currAdjNodeID+1)->GetObject<CsmaNetDevice> ());
        std::pair< Ptr< Ipv4 >, uint32_t> interface = this->ground_station_interfaces[i].Get(currAdjNodeID+1);
        interface.first->SetDown(interface.second);
        this->ground_station_channels[i]->Reattach(this->ground_station_devices[i].Get(closestAdjSat+1)->GetObject<CsmaNetDevice> ());
        interface = this->ground_station_interfaces[i].Get(closestAdjSat+1);
        interface.first->SetUp(interface.second);
        this->ground_station_channel_tracker[i] = closestAdjSat;
        double new_delay = (closestAdjSatDist*1000)/speed_of_light;
        this->ground_station_channels[i]->SetAttribute("Delay", TimeValue(Seconds(new_delay)));
        std::cout<<"New channel between ground station "<<i<<" and plane "<<planeIndex<<" satellite "<<closestAdjSat<< " with distance "<<closestAdjSatDist<< "km and delay of "<<new_delay<<" seconds"<<std::endl;
      }
  }
  
  //Recompute Routing Tables
  std::cout<<"Recomputing Routing Tables"<<std::endl;
  Ipv4GlobalRoutingHelper::RecomputeRoutingTables ();
  std::cout<<"Finished Recomputing Routing Tables"<<std::endl;
}

}

