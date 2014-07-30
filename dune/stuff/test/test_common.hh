// This file is part of the dune-stuff project:
//   https://users.dune-project.org/projects/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#ifndef DUNE_STUFF_TEST_TOOLS_HH
#define DUNE_STUFF_TEST_TOOLS_HH

#include "config.h"

#include <random>
#include <fstream>
#include <sys/time.h>

#include <dune/stuff/common/disable_warnings.hh>
# include <dune/stuff/test/gtest/gtest.h>

# include <dune/common/float_cmp.hh>
# include <dune/common/fvector.hh>
# include <dune/common/fmatrix.hh>
# include <dune/common/tuples.hh>
# include <dune/common/tupleutility.hh>
# include <dune/common/parallel/mpihelper.hh>

# if HAVE_DUNE_FEM
#   include <dune/fem/misc/mpimanager.hh>
# endif
#include <dune/stuff/common/reenable_warnings.hh>

#include <dune/stuff/aliases.hh>
#include <dune/stuff/common/parameter/configcontainer.hh>
#include <dune/stuff/common/logging.hh>
#include <dune/stuff/fem/namespace.hh>


class errors_are_not_as_expected
  : public Dune::Exception
{};


std::vector< double > truncate_vector(const std::vector< double >& in, const size_t size)
{
  assert(size <= in.size());
  if (size == in.size())
    return in;
  else {
    std::vector< double > ret(size);
    for (size_t ii = 0; ii < size; ++ii)
      ret[ii] = in[ii];
    return ret;
  }
} // ... truncate_vector(...)

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


void test_init(int argc, char** argv)
{
  testing::InitGoogleTest(&argc, argv);
  DSC_CONFIG.readOptions(argc, argv);
#if HAVE_DUNE_FEM
  Dune::Fem::MPIManager::initialize(argc, argv);
#else
  Dune::MPIHelper::instance(argc, argv);
#endif
  DSC::Logger().create(DSC::LOG_CONSOLE | DSC::LOG_ERROR);
}

#endif // DUNE_STUFF_TEST_TOOLS_HH
