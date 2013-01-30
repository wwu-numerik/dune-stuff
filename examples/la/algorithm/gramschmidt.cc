#ifdef HAVE_CMAKE_CONFIG
  #include "cmake_config.h"
#elif defined (HAVE_CONFIG_H)
  #include <config.h>
#endif // HAVE_CMAKE_CONFIG

#include <dune/stuff/la/container/eigen.hh>
#include <dune/stuff/la/algorithm/gramschmidt.hh>

using namespace Dune::Stuff;

int main(int /*argc*/, char** /*argv*/)
{
  try {
#if HAVE_EIGEN
    typedef LA::Container::EigenDenseMatrix< double > MatrixType;
    MatrixType matrix(3, 3);
    matrix.backend() << 3, 1, 1,
                        0, 1, 2,
                        0, 0, 4;
    std::cout << "matrix = \n" << matrix.backend() << std::endl;
    MatrixType matrixCopy(matrix);
    typedef LA::Container::EigenDenseMatrix< double > MatrixType;
    MatrixType scalarproduct(3, 3);
    scalarproduct.backend() <<  1, 0, 0,
                                0, 2, 0,
                                0, 0, 3;
    std::cout << "scalarproduct = \n" << scalarproduct.backend() << std::endl;
    LA::Algorithm::gramSchmidt(matrix);
    std::cout << "gramschmidt(matrix) = \n" << matrix.backend() << std::endl;
    LA::Algorithm::gramSchmidt(scalarproduct, matrixCopy);
    std::cout << "gramschmidt(scalarproduct, matrix) = \n" << matrixCopy.backend() << std::endl;
#else // HAVE_EIGEN
    std::cout << "ERROR: eigen not found!" << std::endl;
#endif // HAVE_EIGEN
  } catch(Dune::Exception& e) {
    std::cerr << "Dune reported error: " << e.what() << std::endl;
  } catch(std::exception& e) {
    std::cerr << e.what() << std::endl;
  } catch(...) {
    std::cerr << "Unknown exception thrown!" << std::endl;
  } // try
  // if we came that far we can as well be happy about it
  return 0;
} // int main(int argc, char** argv)
