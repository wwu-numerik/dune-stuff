#ifdef HAVE_CMAKE_CONFIG
  #include "cmake_config.h"
#elif defined (HAVE_CONFIG_H)
  #include <config.h>
#endif // HAVE_CMAKE_CONFIG

#include <iostream>

#include <boost/filesystem.hpp>

#include <dune/common/exceptions.hh>
#include <dune/common/shared_ptr.hh>
#include <dune/common/mpihelper.hh>
#include <dune/common/timer.hh>

#include <dune/stuff/common/parameter/tree.hh>
#include <dune/stuff/common/logging.hh>
#include <dune/stuff/common/filesystem.hh>
#include <dune/stuff/common/string.hh>
#include <dune/stuff/grid/provider.hh>

const std::string id = "grid.providers";

/**
  \brief      Creates a parameter file if it does not exist.

              Nothing is done if the file already exists. If not, a parameter file will be created with all neccessary
              keys and values.
  \param[in]  filename
              (Relative) path to the file.
  **/
void ensureParamFile(std::string filename)
{
  Dune::Stuff::Common::testCreateDirectory(filename);
  if (!boost::filesystem::exists(filename)) {
    std::ofstream file;
    file.open(filename);
    file << "[" << id << "]" << std::endl;
    file << "grid = stuff.grid.provider.cube" << std::endl;
    file << "       stuff.grid.provider.gmsh" << std::endl;
    file << "       stuff.grid.provider.starcd" << std::endl;
    file << "filename = " << id << ".grid"<< std::endl;
    file << "[stuff.grid.provider.cube]" << std::endl;
    file << "lowerLeft = [0.0; 0.0; 0.0]" << std::endl;
    file << "upperRight = [1.0; 1.0; 1.0]" << std::endl;
    file << "numElements = [4; 4; 4]" << std::endl;
    file << "[stuff.grid.provider.gmsh]" << std::endl;
    file << "filename = curved2d.msh" << std::endl;
    file << "[stuff.grid.provider.starcd]" << std::endl;
    file << "filename = meshExport10" << std::endl;
    file.close();
  } // only write param file if there is none
} // void ensureParamFile()

template< class GridViewType >
unsigned int measureTiming(const GridViewType& gridView)
{
  unsigned int elements = 0;
  for (typename GridViewType::template Codim< 0 >::Iterator it = gridView.template begin< 0 >();
       it != gridView.template end< 0 >();
       ++it)
    ++elements;
  return elements;
} // unsigned int measureTiming(const GridViewType& gridView)

int main(int argc, char** argv)
{
  try {
    // mpi
    Dune::MPIHelper::instance(argc, argv);

    // parameter
    const std::string filename = id + ".param";
    ensureParamFile(filename);
    Dune::Stuff::Common::ExtendedParameterTree paramTree(argc, argv, filename);
    if (!paramTree.hasSub(id))
      DUNE_THROW(Dune::RangeError,
                 "\nMissing sub '" << id << "' in the following Dune::ParameterTree:\n" << paramTree.reportString("  "));

    // logger
    Dune::Stuff::Common::Logger().create(Dune::Stuff::Common::LOG_INFO |
                                         Dune::Stuff::Common::LOG_CONSOLE |
                                         Dune::Stuff::Common::LOG_DEBUG);
    Dune::Stuff::Common::LogStream& info = Dune::Stuff::Common::Logger().info();

    // timer
    Dune::Timer timer;

    // grid
    info << "setting up grid... ";
    typedef Dune::Stuff::Grid::Provider::Interface<> GridProviderType;
    const GridProviderType* gridProvider =
        Dune::Stuff::Grid::Provider::create<>(paramTree.get(id + ".grid", "stuff.grid.provider.cube"),
                                              paramTree);
    typedef GridProviderType::GridType GridType;
    const Dune::shared_ptr< const GridType > grid = gridProvider->grid();
    info << "  done (has " << grid->size(0) << " elements, took " << timer.elapsed() << " sec)" << std::endl;

    info << "visualizing grid... " << std::flush;
    timer.reset();
    gridProvider->visualize(paramTree.sub(id).get("filename", id + ".grid"));
    info << " done (took " << timer.elapsed() << " sek)" << std::endl;

    info << "walking leaf grid view... " << std::flush;
    timer.reset();
    const unsigned int leafElements = measureTiming(grid->leafView());
    info << " done (has " << leafElements << " elements, took " << timer.elapsed() << " sek)" << std::endl;

    // if we came that far we can as well be happy about it
    return 0;
  } catch(Dune::Exception& e) {
    std::cerr << "Dune reported error: " << e.what() << std::endl;
  } catch(std::exception& e) {
    std::cerr << e.what() << std::endl;
  } catch(...) {
    std::cerr << "Unknown exception thrown!" << std::endl;
  } // try
} // main
