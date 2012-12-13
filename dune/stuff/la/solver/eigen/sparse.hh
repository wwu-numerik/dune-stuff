#ifndef DUNE_STUFF_LA_SOLVER_SPARSE_EIGEN_HH
#define DUNE_STUFF_LA_SOLVER_SPARSE_EIGEN_HH

#if HAVE_EIGEN

#include <dune/common/exceptions.hh>

#include <dune/stuff/la/container/eigen.hh>

namespace Dune {
namespace Stuff {
namespace LA {
namespace Solver {
namespace Eigen {
namespace Sparse {

template< class ElementType = double >
class Interface
{
public:
  typedef Dune::Stuff::LA::Container::Eigen::SparseMatrix< ElementType > SparseMatrixType;

  typedef Dune::Stuff::LA::Container::Eigen::DenseVector< ElementType > DenseVectorType;

  virtual bool apply(const SparseMatrixType& /*systemMatrix*/,
                     const DenseVectorType& /*rhsVector*/,
                     DenseVectorType& /*solutionVector*/,
                     unsigned int /*maxIter*/,
                     double /*precision*/) const = 0;
}; // class Interface


template< class ElementType = double >
class BicgstabIlut
  : public Interface< ElementType >
{
public:
  typedef Interface< ElementType > BaseType;

  typedef typename BaseType::SparseMatrixType SparseMatrixType;

  typedef typename BaseType::DenseVectorType DenseVectorType;

  virtual bool apply(const SparseMatrixType& systemMatrix,
                     const DenseVectorType& rhsVector,
                     DenseVectorType& solutionVector,
                     unsigned int maxIter = 5000,
                     double precision = 1e-12) const
  {
    typedef ::Eigen::IncompleteLUT< ElementType > PreconditionerType;
    typedef ::Eigen::BiCGSTAB< typename SparseMatrixType::BaseType, PreconditionerType > SolverType;
    SolverType solver;
    solver.setMaxIterations(maxIter);
    solver.setTolerance(precision);
    solver.compute(systemMatrix.base());
    solutionVector.base() = solver.solve(rhsVector.base());
    const ::Eigen::ComputationInfo info = solver.info();
    return (info == ::Eigen::Success);
  } // virtual bool apply(...)
}; // class BicgstabIlut


template< class ElementType = double >
class BicgstabDiagonal
  : public Interface< ElementType >
{
public:
  typedef Interface< ElementType > BaseType;

  typedef typename BaseType::SparseMatrixType SparseMatrixType;

  typedef typename BaseType::DenseVectorType DenseVectorType;

  virtual bool apply(const SparseMatrixType& systemMatrix,
                     const DenseVectorType& rhsVector,
                     DenseVectorType& solutionVector,
                     unsigned int maxIter = 5000,
                     double precision = 1e-12) const
  {
    typedef ::Eigen::DiagonalPreconditioner< ElementType > PreconditionerType;
    typedef ::Eigen::BiCGSTAB< typename SparseMatrixType::BaseType, PreconditionerType > SolverType;
    SolverType solver;
    solver.setMaxIterations(maxIter);
    solver.setTolerance(precision);
    solver.compute(systemMatrix.base());
    solutionVector.base() = solver.solve(rhsVector.base());
    const ::Eigen::ComputationInfo info = solver.info();
    return (info == ::Eigen::Success);
  }
}; // class BicgstabDiagonal


template< class ElementType = double >
class CgDiagonalUpper
    : public Interface< ElementType >
{
public:
  typedef Interface< ElementType > BaseType;

  typedef typename BaseType::SparseMatrixType SparseMatrixType;

  typedef typename BaseType::DenseVectorType DenseVectorType;

  virtual bool apply(const SparseMatrixType& systemMatrix,
                     const DenseVectorType& rhsVector,
                     DenseVectorType& solutionVector,
                     unsigned int maxIter = 5000,
                     double precision = 1e-12) const
  {
    typedef ::Eigen::DiagonalPreconditioner< ElementType > PreconditionerType;
    typedef ::Eigen::ConjugateGradient< typename SparseMatrixType::BaseType, ::Eigen::Upper, PreconditionerType > SolverType;
    SolverType solver;
    solver.setMaxIterations(maxIter);
    solver.setTolerance(precision);
    solver.compute(systemMatrix.base());
    solutionVector.base() = solver.solve(rhsVector.base());
    const ::Eigen::ComputationInfo info = solver.info();
    return (info == ::Eigen::Success);
  }
}; // class CgDiagonalUpper


template< class ElementType = double >
class CgDiagonalLower
    : public Interface< ElementType >
{
public:
  typedef Interface< ElementType > BaseType;

  typedef typename BaseType::SparseMatrixType SparseMatrixType;

  typedef typename BaseType::DenseVectorType DenseVectorType;

  virtual bool apply(const SparseMatrixType& systemMatrix,
                     const DenseVectorType& rhsVector,
                     DenseVectorType& solutionVector,
                     unsigned int maxIter = 5000,
                     double precision = 1e-12) const
  {
    typedef ::Eigen::DiagonalPreconditioner< ElementType > PreconditionerType;
    typedef ::Eigen::ConjugateGradient< typename SparseMatrixType::BaseType, ::Eigen::Lower, PreconditionerType > SolverType;
    SolverType solver;
    solver.setMaxIterations(maxIter);
    solver.setTolerance(precision);
    solver.compute(systemMatrix.base());
    solutionVector.base() = solver.solve(rhsVector.base());
    const ::Eigen::ComputationInfo info = solver.info();
    return (info == ::Eigen::Success);
  }
}; // class CgDiagonalLower


template< class ElementType = double >
class SimplicialcholeskyUpper
    : public Interface< ElementType >
{
public:
  typedef Interface< ElementType > BaseType;

  typedef typename BaseType::SparseMatrixType SparseMatrixType;

  typedef typename BaseType::DenseVectorType DenseVectorType;

  virtual bool apply(const SparseMatrixType& systemMatrix,
                     const DenseVectorType& rhsVector,
                     DenseVectorType& solutionVector,
                     unsigned int DUNE_UNUSED(maxIter) = 5000,
                     double DUNE_UNUSED(precision) = 1e-12) const
  {
    typedef ::Eigen::SimplicialCholesky< SparseMatrixType, ::Eigen::Upper > SolverType;
    SolverType solver;
    solver.compute(systemMatrix.base());
    solutionVector.base() = solver.solve(rhsVector.base());
    const ::Eigen::ComputationInfo info = solver.info();
    return (info == ::Eigen::Success);
  }
}; // class SimplicialcholeskyUpper


template< class ElementType = double >
class SimplicialcholeskyLower
    : public Interface< ElementType >
{
public:
  typedef Interface< ElementType > BaseType;

  typedef typename BaseType::SparseMatrixType SparseMatrixType;

  typedef typename BaseType::DenseVectorType DenseVectorType;

  virtual bool apply(const SparseMatrixType& systemMatrix,
                     const DenseVectorType& rhsVector,
                     DenseVectorType& solutionVector,
                     unsigned int DUNE_UNUSED(maxIter) = 5000,
                     double DUNE_UNUSED(precision) = 1e-12) const
  {
    typedef ::Eigen::SimplicialCholesky< SparseMatrixType, ::Eigen::Lower > SolverType;
    SolverType solver;
    solver.compute(systemMatrix.base());
    solutionVector.base() = solver.solve(rhsVector.base());
    const ::Eigen::ComputationInfo info = solver.info();
    return (info == ::Eigen::Success);
  }
}; // class SimplicialcholeskyLower


template< class ElementType = double >
Interface< ElementType >* create(const std::string type = "eigen.bicgstab.incompletelut")
{
  if (type == "eigen.bicgstab.incompletelut") {
    typedef BicgstabIlut< ElementType > BicgstabIlutType;
    BicgstabIlutType* bicgstabIlut = new BicgstabIlutType;
    return bicgstabIlut;
  } else if (type == "eigen.bicgstab.diagonal") {
    typedef BicgstabDiagonal< ElementType > BicgstabDiagonalType;
    BicgstabDiagonalType* bicgstabDiagonal = new BicgstabDiagonalType;
    return bicgstabDiagonal;
  } else if (type == "eigen.cg.diagonal.upper") {
    typedef CgDiagonalUpper< ElementType > CgDiagonalUpperType;
    CgDiagonalUpperType* cgDiagonalUpper = new CgDiagonalUpperType;
    return cgDiagonalUpper;
  } else if (type == "eigen.cg.diagonal.lower") {
    typedef CgDiagonalLower< ElementType > CgDiagonalLowerType;
    CgDiagonalLowerType* cgDiagonalLower = new CgDiagonalLowerType;
    return cgDiagonalLower;
  } else if (type == "eigen.simplicialcholesky.upper") {
    typedef SimplicialcholeskyUpper< ElementType > SimplicialcholeskyUpperType;
    SimplicialcholeskyUpperType* simplicialcholeskyUpper = new SimplicialcholeskyUpperType;
    return simplicialcholeskyUpper;
  } else if (type == "eigen.simplicialcholesky.lower") {
    typedef SimplicialcholeskyLower< ElementType > SimplicialcholeskyLowerType;
    SimplicialcholeskyLowerType* simplicialcholeskyLower = new SimplicialcholeskyLowerType;
    return simplicialcholeskyLower;
  } else
    DUNE_THROW(Dune::RangeError,
               "\nERROR: unknown linear solver '" << type << "' requested!");
} // Interface< ElementType >* create(const std::string type = "eigen.bicgstab.incompletelut")

} // namespace Sparse
} // namespace Eigen
} // namespace Solver
} // namespace LA
} // namespace Stuff
} // namespace Dune

#endif // HAVE_EIGEN

#endif // DUNE_STUFF_LA_SOLVER_SPARSE_EIGEN_HH
