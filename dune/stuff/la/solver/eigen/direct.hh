#ifndef DUNE_STUFF_LA_SOLVER_EIGEN_DIRECT_HH
#define DUNE_STUFF_LA_SOLVER_EIGEN_DIRECT_HH

#ifdef HAVE_CMAKE_CONFIG
  #include "cmake_config.h"
#elif defined (HAVE_CONFIG_H)
  #include "config.h"
#endif // ifdef HAVE_CMAKE_CONFIG

#if HAVE_EIGEN

#include <Eigen/SparseCore>
#include <Eigen/SparseCholesky>

#include <dune/common/exceptions.hh>

#include <dune/stuff/la/container/eigen.hh>

#include "interface.hh"

namespace Dune {
namespace Stuff {
namespace LA {
namespace Solver {
namespace Eigen {


template< class MatrixImp >
class SimplicialLLT
  : public Interface< MatrixImp >
{
public:
  typedef Interface< MatrixImp > BaseType;

  typedef typename BaseType::MatrixType MatrixType;

  typedef typename BaseType::VectorType VectorType;

  typedef typename BaseType::ElementType ElementType;

  typedef typename BaseType::size_type size_type;

private:
  typedef ::Eigen::SimplicialLLT< typename MatrixType::BaseType, ::Eigen::Lower > SolverType;

public:
  SimplicialLLT()
    : initialized_(false)
  {}

  virtual void init(const MatrixType& systemMatrix)
  {
    if (initialized_)
      DUNE_THROW(Dune::InvalidStateException, "\nERROR: init() may only be called once!");
    solver_ = new SolverType(systemMatrix.base());
    initialized_ = true;
  } // virtual void init(...)

  virtual bool apply(const VectorType& rhsVector,
                     VectorType& solutionVector,
                     const size_type /*maxIter = 5000*/,
                     const ElementType /*precision = 1e-12*/) const
  {
    if (!initialized_)
      DUNE_THROW(Dune::InvalidStateException, "\nERROR: please call init() before calling apply()!");
    solutionVector.base() = solver_->solve(rhsVector.base());
    const ::Eigen::ComputationInfo info = solver_->info();
    return (info == ::Eigen::Success);
  } // virtual bool apply(...)

private:
  bool initialized_;
  SolverType* solver_;
}; // class SimplicialLLT


template< class MatrixImp >
class SimplicialLDLT
  : public Interface< MatrixImp >
{
public:
  typedef Interface< MatrixImp > BaseType;

  typedef typename BaseType::MatrixType MatrixType;

  typedef typename BaseType::VectorType VectorType;

  typedef typename BaseType::ElementType ElementType;

  typedef typename BaseType::size_type size_type;

private:
  typedef ::Eigen::SimplicialLDLT< typename MatrixType::BaseType, ::Eigen::Lower > SolverType;

public:
  SimplicialLDLT()
    : initialized_(false)
  {}

  virtual void init(const MatrixType& systemMatrix)
  {
    if (initialized_)
      DUNE_THROW(Dune::InvalidStateException, "\nERROR: init() may only be called once!");
    solver_ = new SolverType(systemMatrix.base());
    initialized_ = true;
  } // virtual void init(...)

  virtual bool apply(const VectorType& rhsVector,
                     VectorType& solutionVector,
                     const size_type /*maxIter = 5000*/,
                     const ElementType /*precision = 1e-12*/) const
  {
    if (!initialized_)
      DUNE_THROW(Dune::InvalidStateException, "\nERROR: please call init() before calling apply()!");
    solutionVector.base() = solver_->solve(rhsVector.base());
    const ::Eigen::ComputationInfo info = solver_->info();
    return (info == ::Eigen::Success);
  } // virtual bool apply(...)

private:
  bool initialized_;
  SolverType* solver_;
}; // class SimplicialLDLT


} // namespace Eigen
} // namespace Solver
} // namespace LA
} // namespace Stuff
} // namespace Dune

#endif // HAVE_EIGEN

#endif // DUNE_STUFF_LA_SOLVER_EIGEN_DIRECT_HH
