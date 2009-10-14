#ifndef STUFF_FUNCTIONS_HH_INCLUDED
#define STUFF_FUNCTIONS_HH_INCLUDED

#include <cmath>
#include <fstream>

#include <dune/grid/io/file/dgfparser/dgfparser.hh> //for GridPtr
#include <dune/fem/io/file/vtkio.hh>

#include "misc.hh"

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


}//end namespace

#endif //includeguard
