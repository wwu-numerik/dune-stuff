#ifndef LOCALMATRIX_PROXY_HH
#define LOCALMATRIX_PROXY_HH

#include <vector>
#include <assert.h>
#include "debug.hh"
#include "math.hh"

namespace Stuff { namespace Matrix {

//! a small proxy object that automagically prevents near-0 value fill-in
template < class MatrixPointerType >
class LocalMatrixProxy {
    typedef typename MatrixPointerType::element_type
        MatrixObjectType;
    typedef typename MatrixObjectType::LocalMatrixType
        LocalMatrixType;
    typedef typename MatrixObjectType::DomainSpaceType::GridType
        GridType;
    typedef typename GridType::template Codim< 0 >::Entity
        EntityType;
    typedef typename MatrixObjectType::MatrixType::Ttype
        FieldType;
    LocalMatrixType local_matrix_;
    const double eps_;
    const unsigned int rows_;
    const unsigned int cols_;
    std::vector<FieldType> entries_;

public:
    LocalMatrixProxy( MatrixPointerType& object, const EntityType& self, const EntityType& neigh, const double eps )
        : local_matrix_( object->localMatrix(self,neigh) ),
          eps_( eps ),
          rows_( local_matrix_.rows() ),
          cols_( local_matrix_.columns() ),
          entries_( rows_ * cols_, FieldType( 0.0 ) )
    {}

    inline void add( const unsigned int row , const unsigned int col, const FieldType val )
    {
        ASSERT_LT( row , rows_ );
        ASSERT_LT( col , cols_ );
        entries_[row*cols_+col] += val;
    }

    ~LocalMatrixProxy()
    {
        for( unsigned int i = 0; i < rows_; ++i )
        {
            for( unsigned int j = 0; j < cols_; ++j )
            {
                const FieldType& i_j = entries_[i*cols_+j];
                if( !aboutEqual(i_j,0.0) )
                    local_matrix_.add( i, j , i_j );
            }
        }
    }
    unsigned int rows() const { return rows_; }
    unsigned int cols() const { return cols_; }
};

//! a small proxy object that automagically prevents near-0 value fill-in
template < class MatrixPointerType >
class ISTLLocalMatrixProxy {
    typedef typename MatrixPointerType::element_type
        MatrixObjectType;
    typedef typename MatrixObjectType::LocalMatrixType
        LocalMatrixType;
    typedef typename MatrixObjectType::MatrixType::block_type
        block_type;
    typedef typename GridType::template Codim< 0 >::Entity
        EntityType;
    typedef typename MatrixObjectType::MatrixType::Ttype
        FieldType;
    //			LocalMatrixType local_matrix_;
    MatrixPointerType matrix_pointer_;
    const EntityType& self_;
    const EntityType& neigh_;
    const double eps_;
    const unsigned int rows_;
    const unsigned int cols_;
    std::vector<FieldType> entries_;
    //! global row numbers
    std :: vector< int > rowMap_;
    //! global col numbers
    std :: vector< int > colMap_;

public:
    ISTLLocalMatrixProxy( MatrixPointerType& pointer, const EntityType& self, const EntityType& neigh, const double eps )
        : //local_matrix_( object->localMatrix(self,neigh) ),
          matrix_pointer_( pointer ),
          self_(self),
          neigh_(neigh),
          eps_( eps ),
          rows_( block_type::rows ),
          cols_( block_type::cols ),
          entries_( rows_ * cols_, FieldType( 0.0 ) )
    {
        const auto& domainSpace = matrix_pointer_->rowSpace();
        const auto& rangeSpace = matrix_pointer_->colSpace();
        rowMap_.resize( domainSpace.baseFunctionSet( self ).numBaseFunctions() );
        colMap_.resize( rangeSpace.baseFunctionSet( neigh ).numBaseFunctions() );

        const auto dmend = domainSpace.mapper().end( self );
        for( auto dmit = domainSpace.mapper().begin( self ); dmit != dmend; ++dmit )
        {
            //                      ASSERT_EQ( dmit.global() , domainSpace.mapToGlobal( self, dmit.local() ) );
            rowMap_[ dmit.local() ] = dmit.global();
        }
        const auto rmend = rangeSpace.mapper().end( neigh );
        for( auto rmit = rangeSpace.mapper().begin( neigh ); rmit != rmend; ++rmit )
        {
            //                      ASSERT_EQ( rmit.global() , rangeSpace.mapToGlobal( neigh, rmit.local() ) );
            colMap_[ rmit.local() ] = rmit.global();
        }
    }

    inline void add( const unsigned int row , const unsigned int col, const FieldType val )
    {
        ASSERT_LT( row, rows_ );
        ASSERT_LT( col, cols_ );
        assert( !std::isnan( matrix_pointer_->matrix()(rowMap_[ row ], colMap_[ col ]) ) );
        entries_[row*cols_+col] += val;
    }

    ~ISTLLocalMatrixProxy()
    {
        for( unsigned int i = 0; i < rows_; ++i )
        {
            for( unsigned int j = 0; j < cols_; ++j )
            {
                const FieldType& i_j = entries_[i*cols_+j];
                if( std::fabs(i_j) > eps_ )
                {
                    assert( !std::isnan( i_j ) );
                    assert( !std::isinf( i_j ) );
                    matrix_pointer_->add( rowMap_[ i ], colMap_[ j ], i_j );
                    assert( !std::isnan( matrix_pointer_->matrix()(rowMap_[ i ], colMap_[ j ]) ) );
                    assert( !std::isinf( matrix_pointer_->matrix()(rowMap_[ i ], colMap_[ j ]) ) );
                }
            }
        }
    }

    unsigned int rows() const { return rows_; }
    unsigned int cols() const { return cols_; }
};

} } // namespace Stuff { namespace Matrix {

#endif // LOCALMATRIX_PROXY_HH
