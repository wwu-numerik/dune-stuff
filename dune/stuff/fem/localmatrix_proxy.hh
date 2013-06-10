#ifndef LOCALMATRIX_PROXY_HH
#define LOCALMATRIX_PROXY_HH

#include <vector>
#include <assert.h>
#include <dune/common/float_cmp.hh>
#include <dune/stuff/common/debug.hh>
#include <dune/stuff/common/type_utils.hh>
#include <dune/stuff/aliases.hh>

namespace Dune {
namespace Stuff {
namespace Fem {


template <class MatrixObjectType, class Enable = void>
struct LocalMatrixProxyTraits
{
    typedef typename MatrixObjectType::LocalMatrixType LocalMatrixType;
    typedef typename MatrixObjectType::DomainSpaceType DomainSpaceType;
    typedef typename MatrixObjectType::RangeSpaceType RangeSpaceType;
    typedef typename MatrixObjectType::DomainSpaceType::GridType GridType;
    typedef typename MatrixObjectType::DomainSpaceType::EntityType DomainEntityType;
    typedef typename MatrixObjectType::RangeSpaceType::EntityType RangeEntityType;
    typedef typename MatrixObjectType::RangeSpaceType::RangeFieldType FieldType;
};


template <class MatrixObjectType>
struct LocalMatrixProxyTraits<MatrixObjectType, typename std::enable_if<DSC::is_smart_ptr<MatrixObjectType>::value>::type>
{
    typedef typename MatrixObjectType::element_type::LocalMatrixType LocalMatrixType;
    typedef typename MatrixObjectType::element_type::DomainSpaceType DomainSpaceType;
    typedef typename MatrixObjectType::element_type::RangeSpaceType RangeSpaceType;
    typedef typename MatrixObjectType::element_type::DomainSpaceType::GridType GridType;
    typedef typename MatrixObjectType::element_type::DomainSpaceType::EntityType DomainEntityType;
    typedef typename MatrixObjectType::element_type::RangeSpaceType::EntityType RangeEntityType;
    typedef typename MatrixObjectType::element_type::RangeSpaceType::RangeFieldType FieldType;
};

//! a small proxy object that automagically prevents near-0 value fill-in
template< class MatrixObjectType>
class LocalMatrixProxy
{
  typedef LocalMatrixProxyTraits<MatrixObjectType> TraitsType;
  typedef typename TraitsType::LocalMatrixType LocalMatrixType;
  typedef typename TraitsType::DomainSpaceType::GridType GridType;
  typedef typename TraitsType::DomainSpaceType::EntityType DomainEntityType;
  typedef typename TraitsType::RangeSpaceType::EntityType RangeEntityType;
  typedef typename TraitsType::RangeSpaceType::RangeFieldType FieldType;
  typedef Dune::FloatCmpOps<FieldType> CompareType;
  typedef Dune::FloatCmp::DefaultEpsilon<typename CompareType::EpsilonType, CompareType::cstyle> DefaultEpsilon;
  LocalMatrixType local_matrix_;
  const double eps_;
  const unsigned int rows_;
  const unsigned int cols_;
  std::vector< FieldType > entries_;

public:
  LocalMatrixProxy(MatrixObjectType& object, const DomainEntityType& self, const RangeEntityType& neigh,
                   const double eps = DefaultEpsilon::value())
    : local_matrix_( DSC::PtrCaller<MatrixObjectType>::call(object).localMatrix(self, neigh) )
    , eps_(eps)
    , rows_( local_matrix_.rows() )
    , cols_( local_matrix_.columns() )
    , entries_( rows_ * cols_, FieldType(0.0) )
  {}

  inline void add(const unsigned int row, const unsigned int col, const FieldType val) {
    ASSERT_LT(row, rows_);
    ASSERT_LT(col, cols_);
    entries_[row * cols_ + col] += val;
  }

  auto domainBaseFunctionSet() const -> decltype(local_matrix_.domainBaseFunctionSet()) {
    return local_matrix_.domainBaseFunctionSet();
  }

  auto rangeBaseFunctionSet() const -> decltype(local_matrix_.rangeBaseFunctionSet()) {
    return local_matrix_.rangeBaseFunctionSet();
  }

  ~LocalMatrixProxy() {
    const auto comp = CompareType(eps_);
    for (unsigned int i = 0; i < rows_; ++i)
    {
      for (unsigned int j = 0; j < cols_; ++j)
      {
        const FieldType& i_j = entries_[i * cols_ + j];
        if ( comp.ne(i_j, 0.0) )
          local_matrix_.add(i, j, i_j);
      }
    }
  }

  unsigned int rows() const { return rows_; }
  unsigned int cols() const { return cols_; }
  unsigned int columns() const { return cols_; }
};

//! a small proxy object that automagically prevents near-0 value fill-in
template< class MatrixObjectType >
class ISTLLocalMatrixProxy
{
  typedef typename MatrixObjectType::LocalMatrixType LocalMatrixType;
  typedef typename MatrixObjectType::MatrixType::block_type block_type;
  typedef typename MatrixObjectType::DomainSpaceType::GridType GridType;
  typedef typename GridType::template Codim< 0 >::Entity EntityType;
  typedef typename MatrixObjectType::MatrixType::Ttype FieldType;
  // LocalMatrixType local_matrix_;
  MatrixObjectType matrix_pointer_;
  const EntityType& self_;
  const EntityType& neigh_;
  const double eps_;
  const unsigned int rows_;
  const unsigned int cols_;
  std::vector< FieldType > entries_;
  //! global row numbers
  std::vector< int > rowMap_;
  //! global col numbers
  std::vector< int > colMap_;

public:
  ISTLLocalMatrixProxy(MatrixObjectType& pointer, const EntityType& self, const EntityType& neigh, const double eps)
    : matrix_pointer_(pointer)
    , self_(self)
    , neigh_(neigh)
    , eps_(eps)
    , rows_(block_type::rows)
    , cols_(block_type::cols)
    , entries_( rows_ * cols_, FieldType(0.0) )
  {
    const auto& domainSpace = matrix_pointer_->rowSpace();
    const auto& rangeSpace = matrix_pointer_->colSpace();

    rowMap_.resize( domainSpace.baseFunctionSet(self).size() );
    colMap_.resize( rangeSpace.baseFunctionSet(neigh).size() );

    const auto dmend = domainSpace.mapper().end(self);
    for (auto dmit = domainSpace.mapper().begin(self); dmit != dmend; ++dmit)
    {
      // ASSERT_EQ( dmit.global() , domainSpace.mapToGlobal( self, dmit.local() ) );
      rowMap_[dmit.local()] = dmit.global();
    }
    const auto rmend = rangeSpace.mapper().end(neigh);
    for (auto rmit = rangeSpace.mapper().begin(neigh); rmit != rmend; ++rmit)
    {
      // ASSERT_EQ( rmit.global() , rangeSpace.mapToGlobal( neigh, rmit.local() ) );
      colMap_[rmit.local()] = rmit.global();
    }
  }

  inline void add(const unsigned int row, const unsigned int col, const FieldType val) {
    ASSERT_LT(row, rows_);
    ASSERT_LT(col, cols_);
    assert( !std::isnan( matrix_pointer_->matrix() (rowMap_[row], colMap_[col]) ) );
    entries_[row * cols_ + col] += val;
  }

  ~ISTLLocalMatrixProxy() {
    for (unsigned int i = 0; i < rows_; ++i)
    {
      for (unsigned int j = 0; j < cols_; ++j)
      {
        const FieldType& i_j = entries_[i * cols_ + j];
        if (std::fabs(i_j) > eps_)
        {
          assert( !std::isnan(i_j) );
          assert( !std::isinf(i_j) );
          matrix_pointer_->add(rowMap_[i], colMap_[j], i_j);
          assert( !std::isnan( matrix_pointer_->matrix() (rowMap_[i], colMap_[j]) ) );
          assert( !std::isinf( matrix_pointer_->matrix() (rowMap_[i], colMap_[j]) ) );
        }
      }
    }
  }

  unsigned int rows() const { return rows_; }
  unsigned int cols() const { return cols_; }
};

} // namespace Fem
} // namespace Stuff
} // namespace Dune

#endif // LOCALMATRIX_PROXY_HH
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
