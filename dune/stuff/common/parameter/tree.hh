#ifndef DUNE_STUFF_COMMON_PARAMETER_TREE_HH
#define DUNE_STUFF_COMMON_PARAMETER_TREE_HH

// system
#include <sstream>

// dune-common
#include <dune/common/exceptions.hh>
#include <dune/common/parametertree.hh>
#include <dune/common/parametertreeparser.hh>

namespace Dune
{

namespace Stuff
{

namespace Common
{

namespace Parameter
{

namespace Tree
{

/**
  \brief      Fills a Dune::ParameterTree given a parameter file or command line arguments.
  \param[in]  argc
              From \c main()
  \param[in]  argv
              From \c main()
  \param[out] paramTree
              The Dune::ParameterTree that is to be filled.
  **/
Dune::ParameterTree init(int argc, char** argv, std::string filename)
{
  Dune::ParameterTree paramTree;
  if (argc == 1) {
    Dune::ParameterTreeParser::readINITree(filename, paramTree);
  } else if (argc == 2) {
    Dune::ParameterTreeParser::readINITree(argv[1], paramTree);
  } else {
    Dune::ParameterTreeParser::readOptions(argc, argv, paramTree);
  }
  if (paramTree.hasKey("paramfile")) {
    Dune::ParameterTreeParser::readINITree(paramTree.get< std::string >("paramfile"), paramTree, false);
  }
  return paramTree;
} // Dune::ParameterTree init(int argc, char** argv, std::string filename)

void assertSub(const Dune::ParameterTree& paramTree, std::string sub, std::string id = "")
{
  if (!paramTree.hasSub(sub)) {
    std::stringstream msg;
    msg << "Error";
    if (id != "") {
      msg << " in " << id;
    }
    msg << ": subTree '" << sub << "' not found in the following Dune::Parametertree" << std::endl;
    paramTree.report(msg);
    DUNE_THROW(Dune::InvalidStateException, msg.str());
  }
} // void assert_sub(std::string "sub")

} // namespace Tree

} // namespace Parameter

} // namespace Common

} // namespace Stuff

} // namespace Dune

#endif // DUNE_STUFF_COMMON_PARAMETER_TREE_HH
