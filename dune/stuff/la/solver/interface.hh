#ifndef DUNE_STUFF_LA_SOLVER_INTERFACE_HH
#define DUNE_STUFF_LA_SOLVER_INTERFACE_HH

#ifdef HAVE_CMAKE_CONFIG
  #include "cmake_config.h"
#elif defined (HAVE_CONFIG_H)
  #include "config.h"
#endif // ifdef HAVE_CMAKE_CONFIG

#if HAVE_EIGEN
  #include <Eigen/Core>
  #include <Eigen/IterativeLinearSolvers>
#endif

#include <dune/common/exceptions.hh>
#include <dune/stuff/la/container/interface.hh>

namespace Dune {
namespace Stuff {
namespace LA {
namespace Solver {


template< class MatrixImp, class VectorImp >
class Interface
{
public:
  typedef typename Dune::Stuff::LA::Container::MatrixInterface< typename MatrixImp::Traits > MatrixType;
  typedef typename MatrixType::ElementType ElementType;
  typedef typename Dune::Stuff::LA::Container::VectorInterface< typename VectorImp::Traits > VectorType;
  typedef typename MatrixType::size_type size_type;

  virtual size_type apply(const MatrixType& /*_systemMatrix*/,
                          const VectorType& /*_rhsVector*/,
                          VectorType& /*_solutionVector*/,
                          const size_type /*_maxIter*/,
                          const ElementType /*_precision*/) const = 0;
#if HAVE_EIGEN
  size_type translateInfo(const ::Eigen::ComputationInfo& info)
  {
    switch (info) {
      case ::Eigen::Success: return 0;
      case ::Eigen::NoConvergence: return 1;
      case ::Eigen::NumericalIssue: return 2;
      default: return 3;
    }
  }
#endif
}; // class Interface


template< class MatrixImp, class VectorImp >
class SolverNotImplementedForThisMatrixVectorCombination
  : public Interface< MatrixImp, VectorImp >
{
public:
  typedef typename Interface< MatrixImp, VectorImp >::MatrixType   MatrixType;
  typedef typename Interface< MatrixImp, VectorImp >::VectorType   VectorType;
  typedef typename Interface< MatrixImp, VectorImp >::ElementType  ElementType;
  typedef typename Interface< MatrixImp, VectorImp >::size_type    size_type;

  SolverNotImplementedForThisMatrixVectorCombination(const std::string msg = "\nERROR: this solver is not implemented for this matrix/vector combination")
  {
    DUNE_THROW(Dune::NotImplemented, msg);
  }

  virtual size_type apply(const MatrixType& /*_systemMatrix*/,
                          const VectorType& /*_rhsVector*/,
                          VectorType& /*_solutionVector*/,
                          const size_type /*_maxIter*/ = 0,
                          const ElementType /*_precision*/ = 0) const
  {}
}; // class SolverNotImplementedForThisMatrixVectorCombination


template< class MatrixImp, class VectorImp >
class Cg
  : public SolverNotImplementedForThisMatrixVectorCombination< MatrixImp, VectorImp >
{};

template< class MatrixImp, class VectorImp >
class CgDiagonal
  : public SolverNotImplementedForThisMatrixVectorCombination< MatrixImp, VectorImp >
{};

template< class MatrixImp, class VectorImp >
class Bicgstab
  : public SolverNotImplementedForThisMatrixVectorCombination< MatrixImp, VectorImp >
{};

template< class MatrixImp, class VectorImp >
class BicgstabDiagonal
  : public SolverNotImplementedForThisMatrixVectorCombination< MatrixImp, VectorImp >
{};

template< class MatrixImp, class VectorImp >
class BicgstabILUT
  : public SolverNotImplementedForThisMatrixVectorCombination< MatrixImp, VectorImp >
{};

template< class MatrixImp, class VectorImp >
class SimplicialLLT
  : public SolverNotImplementedForThisMatrixVectorCombination< MatrixImp, VectorImp >
{};

template< class MatrixImp, class VectorImp >
class SimplicialLDLT
  : public SolverNotImplementedForThisMatrixVectorCombination< MatrixImp, VectorImp >
{};

//template< class MatrixImp, class VectorImp >
//class Fasp
//  : public SolverNotImplementedForThisMatrixVectorCombination< MatrixImp, VectorImp >
//{};


} // namespace Solver
} // namespace LA
} // namespace Stuff
} // namespace Dune

#endif // DUNE_STUFF_LA_SOLVER_INTERFACE_HH
