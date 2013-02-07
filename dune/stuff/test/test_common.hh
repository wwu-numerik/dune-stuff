#ifndef DUNE_STUFF_TEST_TOOLS_HH
#define DUNE_STUFF_TEST_TOOLS_HH

#ifdef HAVE_CMAKE_CONFIG
 #include "cmake_config.h"
#elif defined (HAVE_CONFIG_H)
 #include <config.h>
#endif // ifdef HAVE_CMAKE_CONFIG

#include <dune/common/float_cmp.hh>
#include <dune/common/fvector.hh>
#include <dune/common/fmatrix.hh>
#include <dune/common/tuples.hh>
#include <dune/common/tupleutility.hh>
#include <dune/common/mpihelper.hh>
#include <dune/stuff/aliases.hh>

#include <gtest.h>

#include <random>
#include <fstream>
#include <sys/time.h>

#define MY_ASSERT(cond) EXPECT_TRUE(cond)

template < template <class> class Test >
struct TestRunner {
    struct Visitor {
        template <class T>
        void visit(const T&) {
            Test<T>().run();
        }
    };

    template < class Tuple >
    static void run() {
        Tuple t;
        Dune::ForEachValue<Tuple> fe(t);
        Visitor v;
        fe.apply(v);
    }
};

template < int i >
struct Int {
  static const int value = i;
};

//! where sleep only counts toward wall time, this wastes actual cpu time
void busywait(const int ms)  {
  // "round" up to next full 10 ms to align with native timer res
  const int milliseconds = (ms/10)*10 + 10;
  timeval start, end;
  gettimeofday(&start, NULL);
  do  {
   gettimeofday(&end, NULL);
  } while( ((end.tv_sec - start.tv_sec )*1e6) + ((end.tv_usec - start.tv_usec)) < milliseconds * 1000 );
}


typedef Dune::tuple<double, float, //Dune::bigunsignedint,
  int, unsigned int, unsigned long, long long, char> BasicTypes;

#endif // DUNE_STUFF_TEST_TOOLS_HH
