/**
 *  \file   parametercontainer.hh
 *
 *  \brief  containing class ParameterContainer
 **/

#ifndef PARAMETERCONTAINER_HH_INCLUDED
#define PARAMETERCONTAINER_HH_INCLUDED

#include <dune/fem/io/parameter.hh>

#include "misc.hh"
#include "logging.hh"

#include <vector>

/**
 *  \brief  class containing global parameters
 *
 *  ParameterContainer contains all the needed global parameters getting them via Dune::Parameter
 *
 *  \todo   needs reorganizing!
 **/
class ParameterContainer
{
    public:
        /**
         *  \brief  destuctor
         *
         *  doing nothing
         **/
        ~ParameterContainer()
        {
        }

        /**
         *  \brief  prints all parameters
         *
         *  \todo   implement me
         *
         *  \param  out stream to print to
         **/
        void Print( std::ostream& out ) const
        {
            out << "\nthis is the ParameterContainer.Print() function" << std::endl;
        }

        /**
         *  \brief  checks command line parameters
         *
         *  \return true, if comman line arguments are valid
         **/
       bool ReadCommandLine( int argc, char** argv )
        {
            if ( argc == 2 )
            {
                parameter_filename_ = argv[1];
                Dune::Parameter::append( parameter_filename_ );
            }
            else
            {
                Dune::Parameter::append( argc, argv );
//                std::cerr << "\nUsage: " << argv[0] << " parameterfile" << std::endl;
//                PrintParameterSpecs( std::cerr );
//                std::cerr << std::endl;
            }
            return CheckSetup();

        }

        /** \brief checks for mandatory params
         *
         *  \return true, if all needed parameters exist
         **/
        bool CheckSetup()
        {
            typedef std::vector<std::string>::iterator
                Iterator;
            Iterator it = mandatory_params_.begin();
            Iterator new_end = std::remove_if( it, mandatory_params_.end(), Dune::Parameter::exists );
            all_set_up_ = ( new_end ==  it );
            for ( it; new_end != it; ++it ) {
            	std::cerr   << "\nError: " << parameter_filename_
                            << " is missing parameter: "
                            << *it << std::endl;
            }
            return all_set_up_;
        }

        /**
         *  \brief  prints, how a parameterfile schould look like
         *
         *  \param out stream to print
         **/
        void PrintParameterSpecs( std::ostream& out )
        {
            out << "\na valid parameterfile should at least specify the following parameters:\n"
                << "Remark: the correspondig files have to exist!\n"
                << "(copy this into your parameterfile)\n";
            std::vector<std::string>::const_iterator it = mandatory_params_.begin();
            for (; it != mandatory_params_.end(); ++it )
                std::cerr << *it << ": VALUE\n";
            std::cerr << std::endl;
        }

        std::string DgfFilename( unsigned int dim ) const {
            assert( dim > 0 && dim < 4 );
            assert( all_set_up_ );
            std::stringstream fn;
            fn << "dgf_file_" << dim << "d" ;
            return Dune::Parameter::getValue<std::string>( fn.str() );
        }


        /** \brief  passthrough to underlying Dune::Parameter
            \param  useDbgStream
                    needs to be set to false when using this function in Logging::Create,
                        otherwise an assertion will will cause streams aren't available yet
        **/
        template< typename T >
        T getParam( std::string name, T def, bool useDbgStream = true )
        {
            assert( all_set_up_ );
            #ifndef NDEBUG
                if ( ! Dune::Parameter::exists( name ) ) {
                    if ( useDbgStream )
                        Logger().Dbg() << "WARNING: using default value for parameter \"" << name << "\"" << std::endl;
                    else
                        std::cerr << "WARNING: using default value for parameter \"" << name << "\"" << std::endl;
                }
            #endif
            return Dune::Parameter::getValue( name, def );
        }

        //! passthrough to underlying Dune::Parameter
        template< typename T >
        void setParam( std::string name, T val )
        {
            assert( all_set_up_ );
            return Dune::Parameter::replaceKey( name, val );
        }

    private:
        bool all_set_up_;
        std::string parameter_filename_;
        std::vector<std::string> mandatory_params_;

        /**
         *  \brief  constuctor
         *
         *  \attention  call ReadCommandLine() to set up parameterContainer
         **/
        ParameterContainer()
            : all_set_up_( false )
        {
            const std::string p[] = { "dgf_file_2d", "dgf_file_3d" };
            mandatory_params_ = std::vector<std::string> ( p, p + ( sizeof(p) / sizeof( p[0] ) ) );
        }

        friend ParameterContainer& Parameters();
};

///global ParameterContainer instance
ParameterContainer& Parameters()
{
    static ParameterContainer parameters;
    return parameters;
}
#endif // end of PARAMETERHANDLER.HH
