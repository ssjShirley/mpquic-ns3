/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Mobility model subclass
 * Keeps track of current position of ground stations and distance from satellites
 *
 * ENSC 427: Communication Networks
 * Spring 2020
 * Team 11
 */

#include "ground-station-mobility.h"
#include "ns3/vector.h"
#include "ns3/double.h"
#include "ns3/log.h"
#include "ns3/integer.h"
#define _USE_MATH_DEFINES
#include <cmath>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("GroundStationMobility");

NS_OBJECT_ENSURE_REGISTERED (GroundStationMobilityModel);

uint32_t current = 0; // to know if we are setting up first or second ground station

TypeId
GroundStationMobilityModel::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::GroundStationMobilityModel")
    .SetParent<MobilityModel> ()
    .SetGroupName ("Mobility")
    .AddConstructor<GroundStationMobilityModel> ()
    .AddAttribute ("NPerPlane", "The number of satellites per orbital plane.",
                   IntegerValue (1),
                   MakeIntegerAccessor (&GroundStationMobilityModel::m_nPerPlane),
                   MakeIntegerChecker<uint32_t> ())
    .AddAttribute ("NumberofPlanes", "The total number of orbital planes.",
                   IntegerValue (1),
                   MakeIntegerAccessor (&GroundStationMobilityModel::m_numPlanes),
                   MakeIntegerChecker<uint32_t> ())
    .AddAttribute ("Latitude",
                   "Latitude of ground station.",
                   DoubleValue(1.0),
                   MakeDoubleAccessor (&GroundStationMobilityModel::m_latitude),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("Longitude",
                   "Longitude of ground station.",
                   DoubleValue(1.0),
                   MakeDoubleAccessor (&GroundStationMobilityModel::m_longitude),
                   MakeDoubleChecker<double> ())
  ;

  return tid;
}

GroundStationMobilityModel::GroundStationMobilityModel()
{
}

/* To be called after MobilityModel object is created to set position.
   Input should be a NULL vector as position is determined by number of orbital planes and number of satellites per   
   orbital plane
   Both ground stations are set along the longitude of a satellite's orbit (not at the same longitude), and at random 
   latitudes   
 */
void 
GroundStationMobilityModel::DoSetPosition (const Vector &position)
{
  current++;
  if (current == 1) // first ground station
  {
    m_latitude = 90 - 180/(m_nPerPlane/2)/2 ;
    m_longitude = -180;
  }
  else // second ground station
  {
    m_latitude = 90 - 180/(m_nPerPlane/2)/2 - 180/(m_nPerPlane/2)*(m_nPerPlane/4);
    m_longitude = -180 + 360/(m_numPlanes*2)*floor(3*m_numPlanes/7);
  }
}

Vector
GroundStationMobilityModel::DoGetPosition (void) const
{
  Vector currentPosition = Vector(m_latitude, m_longitude, 0);
  return currentPosition;
}

/* Vector a is the position of the ground station
   Vector b is the position of the LEO satellite */
double 
CalculateDistanceGroundToSat (const Vector &a, const Vector &b)
{
  double earthRadius = 6378.1; // radius of Earth [km]
  double distance;
  double deltaLatitude = (b.x - a.x)*M_PI/180;
  double deltaLongitude;
  if((b.y == -180 && a.y == -180) || (b.y == 0 && a.y == 0))
  {
    deltaLongitude = abs(a.y - b.y)*M_PI/180;
  }
  else if (b.y == -180)
  {
    deltaLongitude = std::min(std::abs(b.y - a.y), std::abs(0-a.y))*M_PI/180;
  }
  else if (a.y == -180)
  {
    deltaLongitude = std::min(std::abs(b.y - a.y), std::abs(b.y - 0))*M_PI/180;
  }
  else if (b.y == 0)
  {
     deltaLongitude = std::min(std::abs(b.y - a.y), std::abs(180 - a.y))*M_PI/180;
  }
  else if (a.y == 0)
  {
     deltaLongitude = std::min(std::abs(b.y - a.y), std::abs(b.y - 180))*M_PI/180;
  }
  else
  {
    deltaLongitude = abs(a.y - b.y)*M_PI/180;
  }

  distance = pow(earthRadius + b.z - earthRadius*cos(deltaLongitude)*cos(deltaLatitude), 2) + pow(earthRadius*sin(deltaLongitude)*cos(deltaLatitude), 2) + pow(earthRadius*sin(deltaLatitude), 2);
  distance = sqrt(distance);

  return distance;
}

Vector
GroundStationMobilityModel::DoGetVelocity (void) const
{
  Vector null = Vector(0.0, 0.0, 0.0);
  return null;
}

} // namespace ns3
