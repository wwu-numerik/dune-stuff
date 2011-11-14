#ifndef STUFF_FUNCTIONS_HH_INCLUDED
#define STUFF_FUNCTIONS_HH_INCLUDED

#include <cmath>
#include <fstream>
#include <limits>

#include <dune/stuff/timefunction.hh>
#include <dune/grid/io/file/dgfparser/dgfparser.hh> //for GridPtr
#include <dune/fem/io/file/vtkio.hh>
#include <dune/fem/operator/1order/localmassmatrix.hh>
#include "misc.hh"

namespace Stuff{

//! (inplace) multiplies given function with matrix diagonal
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
 *  \attention  works only for constant base function = sqrt(2) atm
 **/
template < class FunctionType >
void getMinMaxOfDiscreteFunction(   const FunctionType& function,
                                    double& min,
                                    double& max )
{
    // preparations
	min = std::numeric_limits<double>::max();
	max = std::numeric_limits<double>::min();
    typedef typename FunctionType::ConstDofIteratorType
        ConstDofIteratorType;
    ConstDofIteratorType  itEnd = function.dend();
    // find minimum and maximum
    for ( ConstDofIteratorType it = function.dbegin(); it != itEnd; ++it ) {
        min = *it < min ? *it : min;
        max = *it > max ? *it : max;
    }
}

//! count dofs of f1,f2 with abs(f1[i] - f2[i]) > tolerance
template < class FunctionType >
unsigned int getNumDiffDofs(    const FunctionType& f1,
                        const FunctionType& f2,
                        const double tolerance )
{
	assert( f1.size() == f2.size() ); //should be implicit cause they're of the same type...
    unsigned int numDiffs = 0;
    typedef typename FunctionType::ConstDofIteratorType
        ConstDofIteratorType;
    ConstDofIteratorType  itEnd = f1.dend();
    ConstDofIteratorType  f2it  = f2.dbegin();
    for ( ConstDofIteratorType f1it = f1.dbegin(); f1it != itEnd; ++f1it, ++f2it ) {
        numDiffs += ( std::fabs( *f1it - *f2it ) > tolerance );
    }
    return numDiffs ;
}

/**
  * \brief shift each dof by scalar
  *
  * this is the actual functions value, no multiplication with basefunctions afterwards
  **/
template < class Function >
void addScalarToFunc( Function& f, double sc )
{
    typedef typename Function::DofIteratorType DofIteratorType;
    DofIteratorType it = f.dbegin();
    for ( ; it != f.dend(); ++it )
        *it += sc;
    return;
}

//! returns arithmetic mean of function's dofs
template < class Function >
typename Function::FieldType getFuncAvg( const Function& f )
{
    typedef typename Function::ConstDofIteratorType DofIteratorType;
    DofIteratorType it = f.dbegin();
	const unsigned int numdofs = f.size();
	typename Function::FieldType sum = 0;
    for ( ; it != f.dend(); ++it )
        sum += *it;
    sum /= double(numdofs);
    return sum;
}

//! inverts dof order
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

/** \todo FELIX needs to doc me **/
template <  class DiscreteFunctionType,
            class ErrorStream >
int saveDiscreteFunction(   const DiscreteFunctionType& discreteFunction,
                            const std::string gridType,
                            const int refineLevel,
                            const std::string dgfFilename,
                            const std::string saveToFilenamePrefix,
                            ErrorStream& error )
{
    int errorState( 0 );

    error.Flush();

    if ( !( discreteFunction.write_ascii( saveToFilenamePrefix + std::string( ".ddf" ) ) ) ) {
        ++errorState;
        error << "Error in saveDiscreteFunction(): could not write to " << saveToFilenamePrefix + std::string( ".ddf" ) << "!" << std::endl;
        return errorState;
    }


    // save dgf file with information
    std::ifstream readFile( dgfFilename.c_str() );
    if ( readFile.is_open() ) {
        std::ofstream writeFile( std::string( saveToFilenamePrefix + std::string( ".dgf" ) ).c_str() );
        if ( writeFile.is_open() ) {
            // shift until "DGF"
            while( !( readFile.eof() ) ) {
                std::string line( "" );
                std::getline( readFile, line );
                if ( line.size() ) {
                    // remove lines with redundant information == "# ddf_"
                    if( !( ( line[0] == '#' ) && ( line[1] == ' ' ) && ( line[2] == 'd' ) && ( line[3] == 'd' ) && ( line[4] == 'f' ) && ( line[5] == '_' ) ) ) {
                        writeFile << line << std::endl;
                    }
                    if( ( line[0] == 'D' ) && ( line[1] == 'G' ) && ( line[2] == 'F' ) ) {
                        break;
                    }
                }
            }
            // insert informations
            writeFile << "# ddf_gridtype: " << gridType << std::endl;
            writeFile << "# ddf_refine_level: " << refineLevel << std::endl;
            // rest of dgf file
            while( !( readFile.eof() ) ) {
                std::string line( "" );
                std::getline( readFile, line );
                if ( line.size() ) {
                    // remove lines with redundant information == "# ddf_"
                    if( !( ( line[0] == '#' ) && ( line[1] == ' ' ) && ( line[2] == 'd' ) && ( line[3] == 'd' ) && ( line[4] == 'f' ) && ( line[5] == '_' ) ) ) {
                        writeFile << line << std::endl;
                    }
                }
            }
            writeFile.flush();
            writeFile.close();
            readFile.close();
        }
        else {
            ++errorState;
            error << "Error in saveDiscreteFunction(): could not write to " << saveToFilenamePrefix + std::string( ".dgf" ) << "!" << std::endl;
            return errorState;
        }
    }
    else {
        ++errorState;
        error << "Error in saveDiscreteFunction(): could not open " << dgfFilename << "!" << std::endl;
        return errorState;
    }
    return errorState;
}

/** \todo FELIX needs to doc me **/
template <  class DiscreteFunctionType,
            class ErrorStream >
int loadDiscreteFunction(   const std::string loadFromfilenamePrefix,
                            ErrorStream& error )
{
    int errorState( 0 );

    error.Flush();

    std::string gridType( "" );
    int refineLevel( 0 );

    bool gridTypeRead( false );
    bool refineLevelRead( false );

    std::ifstream readFile( std::string( loadFromfilenamePrefix + std::string( ".dgf" ) ).c_str() );
    if ( readFile.is_open() ) {
        while( !( readFile.eof() ) ) {
            if ( !( gridTypeRead && refineLevelRead ) ) {
                std::string line( "" );
                std::getline( readFile, line );
                if ( line.size() ) {
                    if ( line.substr( 0, 6 ) == "# ddf_" ) {
                        unsigned int key_start = 6;
                        unsigned int key_end = key_start;
                        for ( ; key_end < line.size(); ++key_end ) {
                            const char &c = line[key_end];
                            if( (c == ' ') || (c == '\t') || (c == ':') ) {
                                break;
                            }
                        }
                        std::string key = line.substr( key_start, key_end - key_start );
                        unsigned int value_start = key_end;
                        for( ; value_start < line.size() ; ++value_start ) {
                            if( line[value_start] == ':' ) {
                                break;
                            }
                        }
                        ++value_start;
                        for( ; value_start < line.size(); ++value_start ) {
                            if( ( line[value_start] != ' ' ) && ( line[value_start] != '\t' ) ) {
                                break;
                            }
                        }
                        if( value_start >= line.size() ) {
                            ++errorState;
                        }
                        std::string value = line.substr( value_start, line.size() - value_start );
                        if ( key == "gridtype" ) {
                            gridType = value;
                            gridTypeRead = true;
                        }
                        else if ( key == "refine_level" ) {
                            refineLevel = Stuff::fromString< int >( value );
                            refineLevelRead = true;
                        }
                    }
                }
            }
            else {
                break;
            }
        }
    }
    else {
        ++errorState;
        error << "Error: could not open " << std::string( loadFromfilenamePrefix + std::string( ".dgf" ) ) << "!" << std::endl;
        return errorState;
    }

    if ( gridTypeRead && refineLevelRead ) {

        typedef typename DiscreteFunctionType::FunctionSpaceType
            DiscreteFunctionSpaceType;

        typedef typename DiscreteFunctionSpaceType::GridPartType
            GridPartType;

        typedef typename GridPartType::GridType
            GridType;

        typedef Dune::GridPtr< GridType >
            GridPointerType;

        GridPointerType gridPointer( std::string( loadFromfilenamePrefix + std::string( ".dgf" ) ) );
        gridPointer->globalRefine( refineLevel );
        GridPartType gridPart( *gridPointer );

        DiscreteFunctionSpaceType discreteFunctionSpace( gridPart );

        DiscreteFunctionType discreteFunction( loadFromfilenamePrefix, discreteFunctionSpace );

        const bool readFromAscii = discreteFunction.read_ascii( std::string( loadFromfilenamePrefix + std::string( ".ddf" ) ) );

        if ( readFromAscii ) {
            Dune::VTKIO< GridPartType > vtkWriter( gridPart );
            const std::string vtkWriterFilename = std::string( "data/saved_" ) + loadFromfilenamePrefix;
            vtkWriter.addVectorVertexData( discreteFunction );
            vtkWriter.write( vtkWriterFilename.c_str() );
            vtkWriter.clear();
        }
        else {
            ++errorState;
            error << "Error: could not read from " << std::string( loadFromfilenamePrefix + std::string( ".ddf" ) ) << "!" << std::endl;
            return errorState;
        }
    }
    else {
        ++errorState;
        error << "Error: one of the following lines is missing in " << std::string( loadFromfilenamePrefix + std::string( ".dgf" ) ) << "!" << std::endl;
        error << "\t# ddf_gridtype: GRIDTYPE" << std::endl;
        error << "\t# ddf_refine_level: REFINELEVEL" << std::endl;
        return errorState;
    }
    return errorState;
}

/** \todo FELIX needs to doc me **/
int readRefineLevelFromDGF(  const std::string filename )
{
    std::string gridType( "" );
    int refineLevel( 0 );

    bool gridTypeRead( false );
    bool refineLevelRead( false );

    std::ifstream readFile( filename.c_str() );
    if ( readFile.is_open() ) {
        while( !( readFile.eof() ) ) {
            if ( !( gridTypeRead && refineLevelRead ) ) {
                std::string line( "" );
                std::getline( readFile, line );
                if ( line.size() ) {
                    if ( line.substr( 0, 6 ) == "# ddf_" ) {
                        unsigned int key_start = 6;
                        unsigned int key_end = key_start;
                        for ( ; key_end < line.size(); ++key_end ) {
                            const char &c = line[key_end];
                            if( (c == ' ') || (c == '\t') || (c == ':') ) {
                                break;
                            }
                        }
                        std::string key = line.substr( key_start, key_end - key_start );
                        unsigned int value_start = key_end;
                        for( ; value_start < line.size() ; ++value_start ) {
                            if( line[value_start] == ':' ) {
                                break;
                            }
                        }
                        ++value_start;
                        for( ; value_start < line.size(); ++value_start ) {
                            if( ( line[value_start] != ' ' ) && ( line[value_start] != '\t' ) ) {
                                break;
                            }
                        }
                        if( value_start >= line.size() ) {
                            --refineLevel;
                        }
                        std::string value = line.substr( value_start, line.size() - value_start );
                        if ( key == "gridtype" ) {
                            gridType = value;
                            gridTypeRead = true;
                        }
                        else if ( key == "refine_level" ) {
                            refineLevel = Stuff::fromString< int >( value );
                            refineLevelRead = true;
                            return refineLevel;
                        }
                    }
                }
            }
            else {
                break;
            }
        }
    }
    else {
        --refineLevel;
        std::cerr << "Error: could not open " << filename << "!" << std::endl;
        return refineLevel;
    }
    return refineLevel;
}

/** \todo FELIX needs to doc me **/
std::string readGridTypeFromDGF(  const std::string filename )
{
    std::string gridType( "no_gridtype_found_in " + filename );
    int refineLevel( 0 );

    bool gridTypeRead( false );
    bool refineLevelRead( false );

    std::ifstream readFile( filename.c_str() );
    if ( readFile.is_open() ) {
        while( !( readFile.eof() ) ) {
            if ( !( gridTypeRead && refineLevelRead ) ) {
                std::string line( "" );
                std::getline( readFile, line );
                if ( line.size() ) {
                    if ( line.substr( 0, 6 ) == "# ddf_" ) {
                        unsigned int key_start = 6;
                        unsigned int key_end = key_start;
                        for ( ; key_end < line.size(); ++key_end ) {
                            const char &c = line[key_end];
                            if( (c == ' ') || (c == '\t') || (c == ':') ) {
                                break;
                            }
                        }
                        std::string key = line.substr( key_start, key_end - key_start );
                        unsigned int value_start = key_end;
                        for( ; value_start < line.size() ; ++value_start ) {
                            if( line[value_start] == ':' ) {
                                break;
                            }
                        }
                        ++value_start;
                        for( ; value_start < line.size(); ++value_start ) {
                            if( ( line[value_start] != ' ' ) && ( line[value_start] != '\t' ) ) {
                                break;
                            }
                        }
                        if( value_start >= line.size() ) {
                            return gridType;
                        }
                        std::string value = line.substr( value_start, line.size() - value_start );
                        if ( key == "gridtype" ) {
                            gridType = value;
                            gridTypeRead = true;
                        }
                        else if ( key == "refine_level" ) {
                            refineLevel = Stuff::fromString< int >( value );
                            refineLevelRead = true;
                        }
                    }
                }
            }
            else {
                break;
            }
        }
    }
    else {
        std::cerr << "Error: could not open " << filename << "!" << std::endl;
        return gridType;
    }
    return gridType;
}

/** \todo RENE needs to doc me **/
template < class FunctionType, class DiscreteFunctionSpaceType  >
std::pair< typename FunctionType::RangeType, double > integralAndVolume( const FunctionType& function, const DiscreteFunctionSpaceType& space, const int polOrd = -1 )
{
	typename FunctionType::RangeType integral_value = typename FunctionType::RangeType(0);
	double total_volume =0;
	typedef typename DiscreteFunctionSpaceType::Traits::GridPartType GridPartType;
	typedef typename DiscreteFunctionSpaceType::Traits::IteratorType Iterator;
	typedef typename DiscreteFunctionSpaceType::BaseFunctionSetType BaseFunctionSetType ;
	typedef typename GridPartType::GridType GridType;

//	typedef typename FunctionType::LocalFunctionType LocalFType;

	typename DiscreteFunctionSpaceType::RangeType ret (0.0);

	// type of quadrature
	typedef Dune::CachingQuadrature<GridPartType,0> QuadratureType;

	typedef Dune::LocalDGMassMatrix< DiscreteFunctionSpaceType, QuadratureType > LocalMassMatrixType;

	const int quadOrd = (polOrd == -1) ? (2 * space.order()) : polOrd;

	// create local mass matrix object
//	LocalMassMatrixType massMatrix( space, quadOrd );

	// check whether geometry mappings are affine or not
//	const bool affineMapping = massMatrix.affine();

	const Iterator endit = space.end();
	for(Iterator it = space.begin(); it != endit ; ++it)
	{
	  // get entity
	  const typename GridType::template Codim<0>::Entity& en = *it;
	  // get geometry
	  const typename GridType::template Codim<0>::Geometry& geo = en.geometry();
	  total_volume += geo.volume();

	  // get quadrature
	  QuadratureType quad(en, quadOrd);

	  // get local function of argument
//	  const LocalFType local_function = function.localFunction(en);

	  const int quadNop = quad.nop();

	  for(int qP = 0; qP < quadNop ; ++qP)
	  {
		const double intel = quad.weight(qP)
				* geo.integrationElement( quad.point(qP) ); // general case
		// evaluate function
		typename DiscreteFunctionSpaceType::RangeType
			dummy;
		typename DiscreteFunctionSpaceType::DomainType
			xWorld = geo.global( quad.point(qP) );
		function.evaluate(xWorld, dummy);
		ret =dummy;
		ret *= intel;

		integral_value +=  ret ;
	  }

	}
	return std::make_pair( integral_value, total_volume);
}

/** \todo RENE needs to doc me **/
template < class FunctionType, class DiscreteFunctionSpaceType  >
typename FunctionType::RangeType meanValue( const FunctionType& function, const DiscreteFunctionSpaceType& space, const int polOrd = -1 )
{
	std::pair< typename FunctionType::RangeType , double > pair = Stuff::integralAndVolume( function, space, polOrd );
	pair.first /= pair.second;
	return pair.first ;
}

/** \todo RENE needs to doc me **/
template < class FunctionType, class DiscreteFunctionSpaceType  >
double boundaryIntegral( const FunctionType& function, const DiscreteFunctionSpaceType& space, const int polOrd = -1 )
{
	double integral_value = 0;
	double total_volume =0;
	typedef typename DiscreteFunctionSpaceType::Traits::GridPartType GridPartType;
	typedef typename DiscreteFunctionSpaceType::Traits::IteratorType Iterator;
	typedef typename DiscreteFunctionSpaceType::BaseFunctionSetType BaseFunctionSetType ;
	typedef typename GridPartType::GridType GridType;

//	typedef typename FunctionType::LocalFunctionType LocalFType;

	typename DiscreteFunctionSpaceType::RangeType ret (0.0);

	// type of quadrature
	typedef Dune::CachingQuadrature<GridPartType,1> QuadratureType;

	typedef Dune::LocalDGMassMatrix< DiscreteFunctionSpaceType, Dune::CachingQuadrature<GridPartType,0> > LocalMassMatrixType;

	const int quadOrd = (polOrd == -1) ? (2 * space.order()) : polOrd;

	// create local mass matrix object
	LocalMassMatrixType massMatrix( space, quadOrd );

	// check whether geometry mappings are affine or not
	const bool affineMapping = massMatrix.affine();

	const Iterator endit = space.end();
	for(Iterator it = space.begin(); it != endit ; ++it)
	{
		// get entity
		const typename GridType::template Codim<0>::Entity& entity = *it;
		// get geometry
		const typename GridType::template Codim<0>::Geometry& geo = entity.geometry();
		total_volume += geo.volume();

		typename GridPartType::IntersectionIteratorType intItEnd = space.gridPart().iend( entity );
		for (   typename GridPartType::IntersectionIteratorType intIt = space.gridPart().ibegin( entity );
			  intIt != intItEnd;
			  ++intIt )
		{
			if ( !intIt->neighbor() && intIt->boundary() )
			{
				// get quadrature
				const QuadratureType quad( space.gridPart(),
											*intIt,
											quadOrd,
											QuadratureType::INSIDE );
				const int quadNop = quad.nop();
				for(int qP = 0; qP < quadNop ; ++qP)
				{
					const double intel = (affineMapping) ?
						quad.weight(qP) : // affine case
						quad.weight(qP) * geo.integrationElement( quad.point(qP) ); // general case

					// evaluate function
					typename DiscreteFunctionSpaceType::RangeType
						dummy;
					typename DiscreteFunctionSpaceType::DomainType
						xWorld = geo.global( quad.point(qP) );
					function.evaluate(xWorld, dummy, *intIt);
					ret = dummy;
					ret *= intel;

					integral_value += ret * intIt->unitOuterNormal( quad.localPoint(qP) ) ;
				}
			}
		}
	}
	return integral_value;
}

/** \todo RENE needs to doc me **/
template <class DiscreteFunctionType>
void invertFunctionDofs( DiscreteFunctionType& function )
{
	typedef typename DiscreteFunctionType::DofIteratorType
		DofIteratorType;

    DofIteratorType it = function.dbegin();
    for( ; it != function.dend(); ++it )
    {
	    if (*it != 0.0 )
			*it = 1/(*it);
    }
    return;
}

//! return true if any dof is nan or inf
template <class DiscreteFunctionType>
bool FunctionContainsNanOrInf( const DiscreteFunctionType& function )
{
	typedef typename DiscreteFunctionType::ConstDofIteratorType
		DofIteratorType;

	DofIteratorType it = function.dbegin();
	for( ; it != function.dend(); ++it )
	{
		if ( std::isnan(*it) || std::isinf(*it) )
			return true;
	}
	return false;
}

bool FunctionContainsNanOrInf( const double* function, size_t size )
{
	for( size_t i = 0; i < size; ++i )
	{
		if ( std::isnan(function[i]) || std::isinf(function[i]) )
			return true;
	}
	return false;
}

//! return true if any dof is nan or inf
template <class DiscreteFunctionType>
void divideFunctionDofs( DiscreteFunctionType& target, const DiscreteFunctionType& divider )
{
	typedef typename DiscreteFunctionType::ConstDofIteratorType
		CDofIteratorType;
	typedef typename DiscreteFunctionType::DofIteratorType
		DofIteratorType;

	DofIteratorType it = target.dbegin();
	CDofIteratorType cit = divider.dbegin();
	for( ; it != target.dend(); ++it,++cit )
	{
		*it /= *cit;
	}
}

//! return true if any entry is nan or inf
template < class MatrixType >
bool MatrixContainsNanOrInf( const MatrixType& matrix )
{
	for(int row=0; row < matrix.size(0); ++row) {
		for(int col=0; col < matrix.size(1); ++col) {
			if ( std::isnan( matrix(row,col) ) || std::isinf( matrix(row,col) ) )
				return true;
		}
	}
	return false;
}

template < class FunctionSpaceImp >
class ConstantFunction : public Dune::Fem::Function < FunctionSpaceImp , ConstantFunction< FunctionSpaceImp > >
{
	  public:
		  typedef ConstantFunction< FunctionSpaceImp >
			  ThisType;
		  typedef Dune::Fem::Function < FunctionSpaceImp ,ThisType >
			  BaseType;
		  typedef typename BaseType::DomainType
			  DomainType;
		  typedef typename BaseType::RangeType
			  RangeType;

		  ConstantFunction( const FunctionSpaceImp& space,
						   const double constant = 0.0 )
			  : BaseType ( space ),
				constant_(  constant )
		  {}

		  ~ConstantFunction()
		  {}

		  inline void evaluate( const double /*time*/, const DomainType& /*arg*/, RangeType& ret ) const { ret = RangeType( constant_ ); }
		  inline void evaluate( const DomainType& /*arg*/, RangeType& ret ) const { ret = RangeType( constant_ ); }
		  template < class IntersectionIteratorType >
		  inline void evaluate( const double /*time*/, const DomainType& /*arg*/,
							   RangeType& ret, const IntersectionIteratorType /*it*/) const
		  { ret = RangeType( constant_ ); }
	private:
		  const double constant_;
};

template < class FunctionSpaceImp, class TimeProviderImp >
class ConstantFunctionTP : public Dune::TimeFunction < FunctionSpaceImp , ConstantFunctionTP< FunctionSpaceImp,TimeProviderImp >, TimeProviderImp >
{
	public:
		typedef ConstantFunctionTP< FunctionSpaceImp, TimeProviderImp >
			ThisType;
		typedef Dune::TimeFunction< FunctionSpaceImp, ThisType, TimeProviderImp >
			BaseType;
		typedef typename BaseType::DomainType
			DomainType;
		typedef typename BaseType::RangeType
			RangeType;

		ConstantFunctionTP(	const TimeProviderImp& timeprovider,
					const FunctionSpaceImp& space,
					const double constant = 0.0 )
			: BaseType( timeprovider, space ),
			  constant_(  constant )
		{}

		~ConstantFunctionTP()
		{}

		void evaluateTime( const double /*time*/, const DomainType& /*arg*/, RangeType& ret ) const { ret = RangeType( constant_ ); }
private:
	  const double constant_;
};

template < class FunctionSpaceImp, class TimeProviderImp >
class ConstantIntersectionTimeFunction : public Dune::IntersectionTimeFunction < FunctionSpaceImp , ConstantIntersectionTimeFunction< FunctionSpaceImp,TimeProviderImp >, TimeProviderImp >
{
public:
	typedef ConstantIntersectionTimeFunction< FunctionSpaceImp, TimeProviderImp >
		ThisType;
	typedef Dune::IntersectionTimeFunction< FunctionSpaceImp, ThisType, TimeProviderImp >
		BaseType;
	typedef typename BaseType::DomainType
		DomainType;
	typedef typename BaseType::RangeType
		RangeType;

	/**
	  *  \brief  constructor
	  *  \param  viscosity,alpha   dummies
	  **/
	ConstantIntersectionTimeFunction( const TimeProviderImp& timeprovider,
				   const FunctionSpaceImp& space,
				   const double constant = 0.0 )
		: BaseType ( timeprovider, space ),
		constant_(  constant )
	{}

	/**
   *  \brief  destructor
   *
   *  doing nothing
   **/
	~ConstantIntersectionTimeFunction()
	{}

	template < class IntersectionType >
	void evaluateTime( const double /*time*/, const DomainType& /*arg*/, RangeType& ret, const IntersectionType& /*intersection */) const
	{
		ret = RangeType(0.0);
	}
private:
	  const double constant_;
};

}//end namespace Stuff

#define NULLFUNCTION_TP(classname)\
template < class T, class P > struct classname : public Stuff::ConstantFunctionTP< T,P >\
{classname( const P& p,const T& t, double=0.0, double=0.0 ):Stuff::ConstantFunctionTP< T,P >(p,t){}};

#define NULLFUNCTION_TP_BOUNDARY(classname)\
template < class T, class P > struct classname : public Stuff::ConstantIntersectionTimeFunction< T,P >\
{classname( const P& p,const T& t, double=0.0, double=0.0 ):Stuff::ConstantIntersectionTimeFunction< T,P >(p,t){}};

#define NULLFUNCTION(classname)\
template < class T > struct classname : public Stuff::ConstantFunction< T >\
{classname( const double d, const T& t, double=0.0, double=0.0 ):Stuff::ConstantFunction< T >(t){}\
classname( const T& t ):Stuff::ConstantFunction< T >(t){}};

#endif //includeguard
