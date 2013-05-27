#ifndef DUNE_STUFF_LA_CONTAINER_EIGENMANAGER_HH
#define DUNE_STUFF_LA_CONTAINER_EIGENMANAGER_HH

#ifdef HAVE_CMAKE_CONFIG
  #include "cmake_config.h"
#elif defined (HAVE_CONFIG_H)
  #include <config.h>
#endif

#ifdef HAVE_EIGEN

#include <string>
#include <map>
#include <memory>

#include <dune/stuff/common/string.hh>

#include "eigen.hh"

namespace Dune {
namespace Stuff {
namespace LA {

class EigenManager
{
public:
  typedef Dune::Stuff::LA::EigenDenseVector< double > DenseVectorType;
  typedef Dune::Stuff::LA::EigenDenseMatrix< double > DenseMatrixType;

  EigenManager();

  std::string createDenseVector(const int size);

  std::string createDenseMatrix(const int rows, const int cols);

  std::string dot(const std::string x, const std::string y);

private:
  unsigned int denseVectorId_;
  unsigned int denseMatrixId_;
  std::map< const std::string, std::shared_ptr< DenseVectorType > > denseVectorMap_;
  std::map< const std::string, std::shared_ptr< DenseMatrixType > > denseMatrixMap_;
}; // class EigenManager

} // namespace LA
} // namespace Stuff
} // namespace Dune

#endif // HAVE_EIGEN
#endif // DUNE_STUFF_LA_CONTAINER_EIGENMANAGER_HH
