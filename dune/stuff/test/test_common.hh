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

#include <gtest/gtest.h>

#include <random>

#define MY_ASSERT(cond) EXPECT_TRUE(cond)

template < typename, bool >
struct Distribution {
};
template < typename T >
struct Distribution<T,true> {
    typedef std::uniform_int_distribution <T>
        type;
};
template < typename T >
struct Distribution<T,false> {
    typedef std::uniform_real_distribution<T>
        type;
};

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

typedef Dune::tuple<double, float, //Dune::bigunsignedint,
  int, unsigned int, unsigned long, long long, char> BasicTypes;

#endif // DUNE_STUFF_TEST_TOOLS_HH
