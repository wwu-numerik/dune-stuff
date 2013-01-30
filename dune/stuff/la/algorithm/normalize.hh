#ifndef DUNE_STUFF_LA_ALGORITHM_NORMALIZE_HH
#define DUNE_STUFF_LA_ALGORITHM_NORMALIZE_HH

#include <dune/common/typetraits.hh>
#include <dune/common/static_assert.hh>

#include <dune/stuff/la/container/interface.hh>
#include <dune/stuff/la/container/eigen.hh>
#include <dune/stuff/common/logging.hh>
#include <dune/stuff/common/color.hh>

namespace Dune {
namespace Stuff {
namespace LA {
namespace Algorithm {


template< class ContainerType >
void normalize(ContainerType& /*_vector*/)
{
  dune_static_assert((Dune::AlwaysFalse< ContainerType >::value), "ERROR: not implemeneted for this ContainerType!");
}


//#if HAVE_EIGEN
template< class ElementType >
void normalize(Dune::Stuff::LA::Container::EigenDenseVector< ElementType >& _vector)
{
  // if this is an empty vector report and do nothing
  if (_vector.size() == 0) {
    if (!Dune::Stuff::Common::Logger().created())
      Dune::Stuff::Common::Logger().create(Dune::Stuff::Common::LOG_CONSOLE | Dune::Stuff::Common::LOG_DEBUG);
    Dune::Stuff::Common::LogStream& debug = Dune::Stuff::Common::Logger().debug();
    debug << "\n" << Dune::Stuff::Common::colorString("WARNING:")
          << " Dune::Stuff::LA::Algorithm::normalize() called with an empty '_vector'!" << std::endl;
  } else {
    // compute L2-norm
    const ElementType norm = std::sqrt(_vector.backend().transpose() * _vector.backend());
    // normalize
    _vector.backend() /= norm;
  }
} // void normalize(Dune::Stuff::LA::Container::EigenDenseVector< ElementType >& _vector)


template< class ElementType >
void normalize(Dune::Stuff::LA::Container::EigenDenseMatrix< ElementType >& _matrix)
{
  // if this is an empty matrix report and do nothing
  if (_matrix.rows() == 0 || _matrix.cols() == 0) {
    if (!Dune::Stuff::Common::Logger().created())
      Dune::Stuff::Common::Logger().create(Dune::Stuff::Common::LOG_CONSOLE | Dune::Stuff::Common::LOG_DEBUG);
    Dune::Stuff::Common::LogStream& debug = Dune::Stuff::Common::Logger().debug();
    debug << "\n" << Dune::Stuff::Common::colorString("WARNING:")
          << " Dune::Stuff::LA::Algorithm::normalize() called with an empty '_matrix'!" << std::endl;
  } else {
    // this is a matrix, check how to interpret it
    if (_matrix.rows() == 1) {
      // this is a row-vector, proceed
      // compute L2-norm
      const ElementType norm = std::sqrt(_matrix.backend().row(0) * _matrix.backend().row(0).transpose());
      // normalize
      _matrix.backend() /= norm;
    } else if (_matrix.cols() == 1) {
      // this i a column-vector, proceed
      // compute L2-norm
      const ElementType norm = std::sqrt(_matrix.backend().col(0).transpose() * _matrix.backend().col(0));
      // normalize
      _matrix.backend() /= norm;
    } else
      DUNE_THROW(Dune::RangeError,
                 "\nERROR: not implemented for matrices!");
  }
} // void normalize(Dune::Stuff::LA::Container::EigenDenseMatrix< ElementType >& _matrix)


template< class ElementType >
void normalize(Dune::Stuff::LA::Container::EigenRowMajorSparseMatrix< ElementType >& /*_vector*/)
{
  dune_static_assert((Dune::AlwaysFalse< ElementType >::value),
                     "ERROR: not implemeneted for EigenRowMajorSparseMatrix!");
}
//#endif // HAVE_EIGEN


template< class ScalarProductType, class ContainerType >
void normalize(const ScalarProductType& /*scalarProduct*/, ContainerType& /*_columnVectors*/)
{
  dune_static_assert((Dune::AlwaysFalse< ScalarProductType >::value || Dune::AlwaysFalse< ContainerType >::value),
                     "ERROR: not implemeneted for this ContainerType!");
}


//#if HAVE_EIGEN
template< class ElementType >
void normalize(const Dune::Stuff::LA::Container::EigenDenseMatrix< ElementType >& /*_scalarProduct*/,
                 Dune::Stuff::LA::Container::EigenDenseVector< ElementType >& /*_columnVectors*/)
{
  dune_static_assert((Dune::AlwaysFalse< ElementType >::value),
                     "ERROR: not implemeneted yet!");
} // void normalize(Dune::Stuff::LA::Container::EigenDenseVector< ElementType >& _vector)


template< class ElementType >
void normalize(const Dune::Stuff::LA::Container::EigenDenseMatrix< ElementType >& _scalarProduct,
                 Dune::Stuff::LA::Container::EigenDenseMatrix< ElementType >& _matrix)
{
  // if this is an empty matrix report and do nothing
  if (_matrix.rows() == 0 || _matrix.cols() == 0) {
    if (!Dune::Stuff::Common::Logger().created())
      Dune::Stuff::Common::Logger().create(Dune::Stuff::Common::LOG_CONSOLE | Dune::Stuff::Common::LOG_DEBUG);
    Dune::Stuff::Common::LogStream& debug = Dune::Stuff::Common::Logger().debug();
    debug << "\n" << Dune::Stuff::Common::colorString("WARNING:")
          << " Dune::Stuff::LA::Algorithm::normalize() called with an empty '_matrix'!" << std::endl;
  } else {
    // this is a matrix, check how to interpret it
    if (_matrix.rows() == 1) {
      // this is a row-vector, check sizes
      assert(_matrix.cols() == _scalarProduct.rows());
      assert(_matrix.cols() == _scalarProduct.cols());
      // compute L2-norm
      const ElementType norm
          = std::sqrt(_matrix.backend().row(0) * _scalarProduct.backend() * _matrix.backend().row(0).transpose());
      // normalize
      _matrix.backend() /= norm;
    } else if (_matrix.cols() == 1) {
      // this i a column-vector, check sizes
      assert(_matrix.rows() == _scalarProduct.rows());
      assert(_matrix.rows() == _scalarProduct.cols());
      // compute L2-norm
      const ElementType norm
          = std::sqrt(_matrix.backend().col(0).transpose() * _scalarProduct.backend() * _matrix.backend().col(0));
      // normalize
      _matrix.backend() /= norm;
    } else
      DUNE_THROW(Dune::RangeError,
                 "\nERROR: not implemented for matrices!");
  }
} // void normalize(Dune::Stuff::LA::Container::EigenDenseVector< ElementType >& _vector)


template< class ElementType >
void normalize(const Dune::Stuff::LA::Container::EigenRowMajorSparseMatrix< ElementType >& /*_scalarProduct*/,
                 Dune::Stuff::LA::Container::EigenDenseVector< ElementType >& /*_columnVectors*/)
{
  dune_static_assert((Dune::AlwaysFalse< ElementType >::value),
                     "ERROR: not implemeneted yet!");
} // void normalize(Dune::Stuff::LA::Container::EigenDenseVector< ElementType >& _vector)


template< class ElementType >
void normalize(const Dune::Stuff::LA::Container::EigenRowMajorSparseMatrix< ElementType >& /*_scalarProduct*/,
                 Dune::Stuff::LA::Container::EigenDenseMatrix< ElementType >& /*_columnVectors*/)
{
  dune_static_assert((Dune::AlwaysFalse< ElementType >::value),
                     "ERROR: not implemeneted yet!");
} // void normalize(Dune::Stuff::LA::Container::EigenDenseVector< ElementType >& _vector)
//#endif // HAVE_EIGEN


} // namespace Algorithm
} // namespace LA
} // namespace Stuff
} // namespace Dune

#endif // DUNE_STUFF_LA_ALGORITHM_NORMALIZE_HH
