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

//! provide a Dummy preconditioner that fits the expectatations below
template <class Dummy>
class IdentityPreconditioner : public ::Eigen::IdentityPreconditioner
{};


template< class MatrixImp, class VectorImp, template < class ElementImp > class Preconditioner = IdentityPreconditioner>
class Bicgstab;


template< class ElementImp, template < class T > class Preconditioner >
class Bicgstab< Container::Eigen::RowMajorSparseMatrix< ElementImp >,
                Container::Eigen::DenseVector< ElementImp >,
                Preconditioner>
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
  typedef Preconditioner< ElementType > PreconditionerType;

  typedef ::Eigen::BiCGSTAB< typename MatrixType::BaseType, PreconditionerType > SolverType;

public:
  Bicgstab()
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
  : public Bicgstab<  Container::Eigen::RowMajorSparseMatrix< ElementImp >,
                      Container::Eigen::DenseVector< ElementImp >,
                      ::Eigen::IncompleteLUT>
{};

template< class MatrixImp, class VectorImp >
class BicgstabDiagonal;

template< class ElementImp >
class BicgstabDiagonal< Container::Eigen::RowMajorSparseMatrix< ElementImp >,
                        Container::Eigen::DenseVector< ElementImp > >
  : public Bicgstab<  Container::Eigen::RowMajorSparseMatrix< ElementImp >,
                      Container::Eigen::DenseVector< ElementImp >,
                      ::Eigen::DiagonalPreconditioner>
{};


template< class MatrixImp, class VectorImp >
class CG;

//! CG implementation following "van der Vorst - Iterative Methods for Large Linear Systems (2002)" p.44
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
    , systemMatrix_(nullptr)
  {}

  virtual void init(const MatrixType& systemMatrix)
  {
    systemMatrix_ = &(systemMatrix);
    initialized_ = true;
  } // virtual void init(...)

  virtual bool apply(const VectorType& rhsVector,
                     VectorType& solutionVector,
                     const size_type maxIter = 5000,
                     const ElementType precision = 1e-12) const
  {
    if (!initialized_)
      DUNE_THROW(Dune::InvalidStateException, "\nERROR: please call init() before calling apply()!");
    assert(systemMatrix_);
    auto& x_i = solutionVector.base();
    const auto& b = rhsVector.base();
    const auto& A = systemMatrix_->base();
    const int cols = A.cols();
    size_type iteration(1);
    ElementType rho(0), rho_prev(1), beta, alpha;
    const ElementType tolerance = precision * precision * b.squaredNorm();
    typename VectorType::BaseType residuum = b - A * x_i;
    typename VectorType::BaseType correction_p(cols);
    typename VectorType::BaseType correction_q(cols);
    rho = residuum.squaredNorm();
    while (iteration <= maxIter) {
      if (iteration == 1) {
          correction_p = residuum;
      }
      else {
          beta = rho / rho_prev;
          correction_p = residuum + beta * correction_p;
      }
      correction_q.noalias() = A * correction_p;
      alpha = rho / correction_p.dot(correction_q);
      x_i += alpha * correction_p;
      residuum -= alpha * correction_q;
      rho = residuum.squaredNorm();
      if (rho < tolerance)
          return true;
      rho_prev = rho;
    }
    return false;
  } // virtual bool apply(...)

private:
  bool initialized_;
  const MatrixType* systemMatrix_;
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
