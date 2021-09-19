/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Mobility model subclass
 * Keeps track of current position and velocity of LEO satellites
 *
 * ENSC 427: Communication Networks
 * Spring 2020
 * Team 11
 */
#ifndef LEO_SATELLITE_MOBILITY_H
#define LEO_SATELLITE_MOBILITY_H

#include "ns3/object.h"
#include "ns3/ptr.h"
#include "ns3/mobility-model.h"
#include "ns3/vector.h"

namespace ns3 {

/**
 * \ingroup leo-satellite
 * \brief leo-satellite mobility model.
 *
 * Each satellite moves in a polar orbit within its plane
 * Satellites move with a fixed velocity determined by their altitude
 * Satellites in adjacent planes move in opposing directions
 */
class LeoSatelliteMobilityModel : public MobilityModel
{
public:
  /**
   * Register this type with the TypeId system.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);
  LeoSatelliteMobilityModel();

private:
  virtual Vector DoGetPosition (void) const;
  virtual void DoSetPosition (const Vector &position);
  virtual Vector DoGetVelocity (void) const;
  friend double CalculateDistance (const Vector &a, const Vector &b);
  uint32_t m_current; // current node
  double m_nPerPlane; // number of satellites per plane -> m_nPerPlane/2 must be even number
  double m_numPlanes; // number of planes -> must be an odd number
  mutable double m_time; // time when current m_latitude, m_longitude, and m_direction were set
  double m_altitude; // [km]
  // The following variables are calculated automatically given the above parameteres
  mutable double m_latitude; // latitude of satellite at m_time
                     // negative value indicates southern latitude, positive value indicates northern latitude
  mutable double m_longitude; // initial longitude of satellite
                      // negative value indicates western longitude, positive value indicates eastern longitude
  mutable bool m_direction; // each adjacent plane will be orbiting in an opposite direction 
                    // 1 = S to N, 0 = N to S
  double m_speed; // [m/s]
};

} // namespace ns3

#endif /* LEO_SATELLITE_MOBILITY_H */
