
#ifndef MpQUICSCHEDULER_H
#define MpQUICSCHEDULER_H

// #include "ns3/stream.h"
#include "ns3/node.h"
#include <boost/random.hpp>
#include <boost/random/discrete_distribution.hpp>
#include <boost/random/mersenne_twister.hpp>


namespace ns3 {

/**
 * \ingroup quic
 *
 * \see MpQuicSchedulerBase
 * \brief (abstract) base class of all MpQuicSchedulers
 *
 * This class constitutes a basic implementation of a Quic Stream.
 *
 */
class MpQuicScheduler : public Object
{
public:
  /**
   * Get the type ID.
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);

  MpQuicScheduler (void);
  virtual ~MpQuicScheduler (void);


  // void SetSubflows(Ptr<QuicSocketBase> m_subflows);
  int GetSend();
  void SetSendDefault();
  void SetWeightType();
  void SetMinPath(int p);

private:
  int WeightedRoundRobin();
  int RoundRobin();
  int RttBase();
  // int FindMinRttPath();
  

  int m_sendIndex;
  std::string m_type;
  int m_pid;
  // Ptr<QuicSocketBase> m_socket;
  // std::vector <MpQuicSubFlow *> m_subflows;
  

  // Define base_generator as a Mersenne Twister. This is needed only to make the
  // code a bit less verbose.
  typedef boost::mt19937 base_generator;
  std::vector<double> p;
  std::vector<unsigned int> trials;
  std::vector<unsigned int> wins;
  std::vector<boost::random::beta_distribution<> > prior_dists;
  base_generator gen;

  // pull_lever has a chance of 1/weight of returning 1.
  unsigned int pull_lever(base_generator *gen, double weight);




  size_t argmax(const std::vector<double>& v);

  int MabAlog();
 
};

} // namespace ns3

#endif /* QUIC_STREAM_H */


