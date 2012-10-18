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
#include <dune/common/parametertreeparser.hh>
#include <dune/stuff/common/string.hh>

namespace Dune {
namespace Stuff {
namespace Common {

//! ParameterTree extension for nicer output
//! \todo This should go into dune-common.
class ExtendedParameterTree
  : public Dune::ParameterTree {
public:
  typedef Dune::ParameterTree BaseType;

  ExtendedParameterTree()
  {}

  ExtendedParameterTree(int argc, char** argv, std::string filename)
    : BaseType(init(argc, argv, filename))
  {}

  ExtendedParameterTree(const Dune::ParameterTree& other)
    : BaseType(other)
  {}

  ExtendedParameterTree& operator=(const Dune::ParameterTree& other)
  {
    if (this != &other) {
      Dune::ParameterTree::operator=(other);
    }
    return *this;
  }

  static void assertSub(const Dune::ParameterTree& paramTree, std::string sub, std::string id = "")
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
  } // static void assertSub(...)

  void assertSub(const std::string subTree, const std::string id = "") const
  {
    assertSub(*this, subTree, id);
  }

  static void assertKey(const Dune::ParameterTree& paramTree, const std::string key, const std::string id = "")
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
  } // static void assertKey(...)

  void assertKey(const std::string key, const std::string id = "") const
  {
    assertKey(*this, key, id);
  }

  void report(std::ostream& stream = std::cout,
              const std::string& prefix = "") const
  {
    for(auto pair : values)
          stream << pair.first << " = \"" << pair.second << "\"" << std::endl;

    for(auto pair : subs)
    {
      ExtendedParameterTree subTree(pair.second);
      if (subTree.getValueKeys().size())
        stream << "[ " << prefix + pair.first << " ]" << std::endl;
      subTree.report(stream, prefix + pair.first + ".");
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
  } // static ExtendedParameterTree init(...)
};

} // namespace Common
} // namespace Stuff
} // namespace Dune

#endif // DUNE_STUFF_COMMON_PARAMETER_TREE_HH
