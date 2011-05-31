#ifndef DUNE_STUFF_TIKZGRID_HH
#define DUNE_STUFF_TIKZGRID_HH

#include "misc.hh"
#include "grid.hh"
#include "static_assert.hh"
#include <boost/format.hpp>
#include <fstream>

namespace Stuff {
namespace Tex {

typedef wraparound_array<std::string,7>
	TexColorArrayType;
namespace {
	TexColorArrayType::BaseType tmp_colors = {{ "black","red","blue","green","yellow","cyan","magenta" }};
}
static const TexColorArrayType texcolors_( tmp_colors );

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
									  const int level = 0,
									  const bool print_entity_num = false)
			:file_(file),
			color_(color),
			level_(level),
			print_entity_num_(print_entity_num)
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

			IntersectionIteratorType intItEnd = ent.ilevelend();
			for (   IntersectionIteratorType intIt = ent.ilevelbegin();
					intIt != intItEnd;
					++intIt )
			{
				CoordType a = intIt->intersectionGlobal().corner(0);
				CoordType b = intIt->intersectionGlobal().corner(1);
				file_ << path % color_ % 0.8
							% a[0] % a[1]
							% b[0] % b[1];
			}
			maybePrintEntityNum( ent );
			file_.flush();
		}

		template < class Entity >
		void maybePrintEntityNum( const Entity& entity )
		{
			if ( !print_entity_num_)
				return;
			typedef typename Entity::Geometry
				EntityGeometryType;
			typedef Dune::FieldVector< typename EntityGeometryType::ctype, EntityGeometryType::coorddimension>
				CoordType;
			const CoordType center = getBarycenterGlobal(entity.geometry());
			file_ << boost::format( "\\node[circle] at (%f,%f) {%d};\n" )
//					 % color_
					 % center[0] % center[1] % 1;
		}

		void preWalk() const{}
		void postWalk()const{}

	protected:
		std::ofstream& file_;
		const std::string color_;
		const int level_;
		const bool print_entity_num_;
};

class PgfEntityFunctorIntersectionsWithShift : public PgfEntityFunctorIntersections {
	public:
		PgfEntityFunctorIntersectionsWithShift(std::ofstream& file,
									  const std::string color = "black",
									  const int level = 0,
									  bool print_entity_num = false)
			:PgfEntityFunctorIntersections(file,color,level,print_entity_num)
		{
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
			maybePrintEntityNum( ent );
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
			file_.flush();
		}
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
		PgfEntityFunctorIntersections pgf( file );
		grid_walk( pgf );
		file.close();
	}

private:
	const GridType& grid_;
};

template < class GridType >
class StackedPgfGrid {
public:
	StackedPgfGrid( GridType& grid )
		:grid_(grid)
	{
		dune_static_assert( GridType::dimensionworld == 2, "pgf output only implemented for dim 2" );
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
			PgfEntityFunctorIntersectionsWithShift pgf( file, texcolors_[i], i, true);//, 2*( (2*i+1)/float(maxref)+0.2) );
			grid_walk( pgf );
			file << "%%%%%%%%%%%%%%%" << view.size( 0 ) << "%%%%%%%%%%%%%%%%\n";
		}

		file << "\\end{tikzpicture}\n"
				"\\end{document}\n";
		file.close();
	}

private:
	GridType& grid_;
};

template < class GridType >
class RefineSeriesPgfGrid {
public:
	RefineSeriesPgfGrid( GridType& grid )
		:grid_(grid)
	{
		dune_static_assert( GridType::dimensionworld == 2, "pgf output only implemented for dim 2" );
	}

	void output( const std::string fn, const int refines )
	{
		Stuff::GridDimensions<GridType> dimensions( grid_ );
		const double x_diam = std::abs(dimensions.coord_limits[0].min()) +
				std::abs(dimensions.coord_limits[0].max());
		const double y_diam = std::abs(dimensions.coord_limits[1].min()) +
				std::abs(dimensions.coord_limits[1].max());
		std::ofstream file( getFileinDatadir( fn ).c_str() );
		boost::format level_float("\\subfloat[Level %d]{\n\\begin{tikzpicture}[scale=\\plotscale]\n");
		file << "\\documentclass{article}\n"
				"\\usepackage{tikz}\n\\usepackage{subfig}\n"
				"\\usetikzlibrary{calc,intersections, arrows,shapes.misc,shapes.arrows}\n"
				"\\pagestyle{empty}\n\\newcommand{\\plotscale}{0.8}\n"
				"\\begin{document}\n\\begin{figure}";
		const int maxref = refines;
		grid_.globalRefine( maxref );
//		for ( int i = maxref-1; i >= 0; --i )
		for ( int i = 0; i < maxref; ++i )
		{
			file << level_float % i;
			typedef typename GridType::LevelGridView
				View;
			{
				const View& view = grid_.levelView(i);
				GridWalk<View> grid_walk( view );
				PgfEntityFunctorIntersections thisLevel( file, "black", i);
				grid_walk( thisLevel );
			}
//			if ( i > 0 )//for dual/changed color outptu
//			{
//				const View& view = grid_.levelView(i-1);
//				GridWalk<View> grid_walk( view );
//				PgfEntityFunctorIntersections lastLevel( file, "black", i-1);
//				grid_walk( lastLevel );
//			}
			file << "\\end{tikzpicture}}\n";
		}
		file << "\\end{figure}\n"
				"\\end{document}\n";
		file.close();
	}

private:
	GridType& grid_;
};




} // namespace Tex
}//namespace Stuff {

#endif // DUNE_STUFF_TIKZGRID_HH
