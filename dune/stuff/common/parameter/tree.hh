#ifndef DUNE_STUFF_COMMON_PARAMETER_TREE_HH
#define DUNE_STUFF_COMMON_PARAMETER_TREE_HH

#ifdef HAVE_CMAKE_CONFIG
 #include "cmake_config.h"
#else
 #include "config.h"
#endif // ifdef HAVE_CMAKE_CONFIG

// system
#include <cstring>
#include <sstream>
#include <vector>

// boost
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

// dune-common
#include <dune/common/exceptions.hh>
#include <dune/common/parametertreeparser.hh>

// dune-stuff
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
      ExtendedParameterTree(paramTree).report(msg);
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
      ExtendedParameterTree(paramTree).report(msg);
      DUNE_THROW(Dune::InvalidStateException, msg.str());
    }
  } // static void assertKey(...)

  void assertKey(const std::string key, const std::string id = "") const
  {
    assertKey(*this, key, id);
  }

  void report(std::ostream& stream = std::cout, const std::string& prefix = "") const
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

  bool hasVector(const std::string& vector) const
  {
    if (hasKey(vector)) {
      const std::string str = get< std::string >(vector, "default_value");
      if (Dune::Stuff::Common::String::equal(str.substr(0, 1), "[")
          && Dune::Stuff::Common::String::equal(str.substr(str.size() - 1, 1), "]"))
        return true;
    }
    return false;
  } // bool hasVector(const std::string& vector) const

  void assertVector(const std::string vector, const std::string id = "") const
  {
    if (!hasVector(vector)) {
      std::stringstream msg;
      msg << "Error";
      if (id != "") {
        msg << " in " << id;
      }
      msg << ": vector '" << vector << "' not found in the following Dune::Parametertree" << std::endl;
      report(msg);
      DUNE_THROW(Dune::InvalidStateException, msg.str());
    }
  } // void assertVector(...)

  template< class T >
  std::vector< T > getVector(const std::string& key, const T def) const
  {
    std::vector< T > ret;
    if (!hasKey(key))
      ret.push_back(def);
    else {
      const std::string str = get< std::string >(key, "default_value");
      // the dune parametertree strips any leading and trailing whitespace
      // so we can be sure that the first and last have to be the brackets []
      assert(Dune::Stuff::Common::String::equal(str.substr(0, 1), "[")
             && "Vectors have to be of the form '[entry_0; entry_1; ... ]'!");
      assert(Dune::Stuff::Common::String::equal(str.substr(str.size() - 1, 1), "]")
             && "Vectors have to be of the form '[entry_0; entry_1; ... ]'!");
      const std::vector< std::string > tokens = Dune::Stuff::Common::tokenize< std::string >(str.substr(1, str.size() - 2), ";");
      for (unsigned int i = 0; i < tokens.size(); ++i)
        ret.push_back(Dune::Stuff::Common::fromString< T >(boost::algorithm::trim_copy(tokens[i])));
    }
    return ret;
  } // std::vector< T > getVector(const std::string& key, const T def) const

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
  } // static ExtendedParameterTree init(...)
};

} // namespace Common
} // namespace Stuff
} // namespace Dune

#endif // DUNE_STUFF_COMMON_PARAMETER_TREE_HH
