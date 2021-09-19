/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Testing mobility class
 *
 * ENSC 427: Communication Networks
 * Spring 2020
 * Team 11
 */

#include "ns3/core-module.h"
#include "ns3/mobility-module.h"
#include "ns3/leo-satellite-mobility.h"
#include "ns3/node-container.h"
#include "ns3/vector.h"

using namespace ns3;

/* Test function to print current position of satelites after simulation has run for a specified time */
void
PrintCurrentPosition (Ptr<const MobilityModel> mobility)
{
  Vector currentPos = mobility->GetPosition ();
  std::cout << Simulator::Now().GetSeconds() << ": x = " << currentPos.x << ", y = " << currentPos.y << ", z = " << currentPos.z << std::endl;
}

/* Test function to print distance between two nodes given their MobilityModelHelpers */
void
PrintDistanceBetween(Ptr<const MobilityModel> nodeA, uint32_t numA, Ptr<const MobilityModel> nodeB, uint32_t numB)
{
  Vector nodeAPosition = nodeA->GetPosition();
  Vector nodeBPosition = nodeB->GetPosition();
  double distance = CalculateDistance(nodeAPosition, nodeBPosition);
  std::cout << "Node " << numA << " position: " << nodeAPosition.x << ", " << nodeAPosition.y << ", " << nodeAPosition.z << std::endl;
  std::cout << "Node " << numB << " position: " << nodeBPosition.x << ", " << nodeBPosition.y << ", " << nodeBPosition.z << std::endl;
  std::cout << "Distance between node " << numA << " and node " << numB << " is " << distance << " km" << std::endl;
}

int 
main (int argc, char *argv[])
{
  CommandLine cmd;
  cmd.Parse (argc, argv);

  NodeContainer c = NodeContainer();
  c.Create (50); // This needs to be equal NPerPlane*NumberofPlanes in mobility model for proper configuration

  MobilityHelper mobility;

  mobility.SetMobilityModel ("ns3::LeoSatelliteMobilityModel",
                             "NPerPlane", IntegerValue (12),
                             "NumberofPlanes", IntegerValue (5),
                             "Altitude", DoubleValue(2000.0),
                             "Time", DoubleValue(Simulator::Now().GetSeconds()));

  mobility.InstallAll ();

  for (NodeContainer::Iterator j = c.Begin ();
       j != c.End (); ++j)
    {
      Ptr<Node> object = *j;
      Ptr<MobilityModel> position = object->GetObject<MobilityModel> ();
      Vector null = Vector(0.0, 0.0, 0.0);
      position->SetPosition(null); // needs to be done to initialize
      NS_ASSERT (position != 0);

      // Printing initial position
      Vector pos = position->GetPosition ();
      std::cout << Simulator::Now().GetSeconds() << ": x = " << pos.x << ", y = " << pos.y << ", z = " << pos.z << std::endl;
    }


  Simulator::Stop (Seconds (10));

  Simulator::Run ();

  // Printing positions after simulator has run for 10s
  for (NodeContainer::Iterator j = c.Begin ();
       j != c.End (); ++j)
    {
      Ptr<Node> object = *j;
      Ptr<MobilityModel> position = object->GetObject<MobilityModel> ();
      PrintCurrentPosition(position);
    }

  /* Obtaining distances between the first node and all the other nodes
     Can be expanded to show distances between each node and all other nodes through a nested for loop */
  NodeContainer::Iterator i = c.Begin();
  Ptr<Node> firstNode = *i;
  Ptr<MobilityModel> firstNodePosition = firstNode->GetObject<MobilityModel> ();
  uint32_t neighbourNodeCount = 1;

  for (NodeContainer::Iterator j = c.Begin ();
       j != c.End (); ++j)
    {
      Ptr<Node> object = *j;
      Ptr<MobilityModel> position = object->GetObject<MobilityModel> ();
      PrintDistanceBetween(firstNodePosition, 1, position, neighbourNodeCount);
      neighbourNodeCount++;
    }


  // Running simulation for another 10 seconds
  Simulator::Stop (Seconds (10));
  
  Simulator::Run ();

  // Printing positions after simulator has run for 20s
  for (NodeContainer::Iterator j = c.Begin ();
       j != c.End (); ++j)
    {
      Ptr<Node> object = *j;
      Ptr<MobilityModel> position = object->GetObject<MobilityModel> ();
      PrintCurrentPosition(position);
    }

  Simulator::Destroy ();
  return 0;
}
