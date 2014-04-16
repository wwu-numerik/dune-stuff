// This file is part of the dune-stuff project:
//   http://users.dune-project.org/projects/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#ifndef DUNE_STUFF_LOOP_TIMER_HH
#define DUNE_STUFF_LOOP_TIMER_HH

#include "math.hh"
#include "profiler.hh"

#include <dune/common/exceptions.hh>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace Dune {
namespace Stuff {
namespace Common {

struct IdentityWeights
{
  double apply(const double to_weigh, const int /*current*/, const int /*max*/) {
    return to_weigh;
  }
};

struct LinearWeights
{
  double apply(const double to_weigh, const int current, const int max) {
    return to_weigh / ( current / double(max) );
  }
};

struct QuadraticWeights
{
  double apply(const double to_weigh, const int current, const int max) {
    return to_weigh / std::pow(current / double(max), 2.0);
  }
};

struct ProgressiveWeights
{
  const int prog_;
  ProgressiveWeights(const int prog)
    : prog_(prog) {}
  double apply(const double to_weigh, const int /*current*/, const int /*max*/) {
    return to_weigh * prog_;
  }
};

/** \brief helper class to estimate time needed to complete a loop with given counter
  **/
template< class CounterType, class WeightType = IdentityWeights >
class LoopTimer
{
  typedef LoopTimer< CounterType, WeightType >
    ThisType;
  CounterType& counter_;
  const int iteration_count_;
  int iteration_;
  std::ostream& output_stream_;
  WeightType weight_;
  MinMaxAvg<double> avg_time_per_iteration_;
  TimingData step_timer_;

public:
  LoopTimer( CounterType& counter, const int iteration_count, std::ostream& output_stream = std::cout,
             WeightType weight = WeightType() )
    : counter_(counter)
    , iteration_count_(iteration_count)
    , iteration_(0)
    , output_stream_(output_stream)
    , weight_(weight)
  {}

  ThisType& operator++() {
    ++iteration_;
    step_timer_.stop();
    avg_time_per_iteration_(weight_.apply(std::abs( step_timer_.delta() ), iteration_, iteration_count_));
    const long remaining_steps = iteration_count_ - iteration_;
    const double remaining_ms = remaining_steps * avg_time_per_iteration_.average();
    const boost::posix_time::time_duration diff = boost::posix_time::microseconds(remaining_ms);
    boost::posix_time::ptime target = boost::posix_time::second_clock::local_time();
    target += diff;
    output_stream_ << boost::format("\n---\n Total Time remaining: %s -- %s (%f %%)\n---\n")
                            % boost::posix_time::to_simple_string(diff)
                            % boost::posix_time::to_simple_string(target)
                            % ( 100 * ( remaining_steps / double(iteration_count_) ) )
                   << std::endl;
    step_timer_ = TimingData();
    ++counter_;
    return *this;
  } // ++
};

} // namespace Common
} // namespace Stuff
} // namespace Dune

#endif // DUNE_STUFF_LOOP_TIMER_HH
