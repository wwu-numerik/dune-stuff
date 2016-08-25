// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//
// Contributors: Sven Kaulmann

#ifndef DUNE_STUFF_COMMON_MATRIX_HH
#define DUNE_STUFF_COMMON_MATRIX_HH

#include <boost/numeric/conversion/cast.hpp>

#include <dune/common/dynmatrix.hh>
#include <dune/common/fmatrix.hh>

#include <dune/stuff/aliases.hh>
#include <dune/stuff/common/exceptions.hh>
#include <dune/stuff/common/fmatrix.hh>
#include <dune/stuff/common/vector.hh>

namespace Dune {
namespace Stuff {
namespace Common {


/**
 * \brief Traits to statically extract the information of a (mathematical) matrix.
 *
 *        If you want your matrix class to benefit from the operators defined in this header you have to manually
 *        specify a specialization of this class in your code with is_matrix defined to true and an appropriate
 *        static methods and members (see the specializations below).
 */
template <class MatType>
struct MatrixAbstraction
{
  typedef MatType MatrixType;
  typedef MatType ScalarType;
  typedef MatType RealType;
  typedef MatType S;
  typedef MatType R;

  static const bool is_matrix = false;

  static const bool has_static_size = false;

  static const size_t static_rows = std::numeric_limits<size_t>::max();

  static const size_t static_cols = std::numeric_limits<size_t>::max();

  static inline /*MatrixType*/ void create(const size_t /*rows*/, const size_t /*cols*/)
  {
    static_assert(AlwaysFalse<MatType>::value, "Do not call me if is_matrix is false!");
  }

  static inline /*MatrixType*/ void create(const size_t /*rows*/, const size_t /*cols*/, const ScalarType& /*val*/)
  {
    static_assert(AlwaysFalse<MatType>::value, "Do not call me if is_matrix is false!");
  }

  static inline /*size_t*/ void rows(const MatrixType& /*mat*/)
  {
    static_assert(AlwaysFalse<MatType>::value, "Do not call me if is_matrix is false!");
  }

  static inline /*size_t*/ void cols(const MatrixType& /*mat*/)
  {
    static_assert(AlwaysFalse<MatType>::value, "Do not call me if is_matrix is false!");
  }

  static inline void
  set_entry(MatrixType& /*mat*/, const size_t /*row*/, const size_t /*col*/, const ScalarType& /*val*/)
  {
    static_assert(AlwaysFalse<MatType>::value, "Do not call me if is_matrix is false!");
  }

  static inline /*ScalarType*/ void get_entry(const MatrixType& /*mat*/, const size_t /*row*/, const size_t /*col*/)
  {
    static_assert(AlwaysFalse<MatType>::value, "Do not call me if is_matrix is false!");
  }
};

template <class K>
struct MatrixAbstraction<Dune::DynamicMatrix<K>>
{
  typedef Dune::DynamicMatrix<K> MatrixType;
  typedef typename Dune::FieldTraits<K>::field_type ScalarType;
  typedef typename Dune::FieldTraits<K>::real_type RealType;
  typedef ScalarType S;
  typedef RealType R;

  static const bool is_matrix = true;

  static const bool has_static_size = false;

  static const size_t static_rows = std::numeric_limits<size_t>::max();

  static const size_t static_cols = std::numeric_limits<size_t>::max();

  static inline MatrixType create(const size_t rows, const size_t cols)
  {
    return MatrixType(rows, cols);
  }

  static inline MatrixType create(const size_t rows, const size_t cols, const ScalarType& val)
  {
    return MatrixType(rows, cols, val);
  }

  static inline size_t rows(const MatrixType& mat)
  {
    return mat.rows();
  }

  static inline size_t cols(const MatrixType& mat)
  {
    return mat.cols();
  }

  static inline void set_entry(MatrixType& mat, const size_t row, const size_t col, const ScalarType& val)
  {
    mat[row][col] = val;
  }

  static inline ScalarType get_entry(const MatrixType& mat, const size_t row, const size_t col)
  {
    return mat[row][col];
  }
};

template <class K, int N, int M>
struct MatrixAbstraction<Dune::FieldMatrix<K, N, M>>
{
  typedef Dune::FieldMatrix<K, N, M> MatrixType;
  typedef typename Dune::FieldTraits<K>::field_type ScalarType;
  typedef typename Dune::FieldTraits<K>::real_type RealType;
  typedef ScalarType S;
  typedef RealType R;

  static const bool is_matrix = true;

  static const bool has_static_size = true;

  static const size_t static_rows = N;

  static const size_t static_cols = M;

  static inline MatrixType create(const size_t rows, const size_t cols)
  {
    if (rows != N)
      DUNE_THROW(Dune::Stuff::Exceptions::shapes_do_not_match, "rows = " << rows << "\nN = " << int(N));
    if (cols != M)
      DUNE_THROW(Dune::Stuff::Exceptions::shapes_do_not_match, "cols = " << cols << "\nM = " << int(M));
    return MatrixType();
  }

  static inline MatrixType create(const size_t rows, const size_t cols, const ScalarType& val)
  {
    if (rows != N)
      DUNE_THROW(Dune::Stuff::Exceptions::shapes_do_not_match, "rows = " << rows << "\nN = " << int(N));
    if (cols != M)
      DUNE_THROW(Dune::Stuff::Exceptions::shapes_do_not_match, "cols = " << cols << "\nM = " << int(M));
    return MatrixType(val);
  }

  static inline size_t rows(const MatrixType& /*mat*/)
  {
    return boost::numeric_cast<size_t>(N);
  }

  static inline size_t cols(const MatrixType& /*mat*/)
  {
    return boost::numeric_cast<size_t>(M);
  }

  static inline void set_entry(MatrixType& mat, const size_t row, const size_t col, const ScalarType& val)
  {
    mat[row][col] = val;
  }

  static inline ScalarType get_entry(const MatrixType& mat, const size_t row, const size_t col)
  {
    return mat[row][col];
  }
};

template <class K, int N, int M>
struct MatrixAbstraction<Dune::Stuff::Common::FieldMatrix<K, N, M>>
{
  typedef Dune::Stuff::Common::FieldMatrix<K, N, M> MatrixType;
  typedef typename Dune::FieldTraits<K>::field_type ScalarType;
  typedef typename Dune::FieldTraits<K>::real_type RealType;
  typedef ScalarType S;
  typedef RealType R;

  static const bool is_matrix = true;

  static const bool has_static_size = true;

  static const size_t static_rows = N;

  static const size_t static_cols = M;

  static inline MatrixType create(const size_t rows, const size_t cols)
  {
    return MatrixType(rows, cols);
  }

  static inline MatrixType create(const size_t rows, const size_t cols, const ScalarType& val)
  {
    return MatrixType(rows, cols, val);
  }

  static inline size_t rows(const MatrixType& /*mat*/)
  {
    return N;
  }

  static inline size_t cols(const MatrixType& /*mat*/)
  {
    return M;
  }

  static inline void set_entry(MatrixType& mat, const size_t row, const size_t col, const ScalarType& val)
  {
    mat[row][col] = val;
  }

  static inline ScalarType get_entry(const MatrixType& mat, const size_t row, const size_t col)
  {
    return mat[row][col];
  }
};


template <class MatrixType>
struct is_matrix
{
  static const bool value = MatrixAbstraction<MatrixType>::is_matrix;
};


template <class MatrixType>
typename std::enable_if<is_matrix<MatrixType>::value, MatrixType>::type
create(const size_t rows, const size_t cols, const typename MatrixAbstraction<MatrixType>::S& val)
{
  return MatrixAbstraction<MatrixType>::create(rows, cols, val);
}


} // namespace Common
} // namespace Stuff
} // namespace Dune


template <class S, class M>
typename std::enable_if<std::is_arithmetic<S>::value && Dune::Stuff::Common::is_matrix<M>::value, M>::type
operator*(const S& scalar, const M& mat)
{
  M result(mat);
  for (size_t ii = 0; ii < DSC::MatrixAbstraction<M>::rows(mat); ++ii)
    for (size_t jj = 0; jj < DSC::MatrixAbstraction<M>::cols(mat); ++jj)
      DSC::MatrixAbstraction<M>::set_entry(result, ii, jj, DSC::MatrixAbstraction<M>::get_entry(mat, ii, jj) * scalar);
  return result;
} // ... operator*(...)


template <class L, class R>
typename std::enable_if<Dune::Stuff::Common::is_matrix<L>::value && Dune::Stuff::Common::is_matrix<R>::value
                            && std::is_same<typename Dune::Stuff::Common::MatrixAbstraction<L>::S,
                                            typename Dune::Stuff::Common::MatrixAbstraction<R>::S>::value,
                        L>::type
operator+(const L& left, const R& right)
{
  const auto rows_left = DSC::MatrixAbstraction<L>::rows(left);
  const auto cols_left = DSC::MatrixAbstraction<L>::cols(left);
  const auto rows_right = DSC::MatrixAbstraction<R>::rows(right);
  const auto cols_right = DSC::MatrixAbstraction<R>::cols(right);
  if (rows_left != rows_right || cols_left != cols_right)
    DUNE_THROW(Dune::Stuff::Exceptions::shapes_do_not_match,
               "left.rows() = " << rows_left << "\nright.rows() = " << rows_right << "\nleft.cols() = " << cols_left
                                << "\nright.cols() = "
                                << cols_right);
  L result(left);
  for (size_t ii = 0; ii < rows_left; ++ii)
    for (size_t jj = 0; jj < cols_left; ++jj)
      DSC::MatrixAbstraction<L>::set_entry(result,
                                           ii,
                                           jj,
                                           DSC::MatrixAbstraction<L>::get_entry(left, ii, jj)
                                               + DSC::MatrixAbstraction<R>::get_entry(right, ii, jj));
  return result;
} // ... operator+(...)


#endif // DUNE_STUFF_COMMON_MATRIX_HH
