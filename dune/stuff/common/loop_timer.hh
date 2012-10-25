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
  MovingAverage avg_time_per_iteration_;
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
    avg_time_per_iteration_ += weight_.apply(std::abs( step_timer_.delta() ), iteration_, iteration_count_);
    const long remaining_steps = iteration_count_ - iteration_;
    const double remaining_ms = remaining_steps * double(avg_time_per_iteration_);
    boost::posix_time::time_duration diff = boost::posix_time::microseconds(remaining_ms);
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
/** Copyright (c) 2012, Rene Milk
   * All rights reserved.
   *
   * Redistribution and use in source and binary forms, with or without
   * modification, are permitted provided that the following conditions are met:
   *
   * 1. Redistributions of source code must retain the above copyright notice, this
   *    list of conditions and the following disclaimer.
   * 2. Redistributions in binary form must reproduce the above copyright notice,
   *    this list of conditions and the following disclaimer in the documentation
   *    and/or other materials provided with the distribution.
   *
   * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
   * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
   * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
   * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
   * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
   * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
   *
   * The views and conclusions contained in the software and documentation are those
   * of the authors and should not be interpreted as representing official policies,
   * either expressed or implied, of the FreeBSD Project.
   **/
