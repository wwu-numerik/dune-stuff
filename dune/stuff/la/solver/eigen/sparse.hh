#ifndef DUNE_STUFF_LA_SOLVER_SPARSE_EIGEN_HH
#define DUNE_STUFF_LA_SOLVER_SPARSE_EIGEN_HH

#ifdef HAVE_EIGEN

#include <Eigen/Eigen>

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
}; // struct BicgstabIlut

//struct BicgstabDiagonal
//{
//public:
//  static const std::string id;

//  template< class MatrixType, class SolutionType, class RhsType >
//  static void apply(MatrixType& matrix, SolutionType& solution, RhsType& rhs, unsigned int maxIter = 5000, double precision = 1e-12)
//  {
//    typedef typename MatrixType::EntryType EntryType;
//    typedef typename MatrixType::StorageType EigenMatrixType;
//    typedef ::Eigen::DiagonalPreconditioner< EntryType > PreconditionerType;
//    typedef ::Eigen::BiCGSTAB< EigenMatrixType, PreconditionerType > SolverType;
//    SolverType solver;
//    solver.setMaxIterations(maxIter);
//    solver.setTolerance(precision);
//    solver.compute(*(matrix.storage()));
//    *(solution.storage()) = solver.solve(*(rhs.storage()));
//  }
//}; // struct BicgstabDiagonal

//const std::string BicgstabDiagonal::id = "eigen.bicgstab.diagonal";

//struct CgDiagonalUpper
//{
//public:
//  static const std::string id;

//  template< class MatrixType, class SolutionType, class RhsType >
//  static void apply(MatrixType& matrix, SolutionType& solution, RhsType& rhs, unsigned int maxIter = 5000, double precision = 1e-12)
//  {
//    typedef typename MatrixType::EntryType EntryType;
//    typedef typename MatrixType::StorageType EigenMatrixType;
//    typedef ::Eigen::DiagonalPreconditioner< EntryType > PreconditionerType;
//    typedef ::Eigen::ConjugateGradient< EigenMatrixType, ::Eigen::Upper, PreconditionerType > SolverType;
//    SolverType solver;
//    solver.setMaxIterations(maxIter);
//    solver.setTolerance(precision);
//    solver.compute(*(matrix.storage()));
//    *(solution.storage()) = solver.solve(*(rhs.storage()));
//  }
//}; // struct CgDiagonalUpper

//const std::string CgDiagonalUpper::id = "eigen.cg.diagonal.upper";

//struct CgDiagonalLower
//{
//public:
//  static const std::string id;

//  template< class MatrixType, class SolutionType, class RhsType >
//  static void apply(MatrixType& matrix, SolutionType& solution, RhsType& rhs, unsigned int maxIter = 5000, double precision = 1e-12)
//  {
//    typedef typename MatrixType::EntryType EntryType;
//    typedef typename MatrixType::StorageType EigenMatrixType;
//    typedef ::Eigen::DiagonalPreconditioner< EntryType > PreconditionerType;
//    typedef ::Eigen::ConjugateGradient< EigenMatrixType, ::Eigen::Lower, PreconditionerType > SolverType;
//    SolverType solver;
//    solver.setMaxIterations(maxIter);
//    solver.setTolerance(precision);
//    solver.compute(*(matrix.storage()));
//    *(solution.storage()) = solver.solve(*(rhs.storage()));
//  }
//}; // struct CgDiagonalLower

//const std::string CgDiagonalLower::id = "eigen.cg.diagonal.lower";

//struct SimplicialcholeskyUpper
//{
//public:
//  static const std::string id;

//  template< class MatrixType, class SolutionType, class RhsType >
//  static void apply(MatrixType& matrix, SolutionType& solution, RhsType& rhs, unsigned int maxIter = 5000, double precision = 1e-12)
//  {
//    typedef typename MatrixType::EntryType EntryType;
//    typedef typename MatrixType::StorageType EigenMatrixType;
//    typedef ::Eigen::SimplicialCholesky< EigenMatrixType, ::Eigen::Upper > SolverType;
//    SolverType solver;
//    solver.compute(*(matrix.storage()));
//    *(solution.storage()) = solver.solve(*(rhs.storage()));
//  }
//}; // struct SimplicialcholeskyUpper

//const std::string SimplicialcholeskyUpper::id = "eigen.simplicialcholesky.upper";

//struct SimplicialcholeskyLower
//{
//public:
//  static const std::string id;

//  template< class MatrixType, class SolutionType, class RhsType >
//  static void apply(MatrixType& matrix, SolutionType& solution, RhsType& rhs, unsigned int maxIter = 5000, double precision = 1e-12)
//  {
//    typedef typename MatrixType::EntryType EntryType;
//    typedef typename MatrixType::StorageType EigenMatrixType;
//    typedef ::Eigen::SimplicialCholesky< EigenMatrixType, ::Eigen::Lower > SolverType;
//    SolverType solver;
//    solver.compute(*(matrix.storage()));
//    *(solution.storage()) = solver.solve(*(rhs.storage()));
//  }
//}; // struct SimplicialcholeskyLower

//const std::string SimplicialcholeskyLower::id = "eigen.simplicialcholesky.lower";

template< class ElementType = double >
Interface< ElementType >* create(const std::string type = "eigen.bicgstab.incompletelut")
{
  if (type == "eigen.bicgstab.incompletelut") {
    typedef BicgstabIlut< ElementType > BicgstabIlutType;
    BicgstabIlutType* bicgstabIlut = new BicgstabIlutType;
    return bicgstabIlut;
  } else
    DUNE_THROW(Dune::RangeError,
               "\nERROR: unknown solver '" << type << "' requested!");
} // Interface< ElementType >* create(const std::string type = "eigen.bicgstab.incompletelut")


} // namespace Sparse
} // namespace Eigen
} // namespace Solver
} // namespace LA
} // namespace Stuff
} // namespace Dune

#endif // HAVE_EIGEN

#endif // DUNE_STUFF_LA_SOLVER_SPARSE_EIGEN_HH
