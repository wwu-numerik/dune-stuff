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

class PgfEntityFunctor {
	public:
		PgfEntityFunctor( std::ofstream& file )
			:file_(file)
		{}

		template <class Entity>
		void operator() ( const Entity& ent, const Entity& /*dummy*/, const int ent_idx, const int ent_idx_neigh )
		{
			if ( ent_idx_neigh != ent_idx )
				return;
			typedef typename Entity::Geometry
				EntityGeometryType;
			typedef Dune::FieldVector< typename EntityGeometryType::ctype, EntityGeometryType::coorddimension>
				DomainType;
			const typename Entity::Geometry& geo = ent.geometry();
			double vol = geo.volume();
			boost::format vertex("\\coordinate(C_%d_%d) at (%f,%f);\n");
			for ( size_t i = 0; i < geo.corners(); ++i )
			{
				const DomainType& corner( geo.corner( i ) );
				file_ << vertex % ent_idx % i % corner[0] % corner[1];
			}
			// \draw (A)--(B)--(C)--cycle;
			file_ << "\\draw ";
			boost::format line("(C_%d_%d)--");
			for ( size_t i = 0; i < geo.corners(); ++i )
			{
				file_ << line % ent_idx % i ;
			}
			file_ << "cycle;\n ";
		}

		void preWalk() const{}
		void postWalk()const{}
	private:
		std::ofstream& file_;
};

class PgfEntityFunctorIntersections {
	public:
		PgfEntityFunctorIntersections(std::ofstream& file,
									  const std::string color = "black",
									  const int level = 0 )
			:file_(file),
			color_(color),
			level_(level)
		{
			std::cout << "level " << level_ << std::endl;
		}

		template <class Entity>
		void operator() ( const Entity& ent, const Entity& /*dummy*/, const int ent_idx, const int ent_idx_neigh )
		{
			if ( ent_idx_neigh != ent_idx )
				return;

			typedef typename Entity::Geometry
				EntityGeometryType;
			typedef typename Entity::LevelIntersectionIterator
				IntersectionIteratorType;
			typedef typename IntersectionIteratorType::Geometry
				IntersectionGeometry;
			typedef Dune::FieldVector< typename IntersectionGeometry::ctype, IntersectionGeometry::coorddimension>
				CoordType;

			boost::format path("\\draw[draw=%s,line width=%fpt,line cap=round] (%f,%f)--(%f,%f);\n");

			const CoordType center = getBarycenterGlobal(ent.geometry());
			const float fac = 0.16*level_;

			IntersectionIteratorType intItEnd = ent.ilevelend();
			for (   IntersectionIteratorType intIt = ent.ilevelbegin();
					intIt != intItEnd;
					++intIt )
			{
				CoordType a = intIt->intersectionGlobal().corner(0);
				CoordType b = intIt->intersectionGlobal().corner(1);
				CoordType a_c = center-a;
				CoordType b_c = center-b;
				a_c *=fac;
				b_c *=fac;
				a+=a_c;
				b+=b_c;
				file_ << path % color_ % 0.8
							% a[0] % a[1]
							% b[0] % b[1];
			}
			file_ << boost::format( "\\node[circle] at (%f,%f) {};\n" )
//					 % color_
					 % center[0] % center[1] ;
			file_.flush();
		}

		void preWalk() const{}
		void postWalk()const{}
	private:
		std::ofstream& file_;
		const std::string color_;
		const int level_;
};

template < class GridType >
class PgfGrid {
public:
	PgfGrid( const GridType& grid )
		:grid_(grid)
	{
		dune_static_assert( GridType::dimensionworld == 2, "pgf output only implemented for dim 2" );
	}

	void output( const std::string fn )
	{
		std::ofstream file( getFileinDatadir( fn ).c_str() );
		GridWalk<typename GridType::LeafGridView> grid_walk( grid_.leafView() );
//		PgfEntityFunctor pgf( file );
//		grid_walk( pgf );
//		file.close();

		PgfEntityFunctorIntersections pgf( file );
		grid_walk( pgf );
		file.close();
	}

private:
	const GridType& grid_;
};

template < class GridType >
class MultiPgfGrid {
public:
	MultiPgfGrid( GridType& grid )
		:grid_(grid)
	{
		dune_static_assert( GridType::dimensionworld == 2, "pgf output only implemented for dim 2" );
		texcolors_[0] = "black";
		texcolors_[1] = "red";
		texcolors_[2] = "blue";
		texcolors_[3] = "green";
		texcolors_[4] = "yellow";
		texcolors_[5] = "cyan";
		texcolors_[6] = "magenta";
	}

	void output( const std::string fn, const int refines )
	{
		std::ofstream file( getFileinDatadir( fn ).c_str() );
		file << "\\documentclass{article}\n"
				"\\usepackage{tikz}\n"
				"\\usetikzlibrary{calc,intersections}\n"
				"\\pagestyle{empty}\n"
				"\\begin{document}\n"
				"\\begin{tikzpicture}[scale=4.87]\n";
		const int maxref = refines;
		grid_.globalRefine( maxref );
//		for ( int i = maxref-1; i >= 0; --i )
		for ( int i = 0; i < maxref; ++i )
		{
			typedef typename GridType::LevelGridView
				View;
			const View& view = grid_.levelView(i);
			GridWalk<View> grid_walk( view );
			PgfEntityFunctorIntersections pgf( file, texcolors_[i], i);//, 2*( (2*i+1)/float(maxref)+0.2) );
			grid_walk( pgf );
			file << "%%%%%%%%%%%%%%%" << view.size( 0 ) << "%%%%%%%%%%%%%%%%\n";
		}
		file << "\\end{tikzpicture}\n"
				"\\end{document}\n";
		file.close();
	}

private:
	GridType& grid_;
	wraparound_array<std::string,7>  texcolors_;
};

} //end namespace Stuff

#endif // DUNE_STUFF_TEX_HH
