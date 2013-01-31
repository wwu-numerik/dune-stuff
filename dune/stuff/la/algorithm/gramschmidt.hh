#ifndef DUNE_STUFF_LA_ALGORITHM_GRAMSCHMIDT_HH
#define DUNE_STUFF_LA_ALGORITHM_GRAMSCHMIDT_HH

#include <dune/common/exceptions.hh>
#include <dune/common/static_assert.hh>
#include <dune/common/typetraits.hh>

#include <dune/stuff/common/logging.hh>
#include <dune/stuff/common/color.hh>
#include <dune/stuff/la/container/interface.hh>
#include <dune/stuff/la/container/eigen.hh>
#include <dune/stuff/la/algorithm/normalize.hh>


namespace Dune {
namespace Stuff {
namespace LA {
namespace Algorithm {


template< class ContainerType >
void gramSchmidt(ContainerType& /*_columnVectors*/)
{
  dune_static_assert((Dune::AlwaysFalse< ContainerType >::value), "ERROR: not implemeneted for this ContainerType!");
}


#if HAVE_EIGEN
template< class ElementType >
void gramSchmidt(Dune::Stuff::LA::Container::EigenDenseVector< ElementType >& /*_columnVectors*/)
{
  dune_static_assert((Dune::AlwaysFalse< ElementType >::value),
                     "ERROR: not implemeneted for EigenDenseVector, use normalize()!");
} // void normalize(Dune::Stuff::LA::Container::EigenDenseVector< ElementType >& _vector)


// from Eigen: src/eigen/test/umeyama.cpp
// they say one could use another orthogonalization of the rows, but I'm not sure about this
/*
      // this additional orthogonalization is not necessary in theory but should enhance
      // the numerical orthogonality of the matrix
      for (int row = 0; row < size; ++row)
      {
        typename MatrixType::RowXpr rowVec = Q.row(row);
        for (int prevRow = 0; prevRow < row; ++prevRow)
        {
          typename MatrixType::RowXpr prevRowVec = Q.row(prevRow);
          rowVec -= rowVec.dot(prevRowVec)*prevRowVec;
        }
        Q.row(row) = rowVec.normalized();
      }
*/


template< class ElementType >
void gramSchmidt(Dune::Stuff::LA::Container::EigenDenseMatrix< ElementType >& _columnVectors)
{
  // if this is an empty matrix, throw up
  if (_columnVectors.cols() == 0 || _columnVectors.rows() == 0)
    DUNE_THROW(Dune::MathError,
               "\nERROR: '_columnVectors' is empty!");
  else {
    // this is a matrix, check how to interpret it
    if (_columnVectors.rows() == 1) {
      // this is a row-vector, throw up
      DUNE_THROW(Dune::MathError,
                 "\nERROR: '_columnVectors' is a row-vector!");
    } else if (_columnVectors.cols() == 1) {
      // this is just one column-vector, normalize it
      Dune::Stuff::LA::Algorithm::normalize(_columnVectors);
    } else {
      // this is a set of column-vectors
      typedef typename Dune::Stuff::LA::Container::EigenDenseMatrix< ElementType >::size_type size_type;
      for (size_type ii = 0; ii < _columnVectors.cols(); ++ii) {
        // get the iith column
        typedef typename Dune::Stuff::LA::Container::EigenDenseMatrix< ElementType >::BackendType::ColXpr ColumnType;
        ColumnType iith_column = _columnVectors.backend().col(ii);
        // and orthonormalize it wrt all previous columns
        for (size_type jj = 0; jj < ii; ++ jj) {
          // therefore, get the jjth column,
          ColumnType jjth_column = _columnVectors.backend().col(jj);
          // project the iith column wrt to the jjth column
          iith_column -= iith_column.dot(jjth_column) * jjth_column;
        }
        // and finally normalize the iith column
        const ElementType norm = std::sqrt(iith_column.transpose() * iith_column);
        _columnVectors.backend().col(ii) = iith_column / norm;
      }
    } // this is a matrix, check how to interpret it
  } // if this is an empty matrix, throw up
} // void normalize(Dune::Stuff::LA::Container::EigenDenseMatrix< ElementType >& _matrix)


template< class ElementType >
void gramSchmidt(Dune::Stuff::LA::Container::EigenRowMajorSparseMatrix< ElementType >& /*_columnVectors*/)
{
  dune_static_assert((Dune::AlwaysFalse< ElementType >::value),
                     "ERROR: not implemeneted for EigenRowMajorSparseMatrix!");
}
#endif // HAVE_EIGEN


template< class ScalarProductType, class ContainerType >
void gramSchmidt(const ScalarProductType& /*scalarProduct*/, ContainerType& /*_columnVectors*/)
{
  dune_static_assert((Dune::AlwaysFalse< ScalarProductType >::value || Dune::AlwaysFalse< ContainerType >::value),
                     "ERROR: not implemeneted for this combination of ScalarProductType/ContainerType!");
}


#if HAVE_EIGEN
template< class ElementType >
void gramSchmidt(const Dune::Stuff::LA::Container::EigenDenseMatrix< ElementType >& /*_scalarProduct*/,
                 Dune::Stuff::LA::Container::EigenDenseVector< ElementType >& /*_columnVectors*/)
{
  dune_static_assert((Dune::AlwaysFalse< ElementType >::value),
                     "ERROR: not implemeneted for EigenDenseVector, use normalize()!");
} // void normalize(Dune::Stuff::LA::Container::EigenDenseVector< ElementType >& _vector)


template< class ElementType >
void gramSchmidt(const Dune::Stuff::LA::Container::EigenRowMajorSparseMatrix< ElementType >& /*_scalarProduct*/,
                 Dune::Stuff::LA::Container::EigenDenseVector< ElementType >& /*_columnVectors*/)
{
  dune_static_assert((Dune::AlwaysFalse< ElementType >::value),
                     "ERROR: not implemeneted for EigenDenseVector, use normalize()!");
} // void normalize(Dune::Stuff::LA::Container::EigenDenseVector< ElementType >& _vector)


template< class ElementType >
void gramSchmidt(const Dune::Stuff::LA::Container::EigenDenseMatrix< ElementType >& _scalarProduct,
                 Dune::Stuff::LA::Container::EigenDenseMatrix< ElementType >& _columnVectors)
{
  // if this is an empty matrix, throw up
  if (_columnVectors.cols() == 0 || _columnVectors.rows() == 0)
    DUNE_THROW(Dune::MathError,
               "\nERROR: '_columnVectors' is empty!");
  else {
    // this is a matrix, check how to interpret it
    if (_columnVectors.rows() == 1) {
      // this is a row-vector, throw up
      DUNE_THROW(Dune::MathError,
                 "\nERROR: '_columnVectors' is a row-vector!");
    } else if (_columnVectors.cols() == 1) {
      // this is just one column-vector, normalize it
      Dune::Stuff::LA::Algorithm::normalize(_scalarProduct, _columnVectors);
    } else {
      // this is a set of column-vectors, check sizes
      assert(_columnVectors.rows() == _scalarProduct.rows());
      assert(_columnVectors.rows() == _scalarProduct.cols());
      typedef typename Dune::Stuff::LA::Container::EigenDenseMatrix< ElementType >::size_type size_type;
      for (size_type ii = 0; ii < _columnVectors.cols(); ++ii) {
        // get the iith column
        typedef typename Dune::Stuff::LA::Container::EigenDenseMatrix< ElementType >::BackendType::ColXpr ColumnType;
        ColumnType iith_column = _columnVectors.backend().col(ii);
        // and orthonormalize it wrt all previous columns
        for (size_type jj = 0; jj < ii; ++ jj) {
          // therefore, get the jjth column,
          ColumnType jjth_column = _columnVectors.backend().col(jj);
          // project the iith column wrt to the jjth column
          const ElementType factor = iith_column.transpose() * _scalarProduct.backend() * jjth_column;
          iith_column -= factor * jjth_column;
        }
        // and finally normalize the iith column
        const ElementType norm = std::sqrt(iith_column.transpose() * _scalarProduct.backend() * iith_column);
        _columnVectors.backend().col(ii) = iith_column / norm;
      }
    } // this is a matrix, check how to interpret it
  } // if this is an empty matrix, throw up
} // void normalize(Dune::Stuff::LA::Container::EigenDenseMatrix< ElementType >& _matrix)
#endif // HAVE_EIGEN


template< class ScalarProductType, class BasisVectorsType, class ContainerType >
void gramSchmidt(const ScalarProductType& /*scalarProduct*/,
                 const BasisVectorsType& /*_columnBasisVectors*/,
                 ContainerType& /*_columnVectors*/)
{
  dune_static_assert((Dune::AlwaysFalse< ScalarProductType >::value || Dune::AlwaysFalse< ContainerType >::value),
                     "ERROR: not implemeneted for this combination of ScalarProductType/BasisVectorsType/ContainerType!");
}


#if HAVE_EIGEN
template< class ElementType >
bool gramSchmidt(const Dune::Stuff::LA::Container::EigenRowMajorSparseMatrix< ElementType >& _scalarProduct,
                 const Dune::Stuff::LA::Container::EigenDenseMatrix< ElementType >& _columnBasisVectors,
                 Dune::Stuff::LA::Container::EigenDenseVector< ElementType >& _vector,
                 const ElementType epsilon = 1e-10)
{
  // if this is an empty vector, throw up
  if (_vector.size() == 0)
    DUNE_THROW(Dune::MathError,
               "\nERROR: '_vector' is empty!");
  else {
    // check sizes
    assert(_scalarProduct.rows() == _vector.size());
    assert(_scalarProduct.cols() == _vector.size());
    assert(_columnBasisVectors.rows() == _vector.size());
    typedef typename Dune::Stuff::LA::Container::EigenDenseMatrix< ElementType >::size_type size_type;
    // orthonormalize _vector wrt all columns in _columnBasisVectors
    for (size_type jj = 0; jj < _columnBasisVectors.cols(); ++ jj) {
      // therefore, subtract its projection onto the jjth basis vector
      const ElementType factor = _vector.backend().transpose() * _scalarProduct.backend() * _columnBasisVectors.backend().col(jj);
      const ElementType norm = _columnBasisVectors.backend().col(jj).transpose() * _scalarProduct.backend() * _columnBasisVectors.backend().col(jj);
      _vector.backend() -= (factor/norm) * _columnBasisVectors.backend().col(jj);
    }
    // and normalize it
    const ElementType norm = std::sqrt(_vector.backend().transpose() * _scalarProduct.backend() * _vector.backend());
    if (norm < epsilon)
      return false;
    else {
      _vector.backend() /= norm;
      return true;
    }
  } // if this is an empty matrix, throw up
} // void normalize(Dune::Stuff::LA::Container::EigenDenseVector< ElementType >& _vector)
#endif // HAVE_EIGEN

} // namespace Algorithm
} // namespace LA
} // namespace Stuff
} // namespace Dune

#endif // DUNE_STUFF_LA_ALGORITHM_GRAMSCHMIDT_HH
