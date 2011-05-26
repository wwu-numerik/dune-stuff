#ifndef DUNE_STUFF_TEX_HH
#define DUNE_STUFF_TEX_HH

#include "misc.hh"
#include "runinfo.hh"
#include "grid.hh"
#include "static_assert.hh"

#include <ostream>
#include <sstream>
#include <vector>
#include <boost/format.hpp>

namespace Stuff {
namespace Tex {
//! interface and base class for all out eoc tex output
template < class Info >
class TexOutputBase
{
	protected:
		typedef std::vector< std::string >
			Strings;

		typedef TexOutputBase<Info>
			BaseType;

		Info info_;
		double current_h_;
		Strings headers_;


	public:
		TexOutputBase( const Info& info, Strings& headers )
			: info_(info),
			current_h_(1.0),
			headers_(headers)
		{}

		TexOutputBase( Strings& headers )
			: info_(Info()),
			current_h_(1.0),
			headers_(headers)
		{}

		virtual ~TexOutputBase(){}

		void setInfo( const Info& info )
		{
			info_ = info;
		}

		void putLineEnd( std::ofstream& outputFile_ )
		{
			outputFile_ << "\n"
				<< "\\tabularnewline\n"
				  << "\\hline \n";
			outputFile_.flush();
		}

		virtual void putErrorCol( std::ofstream& outputFile_, const double prevError_, const double error_, const double prevh_,  const bool /*initial*/  ) = 0;

		virtual void putHeader( std::ofstream& outputFile_ ) = 0;

		virtual void putStaticCols( std::ofstream& outputFile_ ) = 0;

		void endTable( std::ofstream& outputFile_ )
		{
			outputFile_ << "\\end{longtable}";
			outputFile_ << info_.extra_info;
			outputFile_.flush();
		}

		double get_h ()
		{
			return current_h_;
		}
};

//! format RunInfo and error vetor into a latex table, performs actual eoc calculation
class EocOutput : public TexOutputBase<RunInfo>
{
	typedef TexOutputBase<RunInfo>
		BaseType;

	public:
		EocOutput( const RunInfo& info, BaseType::Strings& headers )
			: BaseType( info, headers )
		{}

		EocOutput( BaseType::Strings& headers )
			: BaseType( RunInfo(), headers )
		{}

		//! eoc calc happens here
		void putErrorCol( std::ofstream& outputFile_, const double prevError_, const double error_, const double prevh_,  const bool /*initial*/  )
		{
			current_h_ = info_.grid_width;
			double factor = current_h_/prevh_;
			double eoc = std::log(error_/prevError_)/std::log(factor);
			if ( isnan( eoc ) )
				outputFile_ << " & " << std::scientific << error_ << std::fixed << " & " << "--";
			else
				outputFile_ << " & " << std::scientific << error_ << std::fixed << " & " << eoc;
		}

		void putHeader( std::ofstream& outputFile_ )
		{
			const unsigned int dynColSize = 2;
			const unsigned int statColSize = headers_.size() - 2;
			outputFile_ << "\\begin{longtable}{";

			for (unsigned int i=0;i<statColSize;i++) {
				if ( i == 2 )
					outputFile_ << "|r|";//runtime col
				else
					outputFile_ << "|c|";
			}

			for (unsigned int i=0;i<dynColSize;i++) {
				outputFile_ << "|cc|";
			}
			outputFile_ << "}\n"
				<< "\\caption{"
				<< info_.problemIdentifier << ": "
				<< info_.gridname
				<< ( info_.bfg ? std::string(", BFG ($\\tau = ")+ toString( info_.bfg_tau ) + std::string("$ ),"): std::string(", kein BFG,") )
				<< "\\\\"
				<< " Polorder (u,p,$ \\sigma $): (" << info_.polorder_velocity << ", "<< info_.polorder_pressure << ", "<< info_.polorder_sigma << " ) "
				<< " Loeser Genauigkeit: " << info_.solver_accuracy
				<< ", $\\alpha = " << info_.alpha
				<< "$, $\\mu = " << info_.viscosity
				<< "$}\\\\  \n"
				<< "\\hline \n";

			for (unsigned int i=0;i<statColSize;i++) {
				outputFile_ << headers_[i];
				if ( i <  statColSize - 1 )
					outputFile_ << " & ";
			}
			for (unsigned int i=0;i<dynColSize;i++) {
				outputFile_ << " & " << headers_[i+statColSize]
					<< " & EOC ";
			}
			outputFile_ << "\n \\endhead\n"
						<< "\\hline\n"
						<< "\\hline\n";
		}

		virtual void putStaticCols( std::ofstream& outputFile_ )
		{
			std::stringstream runtime;
			if ( info_.run_time > 59 )
				runtime << long(info_.run_time) / 60 << ":" << long(info_.run_time) % 60 ;
			else
				runtime << long(info_.run_time) ;

			outputFile_ << std::setw( 4 )
				<< info_.grid_width << " & "
				<< info_.codim0 << " & "
				<< runtime.str() << " & "
				<< info_.c11.first << " / " << info_.c11.second << " & "
				<< info_.c12.first << " / " << info_.c12.second << " & "
				<< info_.d11.first << " / " << info_.d11.second << " & "
				<< info_.d12.first << " / " << info_.d12.second ;

		}
};

//! basically the same as EocOutput, but with less columns (no stab coefficients)
class RefineOutput : public EocOutput
{
	typedef EocOutput
		BaseType;

	public:
		RefineOutput ( const RunInfo& info, BaseType::Strings& headers )
			: BaseType( info, headers )
		{}

		RefineOutput ( BaseType::Strings& headers )
			: BaseType( RunInfo(), headers )
		{}

		void putStaticCols( std::ofstream& outputFile_ )
		{
			std::stringstream runtime;
			if ( info_.run_time > 59 )
				runtime << long(info_.run_time) / 60 << ":" << long(info_.run_time) % 60 ;
			else
				runtime << long(info_.run_time) ;

			outputFile_ << std::setw( 4 )
				<< info_.grid_width << " & "
				<< info_.codim0 << " & "
				<< runtime.str() ;
		}
};

//! tex output for bfg runs
class BfgOutput : public TexOutputBase<RunInfo>
{
	typedef TexOutputBase<RunInfo>
		BaseType;

	RunInfo reference_;

	public:
		BfgOutput( const RunInfo& info, BaseType::Strings& headers )
			: BaseType( info, headers )
		{}

		BfgOutput( BaseType::Strings& headers, RunInfo reference )
			: BaseType( RunInfo(), headers ),
			reference_(reference)
		{}

		void putErrorCol( std::ofstream& outputFile_, const double /*prevError_*/, const double error_, const double /*prevh_*/,  const bool /*initial*/  )
		{
			//some trickery to calc correct diff w/o further work on the fem stuff
			static bool col = true;
			col = ! col;
			current_h_ = info_.grid_width;
			double diff = error_ - reference_.L2Errors[col];
			outputFile_ << " & " << std::scientific << error_ << std::fixed << " & " << diff;
		}

		void putHeader( std::ofstream& outputFile_ )
		{
			const unsigned int dynColSize = 2;
			const unsigned int statColSize = headers_.size() - 2;
			outputFile_ << "\\begin{longtable}{";

			for (unsigned int i=0;i<statColSize;i++) {
				if ( i == 2 )
					outputFile_ << "|r|";//runtime col
				else
					outputFile_ << "|c|";
			}

			for (unsigned int i=0;i<dynColSize;i++) {
				outputFile_ << "|cc|";
			}
			outputFile_ << "}\n"
				<< "\\caption{"
				<< info_.problemIdentifier << ": "
				<< info_.gridname
				<< " Polorder (u,p,$\\sigma$): (" << info_.polorder_velocity << ", "<< info_.polorder_pressure << ", "<< info_.polorder_sigma << " ) "
				<< " Loeser Genauigkeit: " << info_.solver_accuracy
				<< "}\\\\  \n"
				<< "\\hline \n";

			for (unsigned int i=0;i<statColSize;i++) {
				outputFile_ << headers_[i];
				if ( i <  statColSize - 1 )
					outputFile_ << " & ";
			}
			for (unsigned int i=0;i<dynColSize;i++) {
				outputFile_ << " & " << headers_[i+statColSize]
					<< " & Diff. zur Referenz";
			}
			outputFile_ << "\n \\endhead\n"
						<< "\\hline\n"
						<< "\\hline\n";
		}

		void putStaticCols( std::ofstream& outputFile_ )
		{
			std::stringstream runtime;
			if ( info_.run_time > 59 )
				runtime << long(info_.run_time) / 60 << ":" << long(info_.run_time) % 60 ;
			else
				runtime << long(info_.run_time) ;

			outputFile_ << std::setw( 4 )
				<< info_.grid_width << " & "
				<< info_.codim0 << " & "
				<< runtime.str() << " & "
				<< ( info_.bfg ? toString( info_.bfg_tau ) : std::string("--") ) << " & " //don't output a num in reference row
				<< info_.iterations_inner_avg << " & "
				<< info_.iterations_inner_min << " & "
				<< info_.iterations_inner_max << " & "
				<< info_.iterations_outer_total << " & "
				<< std::scientific << info_.max_inner_accuracy << std::fixed ;
		}
};

//! tex output for accuracy runs(where both inner and outer accuracy are varied)
class AccurracyOutput : public TexOutputBase<RunInfo>
{
	typedef TexOutputBase<RunInfo>
		BaseType;

	public:
		AccurracyOutput( BaseType::Strings& headers )
			: BaseType( RunInfo(), headers )
		{}

		void putErrorCol( std::ofstream& outputFile_, const double /*prevError_*/, const double error_, const double /*prevh_*/,  const bool /*initial*/  )
		{
			//some trickery to calc correct diff w/o further work on the fem stuff
			static bool col = true;
			col = ! col;
			current_h_ = info_.grid_width;
			outputFile_ << " & " << error_ ;
		}

		void putHeader( std::ofstream& outputFile_ )
		{
			const unsigned int dynColSize = 2;
			const unsigned int statColSize = headers_.size() - 2;
			outputFile_ << "\\begin{longtable}{";

			for (unsigned int i=0;i<statColSize;i++) {
				if ( i == 2 )
					outputFile_ << "|r|";//runtime col
				else
					outputFile_ << "|c|";
			}

			for (unsigned int i=0;i<dynColSize;i++) {
				outputFile_ << "|cc|";
			}
			outputFile_ << "}\n"
				<< "\\caption{"
				<< info_.problemIdentifier << ": "
				<< info_.gridname
				<< " Polorder (u,p,$\\sigma$): (" << info_.polorder_velocity << ", "<< info_.polorder_pressure << ", "<< info_.polorder_sigma << " ) "
				<< "}\\\\  \n"
				<< "\\hline \n";

			for (unsigned int i=0;i<statColSize;i++) {
				outputFile_ << headers_[i];
				if ( i <  statColSize - 1 )
					outputFile_ << " & ";
			}
			for (unsigned int i=0;i<dynColSize;i++) {
				outputFile_ << " & " << headers_[i+statColSize];
			}
			outputFile_ << "\n \\endhead\n"
						<< "\\hline\n"
						<< "\\hline\n";
		}

		void putStaticCols( std::ofstream& outputFile_ )
		{
			std::stringstream runtime;
			if ( info_.run_time > 59 )
				runtime << long(info_.run_time) / 60 << ":" << long(info_.run_time) % 60 ;
			else
				runtime << long(info_.run_time) ;

			outputFile_ << std::setw( 4 )
				<< info_.grid_width << " & "
				<< info_.codim0 << " & "
				<< runtime.str() << " & "
				<< info_.iterations_inner_avg << " & "
				<< std::scientific << info_.inner_solver_accuracy  << std::fixed << " & "
				<< info_.iterations_outer_total << " & "
				<< std::scientific << info_.solver_accuracy        << std::fixed ;
		}
};

//! tex output for accuracy runs(where only outer accuracy is varied)
class AccurracyOutputOuter : public TexOutputBase<RunInfo>
{
	typedef TexOutputBase<RunInfo>
		BaseType;

	public:
		AccurracyOutputOuter( BaseType::Strings& headers )
			: BaseType( RunInfo(), headers )
		{}

		void putErrorCol( std::ofstream& outputFile_, const double /*prevError_*/, const double error_, const double /*prevh_*/,  const bool /*initial*/  )
		{
			//some trickery to calc correct diff w/o further work on the fem stuff
			static bool col = true;
			col = ! col;
			current_h_ = info_.grid_width;
			outputFile_ << " & " << error_ ;
		}

		void putHeader( std::ofstream& outputFile_ )
		{
			const unsigned int dynColSize = 2;
			const unsigned int statColSize = headers_.size() - 2;
			outputFile_ << "\\begin{longtable}{";

			for (unsigned int i=0;i<statColSize;i++) {
				if ( i == 2 )
					outputFile_ << "|r|";//runtime col
				else
					outputFile_ << "|c|";
			}

			for (unsigned int i=0;i<dynColSize;i++) {
				outputFile_ << "|cc|";
			}
			outputFile_ << "}\n"
				<< "\\caption{"
				<< info_.problemIdentifier << ": "
				<< info_.gridname
				<< " Polorder (u,p,$\\sigma$): (" << info_.polorder_velocity << ", "<< info_.polorder_pressure << ", "<< info_.polorder_sigma << " ) "
				<< "}\\\\  \n"
				<< "\\hline \n";

			for (unsigned int i=0;i<statColSize;i++) {
				outputFile_ << headers_[i];
				if ( i <  statColSize - 1 )
					outputFile_ << " & ";
			}
			for (unsigned int i=0;i<dynColSize;i++) {
				outputFile_ << " & " << headers_[i+statColSize];
			}
			outputFile_ << "\n \\endhead\n"
						<< "\\hline\n"
						<< "\\hline\n";
		}

		void putStaticCols( std::ofstream& outputFile_ )
		{
			std::stringstream runtime;
			if ( info_.run_time > 59 )
				runtime << long(info_.run_time) / 60 << ":" << long(info_.run_time) % 60 ;
			else
				runtime << long(info_.run_time) ;

			outputFile_ << std::setw( 4 )
				<< info_.grid_width << " & "
				<< info_.codim0 << " & "
				<< runtime.str() << " & "
				<< info_.iterations_inner_avg << " & "
				<< info_.iterations_outer_total << " & "
				<< std::scientific << info_.solver_accuracy  << std::fixed ;
		}
};

} //namespace Tex
} //end namespace Stuff

#endif // DUNE_STUFF_TEX_HH
