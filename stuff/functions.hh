#ifndef STUFF_FUNCTIONS_HH_INCLUDED
#define STUFF_FUNCTIONS_HH_INCLUDED

#include <cmath>

namespace Stuff{

template < class Matrix, class Function >
void DiagonalMult( const Matrix& matrix, Function& f )
{
    Function diag( "temp", f.space() );
    matrix.getDiag( diag );

    typedef typename Function::DofIteratorType DofIteratorType;
    DofIteratorType diag_it = diag.dbegin();
    DofIteratorType f_it = f.dbegin();

    for(int row=0; row< matrix.size(0); row++)
    {
        (*f_it) *= (*diag_it);
        ++f_it;
        ++diag_it;
    }
    return;
}


/**
 *  \brief  gets min and max of a Dune::DiscreteFunction
 *
 *          or compatible in terms of iterators
 *  \todo   doc
 *  \attention  works only for constant base function = sqrt(2) atm
 **/
template < class FunctionType >
void getMinMaxOfDiscreteFunction(   const FunctionType& function,
                                    double& min,
                                    double& max )
{
    // preparations
    min = 0.0;
    max = 0.0;
    typedef typename FunctionType::ConstDofIteratorType
        ConstDofIteratorType;
    ConstDofIteratorType  itEnd = function.dend();
    // find minimum and maximum
    for ( ConstDofIteratorType it = function.dbegin(); it != itEnd; ++it ) {
        min = *it < min ? *it : min;
        max = *it > max ? *it : max;
    }
}

template < class FunctionType >
unsigned int getNumDiffDofs(    const FunctionType& f1,
                        const FunctionType& f2,
                        const double tolerance )
{
    assert( f1.size() == f2.size() ); //should be implicit cause their of same type...
    unsigned int numDiffs = 0;
    typedef typename FunctionType::ConstDofIteratorType
        ConstDofIteratorType;
    ConstDofIteratorType  itEnd = f1.dend();
    ConstDofIteratorType  f2it  = f2.dbegin();
    // find minimum and maximum
    for ( ConstDofIteratorType f1it = f1.dbegin(); f1it != itEnd; ++f1it, ++f2it ) {
        numDiffs += ( std::fabs( *f1it - *f2it ) > tolerance );
    }
    return numDiffs ;
}

template < class Function >
void addScalarToFunc( Function& f, double sc )
{
    typedef typename Function::DofIteratorType DofIteratorType;
    DofIteratorType it = f.dbegin();
    for ( ; it != f.dend(); ++it )
        *it += sc;
    return;
}

template < class Function >
double getFuncAvg( const Function& f )
{
    typedef typename Function::ConstDofIteratorType DofIteratorType;
    DofIteratorType it = f.dbegin();
    const unsigned int numdofs = f.size();
    double sum = 0;
    for ( ; it != f.dend(); ++it )
        sum += *it;
    sum /= double(numdofs);
    return sum;
}

template < class Function >
void switchDofs( Function& f )
{
    typedef typename Function::DofIteratorType DofIteratorType;
    DofIteratorType front = f.dbegin();
    DofIteratorType back = f.dend();
    const unsigned int numdofs = f.size();
    for ( unsigned int i = 0; i < numdofs / 2; ++i ) {
        double tmp = *back;
        *back = *front;
        *front = tmp;
    }
    return;
}

}//end namespace

#endif //includeguard
