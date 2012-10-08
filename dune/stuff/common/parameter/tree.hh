#ifndef DUNE_STUFF_COMMON_PARAMETER_TREE_HH
#define DUNE_STUFF_COMMON_PARAMETER_TREE_HH

#ifdef HAVE_CMAKE_CONFIG
 #include "cmake_config.h"
#else
 #include "config.h"
#endif // ifdef HAVE_CMAKE_CONFIG

// system
#include <sstream>
#include <boost/algorithm/string/join.hpp>
#include <boost/format.hpp>

// dune-common
#include <dune/common/exceptions.hh>
#include <dune/stuff/common/parameter/tree.hh>
#include <dune/common/parametertreeparser.hh>
#include <dune/stuff/common/string.hh>

namespace Dune {
namespace Stuff {
namespace Common {


//! ParameterTree extension for nicer output
class ExtendedParameterTree : public Dune::ParameterTree {
public:
  ExtendedParameterTree()
  {}

  explicit ExtendedParameterTree(const Dune::ParameterTree& other)
    : Dune::ParameterTree(other)
  {}

  ExtendedParameterTree& operator=(const Dune::ParameterTree& other)
  {
    if (this != &other) {
      Dune::ParameterTree::operator=(other);
    }
    return *this;
  }

  void report(std::ostream& stream = std::cout,
              const std::string& prefix = "") const
  {
    for(auto pair : values)
          stream << pair.first << " = \"" << pair.second << "\"" << std::endl;

    for(auto pair : subs)
    {
      ExtendedParameterTree sub(pair.second);
      if (sub.getValueKeys().size())
        stream << "[ " << prefix + pair.first << " ]" << std::endl;
      sub.report(stream, prefix + pair.first + ".");
    }
  }

  /**
    \brief      Fills a Dune::ParameterTree given a parameter file or command line arguments.
    \param[in]  argc
                From \c main()
    \param[in]  argv
                From \c main()
    \param[out] paramTree
                The Dune::ParameterTree that is to be filled.
    **/
  static ExtendedParameterTree init(int argc, char** argv, std::string filename)
  {
    ExtendedParameterTree paramTree;
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
};

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
} // void assertSub()

void assertKey(const Dune::ParameterTree& paramTree, std::string key, std::string id = "")
{
  if (!paramTree.hasKey(key)) {
    std::stringstream msg;
    msg << "Error";
    if (id != "") {
      msg << " in " << id;
    }
    msg << ": key '" << key << "' not found in the following Dune::Parametertree" << std::endl;
    paramTree.report(msg);
    DUNE_THROW(Dune::InvalidStateException, msg.str());
  }
} // void assertKey()

} // namespace Common
} // namespace Stuff
} // namespace Dune

#endif // DUNE_STUFF_COMMON_PARAMETER_TREE_HH
