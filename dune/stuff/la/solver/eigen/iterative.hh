#ifndef DUNE_STUFF_LA_SOLVER_EIGEN_ITERATIVE_HH
#define DUNE_STUFF_LA_SOLVER_EIGEN_ITERATIVE_HH

#ifdef HAVE_CMAKE_CONFIG
  #include "cmake_config.h"
#elif defined (HAVE_CONFIG_H)
  #include "config.h"
#endif // ifdef HAVE_CMAKE_CONFIG

#if HAVE_EIGEN

#include <Eigen/SparseCore>
#include <Eigen/IterativeLinearSolvers>

#include <dune/common/exceptions.hh>

#include <dune/stuff/la/container/eigen.hh>

#include "../interface.hh"

namespace Dune {
namespace Stuff {
namespace LA {
namespace Solver {
namespace Eigen {


template< class MatrixImp, class VectorImp >
class BicgstabDiagonal;


template< class ElementImp >
class BicgstabDiagonal< Container::Eigen::RowMajorSparseMatrix< ElementImp >,
                        Container::Eigen::DenseVector< ElementImp > >
  : public Interface< Container::Eigen::RowMajorSparseMatrix< ElementImp >,
                      Container::Eigen::DenseVector< ElementImp > >
{
public:
  typedef Interface<  Container::Eigen::RowMajorSparseMatrix< ElementImp >,
                      Container::Eigen::DenseVector< ElementImp > >
      BaseType;

  typedef typename BaseType::MatrixType MatrixType;

  typedef typename BaseType::VectorType VectorType;

  typedef typename BaseType::ElementType ElementType;

  typedef typename BaseType::size_type size_type;

private:
  typedef ::Eigen::DiagonalPreconditioner< ElementType > PreconditionerType;

  typedef ::Eigen::BiCGSTAB< typename MatrixType::BaseType, PreconditionerType > SolverType;

public:
  BicgstabDiagonal()
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
                     const size_type maxIter = 5000,
                     const ElementType precision = 1e-12) const
  {
    if (!initialized_)
      DUNE_THROW(Dune::InvalidStateException, "\nERROR: please call init() before calling apply()!");
    solver_->setMaxIterations(maxIter);
    solver_->setTolerance(precision);
    solutionVector.base() = solver_->solve(rhsVector.base());
    const ::Eigen::ComputationInfo info = solver_->info();
    return (info == ::Eigen::Success);
  } // virtual bool apply(...)

private:
  bool initialized_;
  SolverType* solver_;
}; // class BicgstabDiagonal


template< class MatrixImp, class VectorImp >
class BicgstabIncompleteLUT;


template< class ElementImp >
class BicgstabIncompleteLUT<  Container::Eigen::RowMajorSparseMatrix< ElementImp >,
                              Container::Eigen::DenseVector< ElementImp > >
  : public Interface< Container::Eigen::RowMajorSparseMatrix< ElementImp >,
                      Container::Eigen::DenseVector< ElementImp > >
{
public:
  typedef Interface<  Container::Eigen::RowMajorSparseMatrix< ElementImp >,
                      Container::Eigen::DenseVector< ElementImp > >
      BaseType;

  typedef typename BaseType::MatrixType MatrixType;

  typedef typename BaseType::VectorType VectorType;

  typedef typename BaseType::ElementType ElementType;

  typedef typename BaseType::size_type size_type;

private:
  typedef ::Eigen::IncompleteLUT< ElementType > PreconditionerType;

  typedef ::Eigen::BiCGSTAB< typename MatrixType::BaseType, PreconditionerType > SolverType;

public:
  BicgstabIncompleteLUT()
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
                     const size_type maxIter = 5000,
                     const ElementType precision = 1e-12) const
  {
    if (!initialized_)
      DUNE_THROW(Dune::InvalidStateException, "\nERROR: please call init() before calling apply()!");
    solver_->setMaxIterations(maxIter);
    solver_->setTolerance(precision);
    solutionVector.base() = solver_->solve(rhsVector.base());
    const ::Eigen::ComputationInfo info = solver_->info();
    return (info == ::Eigen::Success);
  } // virtual bool apply(...)

private:
  bool initialized_;
  SolverType* solver_;
}; // class BicgstabIncompleteLUT


template< class MatrixImp, class VectorImp >
class CG;


template< class ElementImp >
class CG< Container::Eigen::RowMajorSparseMatrix< ElementImp >,
          Container::Eigen::DenseVector< ElementImp > >
  : public Interface< Container::Eigen::RowMajorSparseMatrix< ElementImp >,
                      Container::Eigen::DenseVector< ElementImp > >
{
public:
  typedef Interface<  Container::Eigen::RowMajorSparseMatrix< ElementImp >,
                      Container::Eigen::DenseVector< ElementImp > >
      BaseType;

  typedef typename BaseType::MatrixType MatrixType;

  typedef typename BaseType::VectorType VectorType;

  typedef typename BaseType::ElementType ElementType;

  typedef typename BaseType::size_type size_type;

public:
  CG()
    : initialized_(false)
  {}

  virtual void init(const MatrixType& systemMatrix)
  {
    if (initialized_)
      DUNE_THROW(Dune::InvalidStateException, "\nERROR: init() may only be called once!");
    assert(false);
    initialized_ = true;
  } // virtual void init(...)

  virtual bool apply(const VectorType& rhsVector,
                     VectorType& solutionVector,
                     const size_type maxIter = 5000,
                     const ElementType precision = 1e-12) const
  {
    if (!initialized_)
      DUNE_THROW(Dune::InvalidStateException, "\nERROR: please call init() before calling apply()!");
    assert(false);
//    solver_->setMaxIterations(maxIter);
//    solver_->setTolerance(precision);
//    solutionVector.base() = solver_->solve(rhsVector.base());
//    const ::Eigen::ComputationInfo info = solver_->info();
//    return (info == ::Eigen::Success);
  } // virtual bool apply(...)

private:
  bool initialized_;
}; // class CG


//template< class MatrixImp >
//class CgDiagonal
//  : public Interface< MatrixImp >
//{
//public:
//  typedef Interface< MatrixImp > BaseType;

//  typedef typename BaseType::MatrixType MatrixType;

//  typedef typename BaseType::VectorType VectorType;

//  typedef typename BaseType::ElementType ElementType;

//  typedef typename BaseType::size_type size_type;

//private:
//  typedef ::Eigen::DiagonalPreconditioner< ElementType > PreconditionerType;

//  typedef ::Eigen::ConjugateGradient< typename MatrixType::BaseType, ::Eigen::Lower, PreconditionerType > SolverType;

//public:
//  CgDiagonal()
//    : initialized_(false)
//  {}

//  virtual void init(const MatrixType& systemMatrix)
//  {
//    if (initialized_)
//      DUNE_THROW(Dune::InvalidStateException, "\nERROR: init() may only be called once!");
//    solver_ = new SolverType(systemMatrix.base());
//    initialized_ = true;
//  } // virtual void init(...)

//  virtual bool apply(const VectorType& rhsVector,
//                     VectorType& solutionVector,
//                     const size_type maxIter = 5000,
//                     const ElementType precision = 1e-12) const
//  {
//    if (!initialized_)
//      DUNE_THROW(Dune::InvalidStateException, "\nERROR: please call init() before calling apply()!");
//    solver_->setMaxIterations(maxIter);
//    solver_->setTolerance(precision);
//    solutionVector.base() = solver_->solve(rhsVector.base());
//    const ::Eigen::ComputationInfo info = solver_->info();
//    return (info == ::Eigen::Success);
//  } // virtual bool apply(...)

//private:
//  bool initialized_;
//  SolverType* solver_;
//}; // class CgDiagonal


//template< class MatrixImp >
//class CgIncompleteLUT
//  : public Interface< MatrixImp >
//{
//public:
//  typedef Interface< MatrixImp > BaseType;

//  typedef typename BaseType::MatrixType MatrixType;

//  typedef typename BaseType::VectorType VectorType;

//  typedef typename BaseType::ElementType ElementType;

//  typedef typename BaseType::size_type size_type;

//private:
//  typedef ::Eigen::IncompleteLUT< ElementType > PreconditionerType;

//  typedef ::Eigen::ConjugateGradient< typename MatrixType::BaseType, ::Eigen::Lower, PreconditionerType > SolverType;

//public:
//  CgIncompleteLUT()
//    : initialized_(false)
//  {}

//  virtual void init(const MatrixType& systemMatrix)
//  {
//    if (initialized_)
//      DUNE_THROW(Dune::InvalidStateException, "\nERROR: init() may only be called once!");
//    solver_ = new SolverType(systemMatrix.base());
//    initialized_ = true;
//  } // virtual void init(...)

//  virtual bool apply(const VectorType& rhsVector,
//                     VectorType& solutionVector,
//                     const size_type maxIter = 5000,
//                     const ElementType precision = 1e-12) const
//  {
//    if (!initialized_)
//      DUNE_THROW(Dune::InvalidStateException, "\nERROR: please call init() before calling apply()!");
//    solver_->setMaxIterations(maxIter);
//    solver_->setTolerance(precision);
//    solutionVector.base() = solver_->solve(rhsVector.base());
//    const ::Eigen::ComputationInfo info = solver_->info();
//    return (info == ::Eigen::Success);
//  } // virtual bool apply(...)

//private:
//  bool initialized_;
//  SolverType* solver_;
//}; // class CgIncompleteLUT


} // namespace Eigen
} // namespace Solver
} // namespace LA
} // namespace Stuff
} // namespace Dune

#endif // HAVE_EIGEN

#endif // DUNE_STUFF_LA_SOLVER_EIGEN_ITERATIVE_HH
