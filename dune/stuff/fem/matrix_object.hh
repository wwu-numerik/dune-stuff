#ifndef DUNE_STUFF_MATRIX_OBJECT_HH
#define DUNE_STUFF_MATRIX_OBJECT_HH

#if 1 //defined HAVE_DUNE_FEM && HAVE_EIGEN

#include <dune/fem/function/adaptivefunction/adaptivefunction.hh>
#include <dune/fem/solver/oemsolver.hh>
#include <dune/fem/operator/common/operator.hh>
#include <dune/fem/operator/common/localmatrix.hh>
#include <dune/fem/operator/common/localmatrixwrapper.hh>
#include <dune/fem/space/lagrangespace/lagrangespace.hh>
#include <dune/fem/misc/functor.hh>
#include <dune/stuff/common/matrix.hh>
#include <dune/stuff/la/container/eigen.hh>
#include <dune/stuff/la/container/pattern.hh>
#include <dune/stuff/la/solver.hh>
#include <dune/stuff/la/solver/interface.hh>
#include <dune/stuff/aliases.hh>
#include <dune/stuff/common/random.hh>
#include <dune/stuff/common/print.hh>

#include <Eigen/IterativeLinearSolvers>
#include <Eigen/SparseCholesky>

#include <memory>

namespace Dune {
namespace Stuff {
namespace Fem {

template< class DomainSpace, class RangeSpace, class TraitsImp >
class EigenMatrixObject;

template <class DomainSpace, class RangeSpace = DomainSpace>
struct EigenMatrixObjectTraits
{
  typedef DomainSpace RowSpaceType;
  typedef RangeSpace ColumnSpaceType;
  typedef EigenMatrixObjectTraits<DomainSpace,RangeSpace> ThisType;

  template <class OperatorTraits>
  struct MatrixObject
  {
    typedef EigenMatrixObject<DomainSpace, RangeSpace, OperatorTraits> MatrixObjectType;
  };
};

template< class DomainSpace, class RangeSpace, class TraitsImp >
class LocalEigenMatrix;

template< class DomainSpace, class RangeSpace, class TraitsImp >
struct LocalEigenMatrixTraits
{
  typedef DomainSpace DomainSpaceType;
  typedef RangeSpace RangeSpaceType;

  typedef LocalEigenMatrix< DomainSpaceType,RangeSpaceType, TraitsImp > LocalMatrixType;

  typedef typename RangeSpaceType :: RangeFieldType RangeFieldType;
  typedef RangeFieldType LittleBlockType;
};

template< class DomainSpace, class RangeSpace, class TraitsImp >
class LocalEigenMatrix
    : public Dune::LocalMatrixDefault< LocalEigenMatrixTraits<DomainSpace, RangeSpace, TraitsImp > >
{
  private:
  typedef Dune::LocalMatrixDefault<LocalEigenMatrixTraits<DomainSpace, RangeSpace, TraitsImp > >
    BaseType;
  typedef EigenMatrixObject<DomainSpace, RangeSpace, TraitsImp >
    MatrixObjectType;
  typedef typename MatrixObjectType :: MatrixType
    MatrixType;

  typedef DomainSpace DomainSpaceType;
  typedef RangeSpace RangeSpaceType;
  typedef typename DomainSpace :: EntityType  DomainEntityType ;
  typedef typename RangeSpace :: EntityType   RangeEntityType ;
  //! type of entries of little blocks
  typedef typename RangeSpaceType :: RangeFieldType
  RangeFieldType;

  //! type of the DoFs
  typedef RangeFieldType DofType;

  MatrixType& matrix_;

  //! global index in the DomainSpace
  std :: vector< int > rowMapper_;

  //! global index in the RangeSpace
  std :: vector< int > colMapper_;

  using BaseType :: domainSpace_;
  using BaseType :: rangeSpace_;

public:
  //! constructor taking entity and spaces for using mapToGlobal
  inline LocalEigenMatrix( const MatrixObjectType& matrixObject,
                           const DomainSpaceType& domainSpace,
                           const RangeSpaceType& rangeSpace,
                           const DomainEntityType& domainEntity,
                           const RangeEntityType& rangeEntity)
    : BaseType( domainSpace, rangeSpace)
    , matrix_( matrixObject.matrix() )
  {
      // initialize base functions sets
      BaseType::init( domainEntity, rangeEntity );

      // rows are determined by the range space
      rowMapper_.resize( rangeSpace_.mapper().numDofs( rangeEntity ) );
      rangeSpace_.mapper().mapEach( rangeEntity, Dune::Fem::AssignFunctor< std::vector< int > >( rowMapper_ ) );

      // columns are determind by the domain space
      colMapper_.resize( domainSpace_.mapper().numDofs( domainEntity ) );
      domainSpace_.mapper().mapEach( domainEntity, Dune::Fem::AssignFunctor< std::vector< int > >( colMapper_ ) );
  }

  //! return number of rows
  int rows () const
  {
    return rowMapper_.size();
  }

  //! return number of columns
  int columns () const
  {
    return colMapper_.size();
  }

  int cols () const
  {
    return colMapper_.size();
  }

  //! add value to matrix entry
  void add( int localRow, int localCol, const DofType value )
  {
    assert(!std::isnan(value));
    index_checks(localRow, localCol);
    matrix_.add(rowMapper_[ localRow ], colMapper_[ localCol ],value);
  }

  //! get matrix entry
  DofType get( int localRow, int localCol ) const
  {
    index_checks(localRow, localCol);
    return matrix_.get( rowMapper_[ localRow ], colMapper_[ localCol ] );
  }

  //! set matrix entry to value
  void set( int localRow, int localCol, const DofType value )
  {
    assert(!std::isnan(value));
    index_checks(localRow, localCol);
    matrix_.set( rowMapper_[ localRow ], colMapper_[ localCol ], value );
  }

  //! set matrix row to zero except diagonla entry
  void unitRow( const int localRow )
  {
    index_checks(localRow, 0);
    matrix_.unitRow( rowMapper_[ localRow ] );
  }

  //! set matrix row to zero
  void clearRow( const int localRow )
  {
    index_checks(localRow, 0);
    matrix_.clearRow( rowMapper_[localRow]);
  }

  DofType operator() (int localRow, int localCol ) const {
      index_checks(localRow, localCol);
      return get(localRow, localCol);
  }

private:
  inline void index_checks(int localRow, int localCol ) const {
      assert( (localRow >= 0) );
      assert( (localRow < rows()) );
      assert( (localCol >= 0) );
      assert( (localCol < columns()) );
  }

};

class LagrangePattern : public DSL::SparsityPatternDefault {

    typedef DSL::SparsityPatternDefault BaseType;
public:
    template< class D_FunctionSpace, class D_GridPart, int D_polOrder, template< class > class D_Storage,
              class R_FunctionSpace, class R_GridPart, int R_polOrder, template< class > class R_Storage>
    LagrangePattern(const Dune::LagrangeDiscreteFunctionSpace<D_FunctionSpace,
                                            D_GridPart, D_polOrder, D_Storage>& domain_space,
                    const Dune::LagrangeDiscreteFunctionSpace<R_FunctionSpace,
                                            R_GridPart, R_polOrder, R_Storage>& range_space,
                    const bool non_conform = false)
        : BaseType(domain_space.size())
    {
        for( const auto& entity : domain_space )
        {
            const auto& gridPart = domain_space.gridPart();
            const auto& rowMapper = domain_space.mapper();
            const auto& colMapper = range_space.mapper();
            const auto rowEnd = rowMapper.end( entity );
            for(auto rowIt = rowMapper.begin( entity );
                rowIt != rowEnd; ++rowIt )
            {
              auto& localIndices = BaseType::set( rowIt.global() );

              const auto colEnd = colMapper.end( entity );
              auto colIt = colMapper.begin( entity );
              for( ; colIt != colEnd; ++colIt )
                localIndices.insert( colIt.global() );
            }
            if(non_conform)
            {
              const auto endit = gridPart.iend( entity );
              for( auto it = gridPart.ibegin( entity );
                   it != endit; ++it )
              {
                const auto& intersection = *it;
                if( intersection.neighbor() && ! intersection.conforming() )
                {
                  auto ep = intersection.outside();
                  const auto& neighbor = *ep;

                  // also add all neighbor dofs
                  for(auto rowIt = rowMapper.begin( entity );
                      rowIt != rowEnd; ++rowIt )
                  {
                    auto& localIndices =  BaseType::set( rowIt.global() );
                    const auto colEnd = colMapper.end( neighbor );
                    for(auto colIt = colMapper.begin( neighbor );
                        colIt != colEnd; ++colIt )
                    {
                      localIndices.insert( colIt.global() );
                    }
                  }
                }
              }
            }
        }
    }
};



template< class DomainSpace, class RangeSpace, class TraitsImp >
class EigenMatrixObject //: public Dune::OEMMatrix
{
  typedef typename DomainSpace::EntityType  DomainEntityType ;
  typedef typename RangeSpace::EntityType   RangeEntityType ;

public:

  typedef TraitsImp Traits;

  typedef DomainSpace DomainSpaceType;
  typedef RangeSpace RangeSpaceType;

  typedef DSL::EigenRowMajorSparseMatrix< typename DomainSpace::RangeFieldType > MatrixType;
  typedef MatrixType PreconditionMatrixType;

private:
  typedef EigenMatrixObject< DomainSpace, RangeSpace, Traits > ThisType;
  typedef Eigen::Matrix<double,Eigen::Dynamic, 1> EigenVectorType;
  typedef Eigen::Map<EigenVectorType> EigenVectorWrapperType;
  typedef Eigen::Map<const EigenVectorType> ConstEigenVectorWrapperType;

  const DomainSpaceType& domainSpace_;
  const RangeSpaceType& rangeSpace_;
  int sequence_;

  mutable MatrixType matrix_;
  bool preconditioning_;
  const DSL::SparsityPatternDefault& sparsity_pattern_;

public:
  //! type of local matrix
  typedef LocalEigenMatrix< DomainSpace, RangeSpace, Traits> LocalMatrixType;

  inline EigenMatrixObject( const DomainSpaceType& domainSpace,
                            const RangeSpaceType& rangeSpace,
                            const DSL::SparsityPatternDefault& sparsity_pattern )
    : domainSpace_(domainSpace)
    , rangeSpace_(rangeSpace)
    , sequence_(-1)
    , matrix_()
    , preconditioning_(false)
    , sparsity_pattern_(sparsity_pattern)
  {
  }

  inline MatrixType& matrix() const
  {
    return matrix_;
  }

  //! return local matrix
  inline LocalMatrixType localMatrix( const DomainEntityType& domainEntity,
                                      const RangeEntityType& rangeEntity ) const
  {
    return LocalMatrixType(*this, domainSpace_, rangeSpace_, domainEntity, rangeEntity);
  }

  //! resize all matrices and clear them
  inline void clear ()
  {
      matrix_.backend().setZero();
  }

  //! return true if precoditioning matrix is provided
  bool hasPreconditionMatrix () const
  {
    return preconditioning_;
  }

  //! return reference to preconditioner
  const PreconditionMatrixType &preconditionMatrix () const
  {
    return matrix_;
  }

  //! reserve memory corresponnding to size of spaces
  inline void reserve(bool verbose = false )
  {
    if( sequence_ != domainSpace_.sequence() )
    {
      sequence_ = domainSpace_.sequence();
    }
    matrix_ = MatrixType(domainSpace_.size(), rangeSpace_.size(), sparsity_pattern_);
  }

  //! mult method of matrix object used by oem solver
  double ddotOEM( const double* v, const double* w ) const
  {
    typedef Dune::AdaptiveDiscreteFunction< DomainSpaceType > DomainFunctionType;
    DomainFunctionType V( "ddot V", domainSpace_, v );
    DomainFunctionType W( "ddot W", domainSpace_, w );
    return V.scalarProductDofs( W );
  }

  //! mult method of matrix object used by oem solver
  void multOEM( const double* arg, double* dest ) const
  {
    const EigenVectorWrapperType arg_w(const_cast<double*>(arg), domainSpace_.size());
    EigenVectorWrapperType dest_w(dest, rangeSpace_.size());
    dest_w = matrix_.backend() * arg_w;
  }

  void multOEM_t( const double* arg, double* dest ) const
  {
    const EigenVectorWrapperType arg_w(const_cast<double*>(arg), rangeSpace_.size());
    EigenVectorWrapperType dest_w(dest, domainSpace_.size());
    dest_w = matrix_.backend().transpose() * arg_w;
  }

  void apply ( const AdaptiveDiscreteFunction< DomainSpaceType >& arg,
               AdaptiveDiscreteFunction< RangeSpaceType>& dest ) const
  {
    multOEM(arg.leakPointer(), dest.leakPointer());
  }

  void apply_t( const AdaptiveDiscreteFunction< RangeSpaceType >& arg,
                AdaptiveDiscreteFunction< DomainSpaceType>& dest ) const
  {
    multOEM_t(arg.leakPointer(), dest.leakPointer());
  }

};

template< class DomainFunction, class RangeFunction, class TraitsImp >
class EigenMatrixOperator
: public EigenMatrixObject< typename DomainFunction::DiscreteFunctionSpaceType, typename RangeFunction::DiscreteFunctionSpaceType, TraitsImp >,
  public Operator< typename DomainFunction::RangeFieldType, typename RangeFunction::RangeFieldType, DomainFunction, RangeFunction >
{
  typedef EigenMatrixOperator< DomainFunction, RangeFunction, TraitsImp > This;
  typedef EigenMatrixObject< typename DomainFunction::DiscreteFunctionSpaceType, typename RangeFunction::DiscreteFunctionSpaceType, TraitsImp > Base;

public:
  typedef typename Base::DomainSpaceType DomainSpaceType;
  typedef typename Base::RangeSpaceType RangeSpaceType;

  /** \copydoc Fem::Operator::assembled */
  static const bool assembled = true ;

  using Base::apply;

  EigenMatrixOperator ( const DomainSpaceType &domainSpace,
                            const RangeSpaceType &rangeSpace,
                            const DSL::SparsityPatternDefault& pattern )
  : Base( domainSpace, rangeSpace, pattern )
  {}

  virtual void operator() ( const DomainFunction &arg, RangeFunction &dest ) const
  {
    Base::apply( arg, dest );
  }

  const Base &systemMatrix () const
  {
    return *this;
  }
};


template< class DomainDiscreteFunctionType, class MatrixOperatorType >
class EigenInverseOperator {

    typedef DSL::EigenMappedDenseVector<typename DomainDiscreteFunctionType::RangeFieldType> EigenVectorWrapperType;
    typedef typename MatrixOperatorType::MatrixType MatrixType;

    const MatrixType& matrix_;
    const Dune::ParameterTree& solver_settings_;
public:

  EigenInverseOperator( const MatrixOperatorType& matrix_operator, const Dune::ParameterTree& solver_settings )
    : matrix_(matrix_operator.matrix())
    , solver_settings_(solver_settings)
  {}

  static Dune::ParameterTree defaultSettings()
  {
    auto settings = DSL::BicgstabILUTSolver<MatrixType, EigenVectorWrapperType>::defaultSettings();
    settings["type"] = "bicgstab.ilut";
    return settings;
  }

  template <class DomainVector, class RangeVector>
  void operator()(const DomainVector& arg, RangeVector& x) const {
      const EigenVectorWrapperType arg_w(const_cast<double*>(arg.leakPointer()), arg.size());
      EigenVectorWrapperType x_w(x.leakPointer(), x.size());
      typedef DSL::SolverInterface<MatrixType, EigenVectorWrapperType> SolverType;
      std::unique_ptr<SolverType> solver(DSL::createSolver<MatrixType,
                                                           EigenVectorWrapperType>(solver_settings_["type"]));
      solver->apply(matrix_, arg_w, x_w, solver_settings_);
  }
};

} // namespace Fem
} // namespace Stuff
} // namespace Dune

#endif // defined HAVE_DUNE_FEM && HAVE_EIGEN

#endif // DUNE_STUFF_MATRIX_OBJECT_HH
