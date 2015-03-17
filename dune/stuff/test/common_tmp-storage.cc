// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//

#include "main.hxx"
#include <dune/stuff/common/tmp-storage.hh>

#include <dune/common/dynmatrix.hh>
#include <dune/common/tupleutility.hh>

#include <dune/stuff/common/math.hh>
#include <dune/stuff/common/float_cmp.hh>
#include <dune/stuff/common/ranges.hh>

using namespace Dune::Stuff::Common;
using namespace std;
typedef testing::Types< double, int, complex<double> > TestTypes;

template < class T >
struct TmpTest : public testing::Test {
  typedef TmpVectorsStorage<T> Vector;
  typedef TmpMatricesStorage<T> Matrix;

  void check_sizes() const {
    const auto dims{0u, 3u, 6u};
    const auto sizes{0u, 3u, 6u};
    for(auto size : sizes) {
      for(auto row : dims) {
        Vector vec({size,size}, row);
        EXPECT_EQ(vec.indices().size(), row);
        EXPECT_EQ(vec.vectors().size(), 2);
        for(auto col : dims) {
          Matrix mat({size,size}, row, col);
          EXPECT_EQ(mat.indices().size(), 4);
          EXPECT_EQ(mat.matrices().size(), 2);
        }
      }
    }
  }

  void check_empty() const {
    vector<size_t> null;
    EXPECT_THROW(Vector(null, 0), out_of_range);
    EXPECT_THROW(Matrix(null, 0, 0), out_of_range);
  }
};

TYPED_TEST_CASE(TmpTest, TestTypes);
TYPED_TEST(TmpTest, All) {
  this->check_sizes();
  this->check_empty();
}
