#ifndef __EXECUTIONTIMER_HH__
#define __EXECUTIONTIMER_HH__

#include <boost/date_time/posix_time/posix_time.hpp>

namespace Dune {

namespace Stuff {

namespace Common {

class ExecutionTimer
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

/** Copyright (c) 2012, Sven Kaulmann
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
