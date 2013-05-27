#ifndef DUNE_STUFF_LA_SOLVER_EIGEN_HH
#define DUNE_STUFF_LA_SOLVER_EIGEN_HH

#ifdef HAVE_CMAKE_CONFIG
  #include "cmake_config.h"
#elif defined (HAVE_CONFIG_H)
  #include "config.h"
#endif // ifdef HAVE_CMAKE_CONFIG

#if HAVE_EIGEN

#include <Eigen/SparseCore>
#include <Eigen/IterativeLinearSolvers>
#include <Eigen/SparseCholesky>

#include <Eigen/SuperLUSupport>


#include <dune/stuff/common/color.hh>
#include <dune/stuff/common/parameter/configcontainer.hh>
#include <dune/stuff/common/logging.hh>
#include <dune/stuff/aliases.hh>
#include <dune/stuff/la/container/eigen.hh>

#include "interface.hh"

namespace Dune {
namespace Stuff {
namespace LA {

template <class MatrixImp, class VectorImp>
struct IsEigenMV {
  typedef typename std::is_base_of< Dune::Stuff::LA::Container::EigenMatrixInterface<typename MatrixImp::Traits>, MatrixImp>::type Mtype;
  typedef typename std::is_base_of< Dune::Stuff::LA::Container::EigenVectorInterface<typename VectorImp::Traits>, VectorImp>::type Vtype;
  static constexpr bool value = Mtype::value && Vtype::value;
};

//! \attention Slow!
//! \todo Implement via Eigen::CG and identity preconditioner!
template< class MatrixImp, class VectorImp >
class CgSolver< MatrixImp, VectorImp, typename std::enable_if<IsEigenMV<MatrixImp, VectorImp>::value>::type>
  : public SolverInterface< MatrixImp, VectorImp>
{
public:
  typedef SolverInterface< MatrixImp, VectorImp>
      BaseType;

  typedef typename BaseType::MatrixType MatrixType;
  typedef typename BaseType::VectorType VectorType;
  typedef typename BaseType::ElementType ElementType;
  typedef typename BaseType::size_type size_type;

  CgSolver()
  {
    DSC_LOG_DEBUG << "\n" << Dune::Stuff::Common::colorString("WARNING:")
                  << " this solver is believed to be slow! " << std::flush;
  }

  static Dune::ParameterTree defaultSettings()
  {
    Dune::ParameterTree description;
    description["maxIter"] = "5000";
    description["precision"] = "1e-12";
    return description;
  } // Dune::ParameterTree defaultSettings()

  virtual size_type apply(const MatrixType& systemMatrix,
                          const VectorType& rhsVector,
                          VectorType& solutionVector,
                          const size_type maxIter = 5000,
                          const ElementType precision = 1e-12,
                          const Dune::ParameterTree /*description*/ = Dune::ParameterTree()) const
  {
    auto& x_i = solutionVector.backend();
    const auto& b = rhsVector.backend();
    const auto& A = systemMatrix.backend();
    const int cols = A.cols();
    size_type iteration(1);
    ElementType rho(0), rho_prev(1), beta, alpha;
    const ElementType tolerance = precision * precision * b.squaredNorm();
    typedef typename DSLC::EigenDenseVector<typename VectorType::ElementType>::BackendType RealEigenVector;
    RealEigenVector residuum = b - A * x_i;
    RealEigenVector correction_p(cols);
    RealEigenVector correction_q(cols);
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
          return 0;
      rho_prev = rho;
    }
    return 1;
  } // virtual bool apply(...)
}; // class Cg


template< class MatrixImp, class VectorImp >
class CgDiagonalSolver< MatrixImp, VectorImp, typename std::enable_if<IsEigenMV<MatrixImp, VectorImp>::value>::type>
  : public SolverInterface< MatrixImp,
                      VectorImp >
{
public:
  typedef SolverInterface<  MatrixImp,
                      VectorImp >
      BaseType;

  typedef typename BaseType::MatrixType MatrixType;
  typedef typename BaseType::VectorType VectorType;
  typedef typename BaseType::ElementType ElementType;
  typedef typename BaseType::size_type size_type;

  CgDiagonalSolver()
  {
    DSC_LOG_DEBUG << "\n" << Dune::Stuff::Common::colorString("WARNING:", Dune::Stuff::Common::Colors::red)
                  << " this solver is believed to produce utterly wrong results! " << std::flush;
  }

  static Dune::ParameterTree defaultSettings()
  {
    Dune::ParameterTree description;
    description["maxIter"] = "5000";
    description["precision"] = "1e-12";
    return description;
  } // Dune::ParameterTree defaultSettings()

  virtual size_type apply(const MatrixType& systemMatrix,
                          const VectorType& rhsVector,
                          VectorType& solutionVector,
                          const size_type maxIter = 5000,
                          const ElementType precision = 1e-12,
                          const Dune::ParameterTree /*description*/ = Dune::ParameterTree()) const
  {
    typedef ::Eigen::ConjugateGradient< typename MatrixType::BackendType,
                                        ::Eigen::Lower,
                                        ::Eigen::DiagonalPreconditioner< ElementType > > EigenSolverType;
    EigenSolverType eigenSolver(systemMatrix.backend());
    eigenSolver.setMaxIterations(maxIter);
    eigenSolver.setTolerance(precision);
    solutionVector.backend() = eigenSolver.solve(rhsVector.backend());
    return BaseType::translateInfo(eigenSolver.info());
  } // virtual bool apply(...)
}; // class CgDiagonal


template< class MatrixImp, class VectorImp >
class BicgstabSolver< MatrixImp,
                VectorImp, typename std::enable_if<IsEigenMV<MatrixImp, VectorImp>::value>::type >
  : public SolverInterface< MatrixImp,
                      VectorImp >
{
public:
  typedef SolverInterface<  MatrixImp,
                      VectorImp >
      BaseType;

  typedef typename BaseType::MatrixType MatrixType;
  typedef typename BaseType::VectorType VectorType;
  typedef typename BaseType::ElementType ElementType;
  typedef typename BaseType::size_type size_type;

  static Dune::ParameterTree defaultSettings()
  {
    Dune::ParameterTree description;
    description["maxIter"] = "5000";
    description["precision"] = "1e-12";
    return description;
  } // Dune::ParameterTree defaultSettings()

  virtual size_type apply(const MatrixType& systemMatrix,
                          const VectorType& rhsVector,
                          VectorType& solutionVector,
                          const size_type maxIter = 5000,
                          const ElementType precision = 1e-12,
                          const Dune::ParameterTree /*description*/ = Dune::ParameterTree()) const
  {
    typedef ::Eigen::BiCGSTAB< typename MatrixType::BackendType, ::Eigen::IdentityPreconditioner > EigenSolverType;
    EigenSolverType eigenSolver(systemMatrix.backend());
    eigenSolver.setMaxIterations(maxIter);
    eigenSolver.setTolerance(precision);
    solutionVector.backend() = eigenSolver.solve(rhsVector.backend());
    return BaseType::translateInfo(eigenSolver.info());
  } // virtual bool apply(...)
}; // class Bicgstab


template< class MatrixImp, class VectorImp >
class BicgstabDiagonalSolver< MatrixImp,
                        VectorImp, typename std::enable_if<IsEigenMV<MatrixImp, VectorImp>::value>::type >
  : public SolverInterface< MatrixImp,
                      VectorImp >
{
public:
  typedef SolverInterface<  MatrixImp,
                      VectorImp >
      BaseType;

  typedef typename BaseType::MatrixType MatrixType;
  typedef typename BaseType::VectorType VectorType;
  typedef typename BaseType::ElementType ElementType;
  typedef typename BaseType::size_type size_type;

  static Dune::ParameterTree defaultSettings()
  {
    Dune::ParameterTree description;
    description["maxIter"] = "5000";
    description["precision"] = "1e-12";
    return description;
  } // Dune::ParameterTree defaultSettings()

  virtual size_type apply(const MatrixType& systemMatrix,
                          const VectorType& rhsVector,
                          VectorType& solutionVector,
                          const size_type maxIter = 5000,
                          const ElementType precision = 1e-12,
                          const Dune::ParameterTree /*description*/ = Dune::ParameterTree()) const
  {
    typedef ::Eigen::BiCGSTAB< typename MatrixType::BackendType, ::Eigen::DiagonalPreconditioner< ElementType > > EigenSolverType;
    EigenSolverType eigenSolver(systemMatrix.backend());
    eigenSolver.setMaxIterations(maxIter);
    eigenSolver.setTolerance(precision);
    solutionVector.backend() = eigenSolver.solve(rhsVector.backend());
    return BaseType::translateInfo(eigenSolver.info());
  } // virtual bool apply(...)static_assert
}; // class BicgstabDiagonal


template< class MatrixImp, class VectorImp >
class BicgstabILUTSolver< MatrixImp,
                    VectorImp, typename std::enable_if<IsEigenMV<MatrixImp, VectorImp>::value>::type >
  : public SolverInterface< MatrixImp,
                      VectorImp >
{
public:
  typedef SolverInterface<  MatrixImp,
                      VectorImp >
      BaseType;

  typedef typename BaseType::MatrixType MatrixType;
  typedef typename BaseType::VectorType VectorType;
  typedef typename BaseType::ElementType ElementType;
  typedef typename BaseType::size_type size_type;

  static Dune::ParameterTree defaultSettings()
  {
    Dune::ParameterTree description;
    description["maxIter"] = "5000";
    description["precision"] = "1e-12";
    return description;
  } // Dune::ParameterTree defaultSettings()

  virtual size_type apply(const MatrixType& systemMatrix,
                          const VectorType& rhsVector,
                          VectorType& solutionVector,
                          const size_type maxIter = 5000,
                          const ElementType precision = 1e-12,
                          const Dune::ParameterTree /*description*/ = Dune::ParameterTree()) const
  {
    typedef ::Eigen::BiCGSTAB< typename MatrixType::BackendType, ::Eigen::IncompleteLUT< ElementType > > EigenSolverType;
    EigenSolverType solver(systemMatrix.backend());
    // configure solver and preconditioner
    double solverEps = DSC_CONFIG_GET("algorithm.solver.eps", 1.0e-8);
    solver.setTolerance(solverEps);
    const int solverVerbose = DSC_CONFIG_GET("algorithm.solver.verbose", 0);
    const int maxIterations
      = DSC_CONFIG_GET("algorithm.solver.maxIterations", solutionVector.size());
    solver.setMaxIterations(maxIterations);
    double dropTol = DSC_CONFIG_GET("algorithm.solver.preconditioner.dropTol",
                                                         Eigen::NumTraits< double >::dummy_precision());
    int fillFactor = DSC_CONFIG_GET("algorithm.solver.preconditioner.fillFactor", 10);
    solver.preconditioner().setDroptol(dropTol);
    solver.preconditioner().setFillfactor(fillFactor);


    solutionVector.backend() = solver.solve(rhsVector.backend());
    return BaseType::translateInfo(solver.info());
  } // virtual bool apply(...)
}; // class BicgstabILUT



//template< class MatrixImp, class VectorImp >
//class SimplicialLLT< MatrixImp,
//                      VectorImp, typename std::enable_if<IsEigenMV<MatrixImp, VectorImp>::value>::type >
//  : public Interface< MatrixImp,
//                      VectorImp >
//{
//public:
//  typedef Interface<  MatrixImp,
//                      VectorImp >
//      BaseType;

//  typedef typename BaseType::MatrixType MatrixType;
//  typedef typename BaseType::VectorType VectorType;
//  typedef typename BaseType::ElementType ElementType;
//  typedef typename BaseType::size_type size_type;

//  SimplicialLLT()
//  {}

//  virtual size_type apply(const MatrixType& systemMatrix,
//                          const VectorType& rhsVector,
//                          VectorType& solutionVector,
//                          const size_type /*maxIter*/ = 5000,
//                          const ElementType /*precision*/ = 1e-12,
//                          const Dune::ParameterTree /*description*/ = Dune::ParameterTree()) const override
//  {
//    typedef ::Eigen::SimplicialLLT< typename MatrixType::BackendType, ::Eigen::Lower > EigenSolverType;
//    EigenSolverType eigenSolver(systemMatrix.backend());
//    solutionVector.backend() = eigenSolver.solve(rhsVector.backend());
//    const ::Eigen::ComputationInfo info = eigenSolver.info();
//    return (info == ::Eigen::Success);
//  } // virtual bool apply(...)
//}; // class SimplicialLLT



template< class MatrixImp, class VectorImp >
class SimplicialLDLTSolver< MatrixImp,
                      VectorImp, typename std::enable_if<IsEigenMV<MatrixImp, VectorImp>::value>::type >
  : public SolverInterface< MatrixImp,
                      VectorImp >
{
public:
  typedef SolverInterface<  MatrixImp,
                      VectorImp >
      BaseType;

  typedef typename BaseType::MatrixType MatrixType;
  typedef typename BaseType::VectorType VectorType;
  typedef typename BaseType::ElementType ElementType;
  typedef typename BaseType::size_type size_type;

  SimplicialLDLTSolver()
  {}

  virtual size_type apply(const MatrixType& systemMatrix,
                          const VectorType& rhsVector,
                          VectorType& solutionVector,
                          const size_type /*maxIter*/ = 5000,
                          const ElementType /*precision*/ = 1e-12,
                          const Dune::ParameterTree /*description*/ = Dune::ParameterTree()) const override
  {
//    typedef ::Eigen::SimplicialLDLT< typename MatrixType::BackendType> EigenSolverType;
    typedef ::Eigen::SuperLU< typename MatrixType::BackendType> EigenSolverType;
    EigenSolverType eigenSolver;
    eigenSolver.compute(systemMatrix.backend());
    auto retval = eigenSolver.solve(rhsVector.backend());
//    retval.evalTo(solutionVector.backend());
    solutionVector.backend() = retval;
    const ::Eigen::ComputationInfo info = eigenSolver.info();
    return (info == ::Eigen::Success);
  } // virtual bool apply(...)
}; // class SimplicialLDLT


} // namespace LA
} // namespace Stuff
} // namespace Dune

#endif // HAVE_EIGEN

#endif // DUNE_STUFF_LA_SOLVER_EIGEN_HH
