// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//
// Contributors: Sven Kaulmann

#ifndef __EXECUTIONTIMER_HH__
#define __EXECUTIONTIMER_HH__

#include <dune/common/deprecated.hh>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace Dune {
namespace Stuff {
namespace Common {

class DUNE_DEPRECATED_MSG("Use Profiler instead") ExecutionTimer
{
public:
  static void startTimer() {
    startt_ = boost::posix_time::microsec_clock::local_time();
    return;
  }

  static double stopTimerSeconds() {
    stopt_ = boost::posix_time::microsec_clock::local_time();
    boost::posix_time::time_duration duration = stopt_ - startt_;
    return duration.total_seconds();
  }

  static double stopTimerMilliseconds() {
    stopt_ = boost::posix_time::microsec_clock::local_time();
    boost::posix_time::time_duration duration = stopt_ - startt_;
    return duration.total_milliseconds();
  }

  static double stopTimerMicroseconds() {
    stopt_ = boost::posix_time::microsec_clock::local_time();
    boost::posix_time::time_duration duration = stopt_ - startt_;
    return duration.total_microseconds();
  }

private:
  static boost::posix_time::ptime startt_;
  static boost::posix_time::ptime stopt_;
};

boost::posix_time::ptime ExecutionTimer::startt_;
boost::posix_time::ptime ExecutionTimer::stopt_;

#define EXECUTETIMEDSECONDS(command) \
  ( ExecutionTimer::startTimer(), command, ExecutionTimer::stopTimerSeconds() )

#define EXECUTETIMEDMILLISECONDS(command) \
  ( ExecutionTimer::startTimer(), command, ExecutionTimer::stopTimerMilliseconds() )

#define EXECUTETIMEDMICROSECONDS(command) \
  ( ExecutionTimer::startTimer(), command, ExecutionTimer::stopTimerMicroseconds() )

} // namespace Common
} // namespace Stuff
} // namespace Dune

#endif /* __EXECUTIONTIMER_HH__ */
