/**
 *  \file   parametercontainer.hh
 *
 *  \brief  containing class ParameterContainer
 **/

#ifndef PARAMETERCONTAINER_HH_INCLUDED
#define PARAMETERCONTAINER_HH_INCLUDED

#include <dune/stuff/deprecated.hh> //ensure DUNE_DEPRECATED is defined properly
#include <dune/fem/io/parameter.hh>

#include "logging.hh"
#include "filesystem.hh"

#include <vector>
#include <algorithm>
#include <fstream>

#include <boost/format.hpp>

namespace Stuff {
	//! a class usable as a default validator for Dune::Parameter
	template< class T >
	class ValidateAny
			: public Dune::ValidatorDefault< T, ValidateAny< T > >
	{
		typedef ValidateAny< T > ThisType;
		typedef Dune::ValidatorDefault< T, ThisType > BaseType;

	public:
		inline ValidateAny ( ){}
		inline ValidateAny ( const ThisType & ){}

		inline bool operator() ( const T& ) const
		{
			return true;
		}

		inline void print(std::ostream& s) const
		{
			s << "ValidateAny: all values should be valid... " << std::endl << std::endl;
		}
	};

	//! validates arg if in given list
	template< class T, class ListImp = std::vector<T> >
	class ValidateInList
			: public Dune::ValidatorDefault< T, ValidateInList< T, ListImp > >
	{
		typedef ValidateInList<T, ListImp> ThisType;
		typedef Dune::ValidatorDefault< T, ThisType > BaseType;
		typedef ListImp ListType;
		ListType valid_list_;
	public:
		ValidateInList(const ListType& valid_list)
			:valid_list_(valid_list)
		{}

		inline bool operator() ( const T& val ) const
		{
			return std::find( valid_list_.begin(), valid_list_.end(), val ) != valid_list_.end();
		}

		inline void print(std::ostream& s) const
		{
			s << "ValidateInList: checked Parameter was not in valid list" << std::endl << std::endl;
		}

	};
}

/**
 *  \brief  class containing global parameters
 *
 *  ParameterContainer contains all the needed global parameters getting them via Dune::Parameter
 *
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
			if ( argc == 2 ) {
                parameter_filename_ = argv[1];
                Dune::Parameter::append( parameter_filename_ );
            }
			else {
                Dune::Parameter::append( argc, argv );
            }
			const std::string datadir = Dune::Parameter::getValidValue( std::string("fem.io.datadir"),
																	   std::string("data"),
																	   Stuff::ValidateAny<std::string>()
																	  );
			Dune::Parameter::replaceKey( "fem.prefix", datadir );
#warning "replaceKey was hard commented in common"
			if ( !Dune::Parameter::exists( "fem.io.logdir" ) )
					Dune::Parameter::replaceKey( "fem.io.logdir", "log" );
			warning_output_ = Dune::Parameter::getValue( "disableParameterWarnings", warning_output_ );
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
            for ( ; new_end != it; ++it ) {
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
	    std::string retval = Dune::Parameter::getValue<std::string>( (boost::format("dgf_file_%dd") % dim).str() );
	    Dune::Parameter::append( (boost::format("fem.io.macroGridFile_%dd") % dim).str(), retval );
	    return retval;
        }


        /** \brief  passthrough to underlying Dune::Parameter
            \param  useDbgStream
                    needs to be set to false when using this function in Logging::Create,
                        otherwise an assertion will will cause streams aren't available yet
        **/
        template< typename T >
        T getParam( std::string name, T def, bool useDbgStream = true )
        {
			return getParam( name, def, Stuff::ValidateAny<T>(), useDbgStream );
        }

		template< typename T, class Validator >
		T getParam( std::string name, T def, const Validator& validator, bool UNUSED_UNLESS_DEBUG(useDbgStream) = true )
		{
			assert( all_set_up_ );
            assert( validator( def ) );
			#ifndef NDEBUG
				if ( warning_output_ && ! Dune::Parameter::exists( name ) ) {
					if ( useDbgStream )
						Logger().Dbg() << "WARNING: using default value for parameter \"" << name << "\"" << std::endl;
					else
						std::cerr << "WARNING: using default value for parameter \"" << name << "\"" << std::endl;
				}
			#endif
            try {
                return Dune::Parameter::getValidValue( name, def, validator );
            }
            catch (Dune::InconsistentDefaultValue& p) {
                std::cerr << boost::format( "Dune::Fem::Parameter reports inconsistent parameter: %s\n") % p.what();
            }
            return def;
		}

        std::map<char,std::string> getFunction( const std::string& name, const std::string def = "0" )
		{
            std::map<char,std::string> ret;
            ret['x'] = getParam( name+"_x", def );
            ret['y'] = getParam( name+"_y", def );
            ret['z'] = getParam( name+"_z", def );
			return ret;
		}

        //! passthrough to underlying Dune::Parameter
        template< typename T >
        void setParam( std::string name, T val )
        {
            assert( all_set_up_ );
            return Dune::Parameter::replaceKey( name, val );
        }

		//! extension to Fem::paramter that allows vector/list like paramteres from a single key
		template < class T >
		std::vector<T> getList( const std::string name, T def ) {
			if ( ! Dune::Parameter::exists( name ) ) {
				std::vector<T> ret;
				ret.push_back( def );
				return ret;
			}
			std::string tokenstring = getParam( name, std::string("dummy") );
			std::string delimiter = getParam( std::string("parameterlist_delimiter"), std::string(";"), false );
			Stuff::Tokenizer<T> tokens ( tokenstring, delimiter );
			return tokens.getTokens();
		}

    private:
        bool all_set_up_;
		bool warning_output_;
        std::string parameter_filename_;
        std::vector<std::string> mandatory_params_;

        /**
         *  \brief  constuctor
         *
         *  \attention  call ReadCommandLine() to set up parameterContainer
         **/
        ParameterContainer()
			: all_set_up_( false ),
			warning_output_( true )
        {
            const std::string p[] = { "dgf_file_2d", "dgf_file_3d" };
            mandatory_params_ = std::vector<std::string> ( p, p + ( sizeof(p) / sizeof( p[0] ) ) );
        }

        friend ParameterContainer& Parameters();
};

//! global ParameterContainer instance
ParameterContainer& Parameters()
{
    static ParameterContainer parameters;
    return parameters;
}

namespace Stuff {
	//! get a path in datadir with existence guarantee (cannot be in filessytem.hh -- cyclic dep )
	std::string getFileinDatadir( const std::string& fn )
	{
		boost::filesystem::path path( Parameters().getParam("fem.io.datadir", std::string(".") ) );
		path /= fn;
		boost::filesystem::create_directories( path.parent_path() );
		return path.string();
	}
}
#endif // end of PARAMETERHANDLER.HH
