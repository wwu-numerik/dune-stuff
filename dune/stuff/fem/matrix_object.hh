#ifndef DUNE_STUFF_MATRIX_OBJECT_HH
#define DUNE_STUFF_MATRIX_OBJECT_HH

#if HAVE_FEM && HAVE_EIGEN

#include <dune/fem/function/adaptivefunction/adaptivefunction.hh>
#include <dune/fem/solver/oemsolver.hh>
#include <dune/fem/operator/common/operator.hh>
#include <dune/fem/operator/common/localmatrix.hh>
#include <dune/fem/operator/common/localmatrixwrapper.hh>
#include <dune/stuff/common/matrix.hh>
#include <dune/stuff/la/container/eigen.hh>
#include <dune/stuff/la/container/pattern.hh>
#include <dune/stuff/aliases.hh>

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
  //! global row numbers
  std :: vector< int > row_;
  //! global col numbers
  std :: vector< int > col_;

  using BaseType :: domainSpace_;
  using BaseType :: rangeSpace_;

public:
  //! constructor taking entity and spaces for using mapToGlobal
  inline LocalEigenMatrix( const MatrixObjectType& matrixObject,
                           const DomainSpaceType& domainSpace,
                           const RangeSpaceType& rangeSpace,
                           const DomainEntityType& rowEntity,
                           const RangeEntityType& colEntity)
    : BaseType( domainSpace, rangeSpace)
    , matrix_( matrixObject.matrix() )
  {
    // initialize base functions sets
    BaseType :: init ( rowEntity , colEntity );

    row_.resize( domainSpace_.baseFunctionSet( rowEntity ).numBaseFunctions() );
    col_.resize( rangeSpace_.baseFunctionSet( colEntity ).numBaseFunctions() );

    // Martin: shouldn't domainSpace and rangeSpace be flipped, here?
    const auto dmend = domainSpace_.mapper().end( rowEntity );
    for(auto dmit = domainSpace_.mapper().begin( rowEntity ); dmit != dmend; ++dmit)
    {
      assert( dmit.global() == domainSpace_.mapToGlobal( rowEntity, dmit.local() ) );
      row_[ dmit.local() ] = dmit.global();
    }

    const auto rmend = rangeSpace_.mapper().end( colEntity );
    for(auto rmit = rangeSpace_.mapper().begin( colEntity ); rmit != rmend; ++rmit)
    {
      assert( rmit.global() == rangeSpace_.mapToGlobal( colEntity, rmit.local() ) );
      col_[ rmit.local() ] = rmit.global();
    }
  }

  //! return number of rows
  int rows () const
  {
    return row_.size();
  }

  //! return number of columns
  int columns () const
  {
    return col_.size();
  }

  //! add value to matrix entry
  void add( int localRow, int localCol, const DofType value )
  {
    assert( value == value );
    assert( (localRow >= 0) );
    assert( (localRow < rows()) );
    assert( (localCol >= 0) );
    assert( (localCol < columns()) );

    matrix_.add(row_[ localRow ], col_[ localCol ],value);
  }

  //! get matrix entry
  DofType get( int localRow, int localCol ) const
  {
    assert( (localRow >= 0) && (localRow < rows()) );
    assert( (localCol >= 0) && (localCol < columns()) );

    return matrix_( row_[ localRow ], col_[ localCol ] );
  }

  //! set matrix entry to value
  void set( int localRow, int localCol, const DofType value )
  {
    assert( (localRow >= 0) && (localRow < rows()) );
    assert( (localCol >= 0) && (localCol < columns()) );

    matrix_.set( row_[ localRow ], col_[ localCol ], value );
  }

  //! set matrix row to zero except diagonla entry
  void unitRow( const int localRow )
  {
    assert( (localRow >= 0) && (localRow < rows()) );
    matrix_.unitRow( row_[ localRow ] );
  }

  //! set matrix row to zero
  void clearRow( const int localRow )
  {
    assert( (localRow >= 0) && (localRow < rows()) );
    matrix_.clearRow( row_[localRow]);
  }

};


template< class DomainSpace, class RangeSpace, class TraitsImp >
class EigenMatrixObject : public Dune::OEMMatrix
{
  typedef typename DomainSpace::EntityType  DomainEntityType ;
  typedef typename RangeSpace::EntityType   RangeEntityType ;

public:

  typedef TraitsImp Traits;

  typedef DomainSpace DomainSpaceType;
  typedef RangeSpace RangeSpaceType;

  typedef DSLC::EigenRowMajorSparseMatrix< typename DomainSpace::RangeFieldType > MatrixType;
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
  const DSLC::SparsityPatternDefault& sparsity_pattern_;

public:
  //! type of local matrix
  typedef LocalEigenMatrix< DomainSpace, RangeSpace, Traits> LocalMatrixType;

  inline EigenMatrixObject( const DomainSpaceType& domainSpace,
                            const RangeSpaceType& rangeSpace,
                            const DSLC::SparsityPatternDefault& sparsity_pattern )
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
    matrix_.backend() *= 0.0;
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
      // if empty grid do nothing (can appear in parallel runs)
      if( (domainSpace_.begin() != domainSpace_.end())
          && (rangeSpace_.begin() != rangeSpace_.end()) )
      {
        matrix_ = MatrixType(domainSpace_.size(), rangeSpace_.size(), sparsity_pattern_);
      }
      sequence_ = domainSpace_.sequence();
    }
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

//  using Base::apply;

  EigenMatrixOperator ( const DomainSpaceType &domainSpace,
                            const RangeSpaceType &rangeSpace,
                            const DSLC::SparsityPatternDefault& pattern )
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

} // namespace Fem
} // namespace Stuff
} // namespace Dune

#endif //HAVE_FEM && HAVE_EIGEN

#endif // DUNE_STUFF_MATRIX_OBJECT_HH
