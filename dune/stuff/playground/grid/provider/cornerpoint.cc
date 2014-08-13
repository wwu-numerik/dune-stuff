// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

const std::string Cornerpoint::id = "stuff.grid.provider.cornerpoint";

Cornerpoint::Cornerpoint(Dune::ParameterTree& paramTree)
  : grid_()
{
  // get correct parameters
  Dune::ParameterTree* paramsP = &paramTree;
  if (paramsP->hasSub(id))
    paramsP = &(paramsP->sub(id));
  Dune::ParameterTree& params = *paramsP;
  // build grid
  const std::string key = "filename";
  assert(params.hasKey(key));
  buildGrid(params.get(key, ""));
  /*buildMDGrid();*/
} // Cornerpoint::Cornerpoint(const Dune::ParameterTree& paramTree)

Cornerpoint::Cornerpoint(std::string filename)
  : grid_()
{
  buildGrid(filename);
  /*buildMDGrid();*/
} // Cornerpoint::Cornerpoint(std::string filename)

Dune::CpGrid& Cornerpoint::grid()
{
  return grid_;
}

const Dune::CpGrid& Cornerpoint::grid() const
{
  return grid_;
}

void Cornerpoint::visualize(Dune::ParameterTree& paramTree)
{
  const std::string localId = "visualize";
  // get correct parameters
  Dune::ParameterTree* paramsP = &paramTree;
  if (paramsP->hasSub(id))
    paramsP = &(paramsP->sub(id));
  if (paramsP->hasSub(localId))
    paramsP = &(paramsP->sub(localId));
  Dune::ParameterTree& params = *paramsP;
  // check for grid visualization
  if (params.hasKey("grid")) {
    const std::string filenameGrid = params.get("grid", id + ".grid");
    // grid view
    typedef Dune::CpGrid GridType;
    GridType& grid = this->grid();
    typedef GridType::LeafGridView GridView;
    GridView gridView = grid.leafView();
    // mapper
    Dune::LeafMultipleCodimMultipleGeomTypeMapper< GridType, P0Layout > mapper(grid);
    std::vector< double > data(mapper.size());
    // walk the grid
    typedef GridView::Codim<0>::Iterator ElementIterator;
    typedef GridView::Codim<0>::Entity ElementType;
    typedef ElementType::LeafIntersectionIterator FacetIteratorType;
    for (ElementIterator it = gridView.begin<0>(); it != gridView.end<0>(); ++it)
    {
      ElementType& element = *it;
      data[mapper.map(element)] = 0.0;
      int numberOfBoundarySegments = 0;
      bool isOnBoundary = false;
      for (FacetIteratorType facet = element.ileafbegin();
           facet != element.ileafend();
           ++facet) {
        if (!facet->neighbor() && facet->boundary()){
          isOnBoundary = true;
          numberOfBoundarySegments += 1;
          data[mapper.map(element)] += double(facet->boundaryId());
        }
      }
      if (isOnBoundary) {
        data[mapper.map(element)] /= double(numberOfBoundarySegments);
      }
    }
    // write to vtk
    Dune::VTKWriter< GridView > vtkwriter(gridView);
    vtkwriter.addCellData(data, "boundaryId");
    vtkwriter.write(filenameGrid, Dune::VTK::ascii);
  } // check for grid visualization
} // void Cornerpoint::visualize(Dune::ParameterTree& paramTree)

void Cornerpoint::buildGrid(std::string filename)
{
  grid_.readEclipseFormat(filename, 0.0, false, false);
  return;
} // void Cornerpoint::buildGrid(std::string filename)
