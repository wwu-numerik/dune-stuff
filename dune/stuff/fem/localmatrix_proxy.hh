// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#ifndef LOCALMATRIX_PROXY_HH
#define LOCALMATRIX_PROXY_HH

#include <vector>
#include <assert.h>

#include <dune/common/float_cmp.hh>

#include <dune/stuff/common/debug.hh>
#include <dune/stuff/common/type_utils.hh>
#include <dune/stuff/aliases.hh>
#include <dune/stuff/fem/namespace.hh>

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
  const size_t rows_;
  const size_t cols_;
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

  inline void add(const size_t row, const size_t col, const FieldType val) {
    ASSERT_LT(row, rows_);
    ASSERT_LT(col, cols_);
    entries_[row * cols_ + col] += val;
  }

#if DUNE_FEM_IS_LOCALFUNCTIONS_COMPATIBLE
  auto domainBaseFunctionSet() const -> decltype(local_matrix_.domainBaseFunctionSet()) {
    return local_matrix_.domainBaseFunctionSet();
  }

  auto rangeBaseFunctionSet() const -> decltype(local_matrix_.rangeBaseFunctionSet()) {
    return local_matrix_.rangeBaseFunctionSet();
  }
#else
  auto domainBasisFunctionSet() const -> decltype(local_matrix_.domainBasisFunctionSet()) {
    return local_matrix_.domainBasisFunctionSet();
  }

  auto rangeBasisFunctionSet() const -> decltype(local_matrix_.rangeBasisFunctionSet()) {
    return local_matrix_.rangeBasisFunctionSet();
  }
#endif

  ~LocalMatrixProxy() {
    const auto comp = CompareType(eps_);
    for (size_t i = 0; i < rows_; ++i)
    {
      for (size_t j = 0; j < cols_; ++j)
      {
        const FieldType& i_j = entries_[i * cols_ + j];
        if ( comp.ne(i_j, 0.0) )
          local_matrix_.add(i, j, i_j);
      }
    }
  }

  size_t rows() const { return rows_; }
  size_t cols() const { return cols_; }
  size_t columns() const { return cols_; }

  void unitRow(size_t row)
  {
    local_matrix_.clearRow(row);
    local_matrix_.set(row, row, 1.);
  }
};

} // namespace Fem
} // namespace Stuff
} // namespace Dune

#endif // LOCALMATRIX_PROXY_HH
