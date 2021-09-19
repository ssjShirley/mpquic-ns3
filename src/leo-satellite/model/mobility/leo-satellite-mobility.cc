/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Mobility model subclass
 * Keeps track of current position and velocity of LEO satellites, calculates distance between satellites
 *
 * ENSC 427: Communication Networks
 * Spring 2020
 * Team 11
 */

#include "leo-satellite-mobility.h"
#include "ns3/vector.h"
#include "ns3/double.h"
#include "ns3/boolean.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "ns3/integer.h"
#define _USE_MATH_DEFINES
#include <cmath>
#include <algorithm>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("LeoSatelliteMobility");

NS_OBJECT_ENSURE_REGISTERED (LeoSatelliteMobilityModel);

double earthRadius = 6378.1; // radius of Earth [km]
uint32_t currentNode = 0; // for initialization only

TypeId
LeoSatelliteMobilityModel::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::LeoSatelliteMobilityModel")
    .SetParent<MobilityModel> ()
    .SetGroupName ("Mobility")
    .AddConstructor<LeoSatelliteMobilityModel> ()
    .AddAttribute ("NPerPlane", "The number of satellites per orbital plane.",
                   IntegerValue (1),
                   MakeIntegerAccessor (&LeoSatelliteMobilityModel::m_nPerPlane),
                   MakeIntegerChecker<uint32_t> ())
    .AddAttribute ("NumberofPlanes", "The total number of orbital planes.",
                   IntegerValue (1),
                   MakeIntegerAccessor (&LeoSatelliteMobilityModel::m_numPlanes),
                   MakeIntegerChecker<uint32_t> ())
    .AddAttribute ("Latitude",
                   "Latitude of satellite.",
                   DoubleValue(1.0),
                   MakeDoubleAccessor (&LeoSatelliteMobilityModel::m_latitude),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("Longitude",
                   "Longitude of satellite. Constant for satellites in same plane.",
                   DoubleValue(1.0),
                   MakeDoubleAccessor (&LeoSatelliteMobilityModel::m_longitude),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("Time",
                   "Time when initial position of satellite is set.",
                   DoubleValue (1.0),
                   MakeDoubleAccessor (&LeoSatelliteMobilityModel::m_time),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("Altitude",
                   "Altitude of satellite. Used to determine velocity.",
                   DoubleValue(1.0),
                   MakeDoubleAccessor (&LeoSatelliteMobilityModel::m_altitude),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("Direction",
                   "Direction of satellite relative to other satellites.",
                   BooleanValue(1),
                   MakeBooleanAccessor (&LeoSatelliteMobilityModel::m_direction),
                   MakeBooleanChecker ())
  ;

  return tid;
}

LeoSatelliteMobilityModel::LeoSatelliteMobilityModel()
{
  currentNode++;
  m_current = currentNode;
}

/* To be called after MobilityModel object is created to set position.
   Input should be a NULL vector as position is determined by number of orbital planes and number of satellites per   
   orbital plane

   Node positions are set as follows:
   Node 1 is the closest to latitude = 90, longitude = -180
   Nodes in the same plane are set by decrementing latitude until -90 is reached, longitude kept the same
   The first node in the next plane is set by setting latitude = 90 and incrementing the longitude
   ...
   Node N is closest to latitude = -90, longitude = 180

   The latitude edges of +90 and -90 are populated
   The longitude edge of -180 is populated (180 is not populated as it is equivalent to -180)
 */
void 
LeoSatelliteMobilityModel::DoSetPosition (const Vector &position)
{
  // Determine speed of satellite from altitude
  double G = 6.673e-11; // gravitational constant [Nm^2/kg^2]
  double earthMass = 5.972e24; // mass of Earth [kg]
  double altitude = m_altitude;

  m_speed = std::sqrt(G*earthMass/(earthRadius*1000 + altitude*1000));

  // Set latitude and longitude of satellite from number of orbital planes and number of satellites per orbital plane
  // First satellite in plane will have a longitude that is a half-step down from 90 degrees 
  if (m_current == 1)
  {
    m_latitude = 90 - 180/(m_nPerPlane/2)/2;
  }
  else if ((fmod(m_current - 1, m_nPerPlane/2) == 0) && (m_current > m_nPerPlane))
  {
    m_latitude = 90 - 180/(m_nPerPlane/2)/2;
  }
  else
  {
     m_latitude = 90 - 180/(m_nPerPlane/2)/2 - 180/(m_nPerPlane/2)*fmod(m_current - 1, m_nPerPlane/2);
  }
  m_longitude = -180 + 360/(m_numPlanes*2)*floor((m_current - 1)/(m_nPerPlane/2));

  // Set direction based on which orbital plane satellite belongs to
  uint32_t plane = floor((m_current - 1)/(m_nPerPlane/2));
  (plane % 2 == 1) ? m_direction = 0: m_direction = 1;
}

Vector
LeoSatelliteMobilityModel::DoGetPosition (void) const
{
   double altitude = m_altitude;
   double latitude = m_latitude;
   double longitude = m_longitude;
   bool direction = m_direction; 
   double currentTime = Simulator::Now().GetSeconds();
   double radius = earthRadius + altitude; 
   double newLatitude;

   // How many orbital periods have been completed, then converted to degree displacement
   double orbitalPeriod = 2*M_PI*radius/(m_speed/1000); // [seconds]
   double orbitalPeriodTravelled = (currentTime - m_time)/orbitalPeriod;
   double degreeDisplacement = fmod(orbitalPeriodTravelled*360, 360); 

   if (direction == 1)
   {
      if ((latitude + degreeDisplacement) > 90)
      {
         degreeDisplacement = degreeDisplacement - (90 - latitude); // We've accounted for the degrees taken to get to north pole
         latitude = 90;
         if (longitude < 0)
            longitude = longitude + 180;
          else
            longitude = longitude - 180;
         direction = 0;

        if ((latitude - degreeDisplacement) < -90)
        {
          degreeDisplacement = degreeDisplacement - 180; // We've accounted for the degrees taken to get to south pole
          latitude = -90;
          if (longitude < 0)
            longitude = longitude + 180;
          else
            longitude = longitude - 180;
          direction = 1;
        }
      }
      (direction == 1) ? newLatitude = latitude + degreeDisplacement : newLatitude = latitude - degreeDisplacement;
   }

  //  else if (direction == 0)
   else
   {
      if ((latitude - degreeDisplacement) < -90)
      {
         degreeDisplacement = degreeDisplacement - (latitude - (-90)); // We've accounted for the degrees taken to get to south pole
         latitude = -90;
         if (longitude < 0)
            longitude = longitude + 180;
          else
            longitude = longitude - 180;
         direction = 1;

        if ((latitude + degreeDisplacement) > 90)
        {
          degreeDisplacement = degreeDisplacement - 180; // We've accounted for the degrees taken to get to north pole
          latitude = 90;
          if (longitude < 0)
            longitude = longitude + 180;
          else
            longitude = longitude - 180;
          direction = 0;
        }
      }
      (direction == 1) ? newLatitude = latitude + degreeDisplacement : newLatitude = latitude - degreeDisplacement;
   }
   
   // Update latitude, longitude, direction, and time values for this object
   m_latitude = newLatitude;
   m_longitude = longitude;
   m_time = currentTime;
   m_direction = direction;

   // Create vector to return
   Vector currentPosition = Vector(m_latitude, m_longitude, altitude);
   return currentPosition;
}

/* Args "a" and "b" to be obtained from LeoSatelliteMobilityModel::DoGetPosition for each argument 
   Distance calculated using Haversine formula for distance of two points on spherical surface
   Ignoring the slight ellipsoidal effects of Earth */
double 
CalculateDistance (const Vector &a, const Vector &b)
{
  double altitude = a.z;
  double radius = earthRadius + altitude;
  //a.x = latitude1
  //a.y = longitude1
  //b.x = latitude2
  //b.x = latitude2   
  // Convert to radians for use in Haversine formula
  double latitude1 = a.x*M_PI/180;
  double latitude2 = b.x*M_PI/180;
  double deltaLatitude = (b.x - a.x)*M_PI/180;
  double deltaLongitude;
  if (b.y == -180 && a.y > 0)
  {
    deltaLongitude = (180 - a.y)*M_PI/180;
  }
  else if (a.y == -180 && b.y > 0)
  {
    deltaLongitude = (b.y - 180)*M_PI/180;
  }
  else
  {
    deltaLongitude = (b.y - a.y)*M_PI/180;
  }

  // Haversine formula
  double y = pow(sin(deltaLatitude/2), 2) + cos(latitude1)*cos(latitude2)*pow(sin(deltaLongitude/2), 2);
  double z = 2*atan(sqrt(y)/sqrt(1-y));
  double distance = radius*z;

  return distance;
}

Vector
LeoSatelliteMobilityModel::DoGetVelocity (void) const
{
  Vector null = Vector(0.0, 0.0, 0.0);
  return null;
}

} // namespace ns3

