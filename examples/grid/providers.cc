
#ifdef HAVE_CMAKE_CONFIG
#include "cmake_config.h"
#elif defined (HAVE_CONFIG_H)
#include <config.h>
#endif // HAVE_CMAKE_CONFIG

// system
#include <iostream>

// boost
#include <boost/filesystem.hpp>

// dune-common
#include <dune/common/exceptions.hh>
#include <dune/common/shared_ptr.hh>
#include <dune/common/mpihelper.hh>
#include <dune/common/timer.hh>

// dune-stuff
#include <dune/stuff/common/parameter/tree.hh>
#include <dune/stuff/common/logging.hh>
#include <dune/stuff/common/filesystem.hh>
#include <dune/stuff/common/string.hh>
#include <dune/stuff/grid/provider/cube.hh>
#include <dune/stuff/grid/provider/gmsh.hh>

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
    file << "provider = stuff.grid.provider.cube" << std::endl;
    file << "[stuff.grid.provider.cube]" << std::endl;
    file << "level = 4" << std::endl;
    file << "filename = " << id << ".grid" << std::endl;
    file << "[stuff.grid.provider.gmsh]" << std::endl;
    file << "mshfile = sample.msh" << std::endl;
    file << "filename = " << id << ".grid" << std::endl;
    file.close();
  } // only write param file if there is none
} // void ensureParamFile()

/**
 * \brief Creates a grid provider
 *
 * \param[in] paramTree
 * \return A shared ptr to the created grid provider
 */
Dune::shared_ptr< Dune::Stuff::Grid::Provider::Interface<> >
  createProvider(const Dune::Stuff::Common::ExtendedParameterTree& paramTree)
{
  // prepare
  paramTree.assertKey(id + ".provider");
  const std::string providerId = paramTree.sub(id).get("provider", "default_value");
  typedef Dune::Stuff::Grid::Provider::Interface<> InterfaceType;
  // choose provider
  if (providerId == "stuff.grid.provider.cube") {
    typedef Dune::Stuff::Grid::Provider::Cube<> DerivedType;
    paramTree.assertSub(DerivedType::id(), id);
    Dune::shared_ptr< InterfaceType > provider(new DerivedType(paramTree.sub(DerivedType::id())));
    return provider;
//  } else if (providerId == "stuff.grid.provider.gmsh") {
//    typedef Dune::Stuff::Grid::Provider::Gmsh<> DerivedType;
//    paramTree.assertSub(DerivedType::id(), id);
//    Dune::shared_ptr< InterfaceType > provider(new DerivedType(paramTree.sub(DerivedType::id())));
//    return provider;
  } else {
    std::stringstream msg;
    msg << std::endl << "Error in " << id << ": unknown provider ('" << providerId << "') given in the following Dune::Parametertree" << std::endl;
    paramTree.report(msg);
    DUNE_THROW(Dune::InvalidStateException, msg.str());
  } // choose provider
} // ... createProvider(...)

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

/**
 * \todo  Move gmsh into seperate examples, since it does not compile with YaspGrid and SGrid!
 */
int main(int argc, char** argv)
{
  try {
    // mpi
    Dune::MPIHelper::instance(argc, argv);

    // parameter
    const std::string filename = id + ".param";
    ensureParamFile(filename);
    Dune::Stuff::Common::ExtendedParameterTree paramTree(argc, argv, filename);

    // logger
    Dune::Stuff::Common::Logger().create(Dune::Stuff::Common::LOG_INFO |
                                         Dune::Stuff::Common::LOG_CONSOLE |
                                         Dune::Stuff::Common::LOG_DEBUG);
    Dune::Stuff::Common::LogStream& info = Dune::Stuff::Common::Logger().info();

    // timer
    Dune::Timer timer;

    // grid
    info << "setting up grid ";
    paramTree.assertSub(id, id);
    info << "using '" << paramTree.sub(id).get("provider", "default_value") << "' provider:" << std::endl;
    typedef Dune::Stuff::Grid::Provider::Interface<> GridProviderType;
    const Dune::shared_ptr< GridProviderType > gridProvider = createProvider(paramTree);
    typedef GridProviderType::GridType GridType;
    const GridType& grid = gridProvider->grid();
    info << "  took " << timer.elapsed()
         << " sec (has " << grid.size(0) << " elements)" << std::endl;

    info << "visualizing grid... " << std::flush;
    timer.reset();
    gridProvider->visualize(paramTree.sub(gridProvider->id()).get("filename", id + ".grid"));
    info << " done (took " << timer.elapsed() << " sek)" << std::endl;

    info << "walking leaf grid view... " << std::flush;
    timer.reset();
    const unsigned int leafElements = measureTiming(grid.leafView());
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
