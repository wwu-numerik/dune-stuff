#ifndef __EXECUTIONTIMER_HH__
#define __EXECUTIONTIMER_HH__

#include <boost/date_time/posix_time/posix_time.hpp>

namespace Stuff {
    
    class MyExecutionTimer {
    public:
      static void startTimer() {
        startt_ = boost::posix_time::microsec_clock::local_time();
        return;
      }

      static double stopTimerSeconds() {
        stopt_ = boost::posix_time::microsec_clock::local_time();
        boost::posix_time::time_duration duration = stopt_-startt_;
        return duration.total_seconds();
      }

      static double stopTimerMilliseconds() {
        stopt_ = boost::posix_time::microsec_clock::local_time();
        boost::posix_time::time_duration duration = stopt_-startt_;
        return duration.total_milliseconds();
      }

      static double stopTimerMicroseconds() {
        stopt_ = boost::posix_time::microsec_clock::local_time();
        boost::posix_time::time_duration duration = stopt_-startt_;
        return duration.total_microseconds();
      }


    private:
      static boost::posix_time::ptime startt_;
      static boost::posix_time::ptime stopt_;
    };

    boost::posix_time::ptime MyExecutionTimer::startt_;
    boost::posix_time::ptime MyExecutionTimer::stopt_;


#define EXECUTETIMEDSECONDS(command) \
    (MyExecutionTimer::startTimer(),command,MyExecutionTimer::stopTimerSeconds())

#define EXECUTETIMEDMILLISECONDS(command) \
    (MyExecutionTimer::startTimer(),command,MyExecutionTimer::stopTimerMilliseconds())

#define EXECUTETIMEDMICROSECONDS(command) \
    (MyExecutionTimer::startTimer(),command,MyExecutionTimer::stopTimerMicroseconds())

    
} // namespace Stuff


#endif /* __EXECUTIONTIMER_HH__ */
