


#include "ns3/object.h"
#include "ns3/log.h"
#include "ns3/uinteger.h"
#include "ns3/double.h"
#include "ns3/boolean.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/nstime.h"
#include "quic-stream.h"
#include "ns3/node.h"
#include "mp-quic-scheduler.h"
#include "ns3/string.h"

#include <algorithm>
#include <iostream>
#include <iomanip>
#include <iterator>
#include <vector>
#include <boost/random.hpp>
#include <boost/random/discrete_distribution.hpp>
#include <boost/random/mersenne_twister.hpp>


namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("MpQuicScheduler");

NS_OBJECT_ENSURE_REGISTERED (MpQuicScheduler);


TypeId
MpQuicScheduler::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::MpQuicScheduler")
    .SetParent<Object> ()
    .SetGroupName ("Internet")
    .AddAttribute ("SchedulerType",
                   "Minimum time in the future an RTO alarm may be set for",
                   StringValue ("RR"),
                   MakeStringAccessor (&MpQuicScheduler::m_type),
                   MakeStringChecker ())
                   
  ;
  return tid;
}

MpQuicScheduler::MpQuicScheduler ()
  : Object (),
  m_socket(0),
  m_lastUsedPathId(0)
{
  NS_LOG_FUNCTION_NOARGS ();
  m_sendIndex = 0;
  m_pid = 0;

  // Probability of winning for each bandit. Change this variable to experiment
  // with different probabilities of winning.
  p = {0.5, 0.5};

  // Number of trials per bandit
  trials = std::vector<unsigned int>(p.size());
  // Number of wins per bandif
  wins = std::vector<unsigned int>(p.size());
  // Beta distributions of the priors for each bandit
  
  // Initialize the prior distributions with alpha=1 beta=1
  for (size_t i = 0; i < p.size(); i++) {
    prior_dists.push_back(boost::random::beta_distribution<>(1, 1));
  }
}

MpQuicScheduler::~MpQuicScheduler ()
{
  NS_LOG_FUNCTION_NOARGS ();
  m_sendIndex = 0;
  m_pid = 0;
}

// void
// MpQuicScheduler::SetSubflows(std::vector <MpQuicSubFlow *> subflows)
// {
//   // m_subflows = subflows;
//   return;
// }


int16_t 
MpQuicScheduler::GetNextPathIdToUse()
{
  m_subflows = m_socket->GetActiveSubflows();
  if (m_subflows.empty())
  {
    m_lastUsedPathId = 0;
    return 0;
  }
  m_lastUsedPathId = (m_lastUsedPathId + 1) % m_subflows.size();
  return m_lastUsedPathId;
}


void
MpQuicScheduler::SetSocket(Ptr<QuicSocketBase> sock)
{
  NS_LOG_FUNCTION (this);
  m_socket = sock;
}

int
MpQuicScheduler::GetSend()
{
  if(m_sendIndex == 7000)
  {
    SetSendDefault();
  }
  if (m_type == "weight")
  {
    return WeightedRoundRobin();
  } 
  else if (m_type == "rtt")
  {
    return RttBase();
  }
  else if (m_type == "bandit")
  {
    return MabAlog();
  }
  else
  {
    return RoundRobin();
  }
}


void
MpQuicScheduler::SetSendDefault()
{
  m_sendIndex = 0;
}

void
MpQuicScheduler::SetWeightType()
{
  m_type = "weight";
}

int
MpQuicScheduler::RoundRobin()
{
  m_sendIndex++;

  if(m_sendIndex%2 == 0 )
  {
    return 0;
  } 
  else 
  {
    return 1;
  }
}

int
MpQuicScheduler::WeightedRoundRobin()
{
  // std::cout<<m_sendIndex<<" index \n";
  m_sendIndex++;
  if(m_sendIndex >=2 && m_sendIndex<= 10)
  {
    return 0;
  } 
  else 
  {
    return 1;
  }
}

int
MpQuicScheduler::RttBase()
{
  m_sendIndex++;
  if(m_sendIndex > 1)
  {
    return m_pid;
  }
  if(m_sendIndex%2 == 0)
  {
    return 1;
  } 
  else 
  {
    return 0;
  }
}

void
MpQuicScheduler::SetMinPath(int p)
{
  m_pid = p;
}
// int
// MpQuicScheduler::FindMinRttPath()
// {
//   int min = 0;
//   Time mrtt=m_subflows[0]->lastMeasuredRtt;
//   for (uint i = 1; i < m_subflows.size(); i++)
//   {
//     if (m_subflows[i]->lastMeasuredRtt < mrtt){
//       mrtt = m_subflows[i]->lastMeasuredRtt;
//       min = i;
//     }
//   }
//   return min;
// }


// pull_lever has a chance of 1/weight of returning 1.
unsigned int 
MpQuicScheduler::pull_lever(base_generator *gen, double weight) {
  double probabilities[] = {1-weight, weight};
  boost::random::discrete_distribution<> dist(probabilities);
  return dist(*gen);
}

// argmax returns the index of maximum element in vector v.
size_t 
MpQuicScheduler::argmax(const std::vector<double>& v){
  return std::distance(v.begin(), std::max_element(v.begin(), v.end()));
}

int
MpQuicScheduler::MabAlog(){
  
  // gen is a Mersenne Twister random generator. We initialzie it here to keep
  // the binary deterministic.
  
  std::vector<double> priors;
  // Sample a random value from each prior distribution.
  for (auto& dist : prior_dists) {
    priors.push_back(dist(gen));
  }
  // Select the bandit that has the highest sampled value from the prior
  size_t chosen_bandit = argmax(priors);
  trials[chosen_bandit]++;
  // Pull the lever of the chosen bandit
  wins[chosen_bandit] += pull_lever(&gen, p[chosen_bandit]);

  // Update the prior distribution of the chosen bandit
  auto alpha = 1 + wins[chosen_bandit];
  auto beta = 1 + trials[chosen_bandit] - wins[chosen_bandit];
  prior_dists[chosen_bandit] = boost::random::beta_distribution<>(alpha, beta);
  
  // std::cout << "chosen bandit: " << chosen_bandit << std::endl;
  return chosen_bandit;
  // auto sp = std::cout.precision();
  // std::cout << std::setprecision(3);
  // for (size_t i = 0; i < p.size(); i++) {
  //   std::cout << "Bandit " << i+1 << ": ";
  //   double empirical_p = double(wins[i]) / trials[i];
  //   std::cout << "wins/trials: " << wins[i] << "/" << trials[i] << ". ";
  //   std::cout << "Estimated p: " << empirical_p << " ";
  //   std::cout << "Actual p: " << p[i] << std::endl;
  // }
  // std::cout << std::endl;
  // auto expected_optimal_wins = *std::max_element(p.begin(), p.end()) * runs;
  // std::cout << std::setprecision(sp);
  // std::cout << "Expected number of wins with optimal strategy: " << expected_optimal_wins << std::endl;
  // std::cout << "Actual wins: " << std::accumulate(wins.begin(), wins.end(), 0) << std::endl;

}


} // namespace ns3
