#ifndef DUNE_STUFF_TIKZGRID_HH
#define DUNE_STUFF_TIKZGRID_HH

#include <dune/stuff/common/misc.hh>
#include "walk.hh"
#include <dune/common/static_assert.hh>
#include <boost/format.hpp>
#include <dune/grid/io/file/dgfparser/dgfparser.hh>
#include <fstream>

namespace Dune {

namespace Stuff {

namespace Grid {

  namespace Tikz {

typedef wraparound_array< std::string, 7 >
TexColorArrayType;

namespace {
TexColorArrayType::BaseType tmp_colors = { { "black", "red", "blue", "green", "yellow", "cyan", "magenta" } };
}
static const TexColorArrayType texcolors_(tmp_colors);

struct TikzCoordWrapper
  : Dune::FieldVector< double, 2 >
{
  template< class T > TikzCoordWrapper(const T& vector) {
    (*this)[0] = (*this)[1] = 0;
    for (size_t i; i < std::min(int( vector.size() ), 2); ++i)
      (*this)[i] = vector[i];
  }
};

/** \brief print a tex(tikz) representation of each entity to file
   * \ingroup GridWalk
   **/
class PgfEntityFunctor
{
public:
  PgfEntityFunctor(std::ofstream& file)
    : file_(file)
  {}

  template< class Entity >
  void operator()(const Entity& ent, const Entity& /*dummy*/, const int ent_idx, const int ent_idx_neigh) {
    if (ent_idx_neigh != ent_idx)
      return;

    typedef typename Entity::Geometry
    EntityGeometryType;
    typedef Dune::FieldVector< typename EntityGeometryType::ctype, EntityGeometryType::coorddimension >
    DomainType;
    const typename Entity::Geometry& geo = ent.geometry();
    double vol = geo.volume();
    boost::format vertex("\\coordinate(C_%d_%d) at (%f,%f);\n");
    for (size_t i = 0; i < geo.corners(); ++i)
    {
      const TikzCoordWrapper corner( geo.corner(i) );
      file_ << vertex % ent_idx % i % corner[0] % corner[1];
    }
    // \draw (A)--(B)--(C)--cycle;
    file_ << "\\draw ";
    boost::format line("(C_%d_%d)--");
    for (size_t i = 0; i < geo.corners(); ++i)
    {
      file_ << line % ent_idx % i;
    }
    file_ << "cycle;\n ";
  } // ()

  void preWalk() const {}
  void postWalk() const {}

private:
  std::ofstream& file_;
};

/** \brief print a tex(tikz) representation of each entity intersection to file
   * \ingroup GridWalk
   **/

class PgfEntityFunctorIntersections
{
public:
  PgfEntityFunctorIntersections(std::ofstream& file,
                                const std::string color = "black",
                                const int level = 0,
                                const bool print_entity_num = false)
    : file_(file)
      , color_(color)
      , level_(level)
      , print_entity_num_(print_entity_num) {
    std::cout << "level " << level_ << std::endl;
  }

  template< class Entity >
  void operator()(const Entity& ent, const Entity& /*dummy*/, const int ent_idx, const int ent_idx_neigh) {
    if (ent_idx_neigh != ent_idx)
      return;

    typedef typename Entity::LevelIntersectionIterator
    IntersectionIteratorType;
    typedef typename IntersectionIteratorType::Intersection::Geometry
    IntersectionGeometry;
    typedef Dune::FieldVector< typename IntersectionGeometry::ctype, IntersectionGeometry::coorddimension >
    CoordType;

    boost::format path("\\draw[draw=%s,line width=%s pt,line cap=round] (%f,%f)--(%f,%f);\n");

    IntersectionIteratorType intItEnd = ent.ilevelend();
    for (IntersectionIteratorType intIt = ent.ilevelbegin();
         intIt != intItEnd;
         ++intIt)
    {
      TikzCoordWrapper a( intIt->geometry().corner(0) );
      TikzCoordWrapper b( intIt->geometry().corner(1) );
      file_ << path % color_ % "\\gridlinewidth"
      % a[0] % a[1]
      % b[0] % b[1];
    }
    maybePrintEntityNum(ent, ent_idx);
    file_.flush();
  } // ()

  template< class Entity >
  void maybePrintEntityNum(const Entity& entity, const int idx) {
    if (!print_entity_num_)
      return;

    typedef typename Entity::Geometry
    EntityGeometryType;
    TikzCoordWrapper center( entity.geometry().center() );
    file_ << boost::format("\\node[circle] at (%f,%f) {%d};\n")
// % color_
    % center[0] % center[1] % idx;
  } // maybePrintEntityNum

  void preWalk() const {}
  void postWalk() const {}

protected:
  std::ofstream& file_;
  const std::string color_;
  const int level_;
  const bool print_entity_num_;
};

/** \brief print a tex(tikz) representation of each entity to file
   * \note PgfEntityFunctorIntersections, shifting the drawn line "inwards"
   * \ingroup GridWalk
   **/
class PgfEntityFunctorIntersectionsWithShift
  : public PgfEntityFunctorIntersections
{
public:
  PgfEntityFunctorIntersectionsWithShift(std::ofstream& file,
                                         const std::string color = "black",
                                         const int level = 0,
                                         bool print_entity_num = false)
    : PgfEntityFunctorIntersections(file, color, level, print_entity_num)
  {}

  template< class Entity >
  void operator()(const Entity& ent, const Entity& /*dummy*/, const int ent_idx, const int ent_idx_neigh) {
    if (ent_idx_neigh != ent_idx)
      return;

    typedef typename Entity::Geometry
    EntityGeometryType;
    typedef typename Entity::LevelIntersectionIterator
    IntersectionIteratorType;
    typedef typename IntersectionIteratorType::Intersection::Geometry
    IntersectionGeometry;
    typedef Dune::FieldVector< typename IntersectionGeometry::ctype, IntersectionGeometry::coorddimension >
    CoordType;

    boost::format path("\\draw[draw=%s,line width=%s pt,line cap=round] (%f,%f)--(%f,%f);\n");

    const TikzCoordWrapper center( ent.geometry().center() );
    const float fac = 0.16 * level_;
    maybePrintEntityNum(ent, ent_idx);
    IntersectionIteratorType intItEnd = ent.ilevelend();
    for (IntersectionIteratorType intIt = ent.ilevelbegin();
         intIt != intItEnd;
         ++intIt)
    {
      TikzCoordWrapper a( intIt->geometry().corner(0) );
      TikzCoordWrapper b( intIt->geometry().corner(1) );
      TikzCoordWrapper a_c = center - a;
      TikzCoordWrapper b_c = center - b;
      a_c *= fac;
      b_c *= fac;
      a += a_c;
      b += b_c;
      file_ << path % color_ % "\\gridlinewidth"
      % a[0] % a[1]
      % b[0] % b[1];
    }
    file_.flush();
  } // ()
};

//! print a tex representation of any given 2D grid leaf level to file
template< class GridType >
class PgfGrid
{
public:
  PgfGrid(const GridType& grid)
    : grid_(grid)
  {}

  void output(const std::string fn) {
    std::ofstream file( getFileinDatadir(fn).c_str() );

    GridWalk< typename GridType::LeafGridView > grid_walk( grid_.leafView() );
    PgfEntityFunctorIntersections pgf(file);
    grid_walk(pgf);
    file.close();
  } // output

private:
  const GridType& grid_;
};

//! print a tex representation of any given 2D grid refine views to file
template< class GridType >
class StackedPgfGrid
{
public:
  StackedPgfGrid(GridType& grid)
    : grid_(grid)
  {}

  void output(const std::string fn, const int refines, const bool includable = true) {
    std::ofstream file( getFileinDatadir(fn).c_str() );

    if (!includable)
    {
      file << "\\documentclass{article}\n"
              "\\usepackage{tikz}\n"
              "\\usetikzlibrary{calc,intersections}\n"
              "\\pagestyle{empty}\n"
              "\\begin{document}\n"
              "\\begin{tikzpicture}[scale=4.87]\n";
    } else
      file << "\\begin{tikzpicture}\n";
    const int maxref = refines;
    grid_.globalRefine(maxref);
// for ( int i = maxref-1; i >= 0; --i )
    for (int i = 0; i < maxref; ++i)
    {
      typedef typename GridType::LevelGridView
      View;
      const View& view = grid_.levelView(i);
      GridWalk< View > grid_walk(view);
      PgfEntityFunctorIntersectionsWithShift pgf(file, texcolors_[i], i, true); // , 2*( (2*i+1)/float(maxref)+0.2) );
      grid_walk(pgf);
      file << "%%%%%%%%%%%%%%%" << view.size(0) << "%%%%%%%%%%%%%%%%\n";
    }

    file << "\\end{tikzpicture}\n";
    if (!includable)
      file << "\\end{document}\n";
    file.close();
  } // output

private:
  GridType& grid_;
};

//! print a tex representation of any given 2D grid refine views to file
template< class GridType >
class RefineSeriesPgfGrid
{
public:
  RefineSeriesPgfGrid(GridType& grid)
    : grid_(grid)
  {}

  void output(const std::string fn, const int refines, const bool includable = true) {
    Stuff::GridDimensions< GridType > dimensions(grid_);
    std::ofstream file( getFileinDatadir(fn).c_str() );
    boost::format level_float("\\subfloat[Level %d]{\n\\begin{tikzpicture}[scale=\\gridplotscale]\n");
    if (!includable)
      file << "\\documentclass{article}\n"
              "\\usepackage{tikz}\n\\usepackage{subfig}\n"
              "\\usetikzlibrary{calc,intersections, arrows,shapes.misc,shapes.arrows}\n"
              "\\pagestyle{empty}\n\\newcommand{\\gridplotscale}{0.8}\n"
              "\\newcommand{\\gridcoordscale}{0.8}\n"
              "\\newcommand{\\gridlinewidth}{0.8}\n"
              "\\begin{document}\n\\begin{figure}";
    const int maxref = refines * Dune::DGFGridInfo< GridType >::refineStepsForHalf();
    grid_.globalRefine(maxref);
// for ( int i = maxref-1; i >= 0; --i )
    for (int i = 0; i < maxref; i += 1)
    {
      typedef typename GridType::LevelGridView
      View;
      {
        const View& view = grid_.levelView(i);
        file << level_float % i;
        GridWalk< View > grid_walk(view);
        PgfEntityFunctorIntersections thisLevel(file, "black", i, true);
        grid_walk(thisLevel);
      }
// if ( i > 0 )//for dual/changed color outptu
// {
// const View& view = grid_.levelView(i-1);
// GridWalk<View> grid_walk( view );
// PgfEntityFunctorIntersections lastLevel( file, "black", i-1);
// grid_walk( lastLevel );
// }

      switch (GridType::dimensionworld) {
       case 1: {
         boost::format edge("\\node[scale=\\gridcoordscale] at (%f,0) {(%d)};\n");
         const double offset = 0.2;
         file << edge % (dimensions.coord_limits[0].min() - offset)
         % ( dimensions.coord_limits[0].min() );
         file << edge % (dimensions.coord_limits[0].max() + offset)
         % ( dimensions.coord_limits[0].max() );
         break;
       }
       case 2:
       case 3: {
         boost::format edge("\\node[scale=\\gridcoordscale] at (%f,%f) {(%d,%d)};\n");
         const double offset = 0.2;
         file << edge % (dimensions.coord_limits[0].min() - offset)
         % (dimensions.coord_limits[1].min() - offset)
         % ( dimensions.coord_limits[0].min() )
         % ( dimensions.coord_limits[1].min() );
         file << edge % (dimensions.coord_limits[0].max() + offset)
         % (dimensions.coord_limits[1].max() + offset)
         % ( dimensions.coord_limits[0].max() )
         % ( dimensions.coord_limits[1].max() );
         break;
       }
      } // switch
      if ( (i + 1) % 3 == 0 )
        file << "\\end{tikzpicture}}\\\\\n";
      else
        file << "\\end{tikzpicture}}\n";
    }
    if (!includable)
      file << "\\end{figure}\n"
              "\\end{document}\n";
    file.close();
  } // output

private:
  GridType& grid_;
};
} // namespace Tikz

} // namespace Grid

} // namespace Stuff

} // namespace Dune

#endif // DUNE_STUFF_TIKZGRID_HH
/** Copyright (c) 2012, Rene Milk
   * All rights reserved.
   *
   * Redistribution and use in source and binary forms, with or without
   * modification, are permitted provided that the following conditions are met:
   *
   * 1. Redistributions of source code must retain the above copyright notice, this
   *    list of conditions and the following disclaimer.
   * 2. Redistributions in binary form must reproduce the above copyright notice,
   *    this list of conditions and the following disclaimer in the documentation
   *    and/or other materials provided with the distribution.
   *
   * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
   * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
   * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
   * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
   * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
   * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
   *
   * The views and conclusions contained in the software and documentation are those
   * of the authors and should not be interpreted as representing official policies,
   * either expressed or implied, of the FreeBSD Project.
   **/
