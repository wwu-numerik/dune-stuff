// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

/**
  \file   dune/stuff/grid/provider/cornerpoint.hh
  \brief  dune/stuff/grid/provider/cornerpoint.hh - Contains a class to construct a cornerpoint grid from file.
  **/

#ifndef DUNE_STUFF_GRID_PROVIDER_CORNERPOINT_HH
#define DUNE_STUFF_GRID_PROVIDER_CORNERPOINT_HH

#if HAVE_DUNE_CORNERPOINT

// dune-common
#include <dune/common/parametertree.hh>
#include <dune/common/shared_ptr.hh>

// dune-grid
#include <dune/grid/common/mcmgmapper.hh>
#include <dune/grid/io/file/vtk/vtkwriter.hh>

// dune-cornerpoint
#include <dune/grid/CpGrid.hpp>

namespace Dune {

//namespace Capabilities
//{

//template< class Grid >
//struct hasHierarchicIndexSet;

//template<>
//struct hasHierarchicIndexSet< Dune::CpGrid >
//{
//  static const bool v = false;
//};

//} // namespace Capabilities

namespace Stuff {
namespace Grid {
namespace Provider {

/**
  \brief  Creates a cornerpoint grid from a grdecl file.

          A corresponding file can be obtained i.e. from https://www.sintef.no/Projectweb/MatMorA/Downloads/Johansen/. If
          \code $BASEDIR\endcode denotes the directory containing dune-rb, the following commands will download and unzip a sample grid file:
          \code mkdir -p $BASEDIR/dune-rb/dune/rb/grid/examples/data && \
 cd $BASEDIR/dune-rb/dune/rb/grid/examples/data && \
 wget https://www.sintef.no/project/MatMoRA/Johansen/FULLFIELD_Eclipse.zip && \
 unzip FULLFIELD_Eclipse.zip && \
 mv FULLFIELD_IMAXJMAX.GRDECL johansen_formation.grdecl && \
 rm FULLFIELD_*\endcode
  **/
class Cornerpoint
{
public:
  //! Type of the provided grid.
  typedef Dune::CpGrid GridType;

  //! Dimension of the provided grid.
  static const size_t dim = 3;

  //! Type of the grids coordinates.
  typedef Dune::FieldVector< GridType::ctype, dim > CoordinateType;

  //! Unique identifier: \c rb.grid.provider.cornerpoint
  static const std::string id;

  /**
    \brief      Creates a cornerpoint grid from a grdecl file.
    \param[in]  paramTree
                A Dune::ParameterTree containing
                <ul><li> the following keys directly or
                <li> a subtree named Cornerpoint::id, containing the following keys.</ul>
                The actual keys are:
                <ul><li> \c filename: an \a absolute path pointing to a .grdecl file.</ul>
    **/
  Cornerpoint(ParameterTree &paramTree);

  /**
    \brief      Creates a cornerpoint grid from a grdecl file.

    \param[in]  filename
                An \a absolute path pointing to a .grdecl file.
    **/
  Cornerpoint(std::string filename);

  /**
    \brief  Provides access to the created grid.
    \return Reference to the grid.
    **/
  GridType& grid();

  const GridType& grid() const;

private:
  template< size_t dim >
  struct P0Layout
  {
    bool contains(Dune::GeometryType& geometry)
    {
      if (geometry.dim() == dim)
        return true;
      return false;
    }
  }; // layout class for codim 0 mapper

  /**
    \brief      Visualizes the grid using Dune::VTKWriter.
    \param[in]  paramTree
                A Dune::ParameterTree containing
                <ul><li> the following keys directly or
                <li> a subtree named Cornerpoint::id, containing the following keys, or
                <li> a subtree named Cornerpoint::id + \c .visualize, containing the following keys.</ul>
                The actual keys are:
                <ul><li> \c grid: if specified, filename of the vtk file in which the grid which can be obtained via
                  grid() is visualized (\a if \a not \a specified: \a no \a visualization).</ul>
    **/
public:
  void visualize(Dune::ParameterTree& paramTree);

private:
  void buildGrid(std::string filename);

  GridType grid_;
}; // class Cornerpoint

#include "cornerpoint.cc"

} // namespace Provider
} // namespace Grid
} // namespace Stuff
} // namespace Dune

#endif // HAVE_DUNE_CORNERPOINT

#endif // DUNE_STUFF_GRID_PROVIDER_CORNERPOINT_HH
