#ifndef DUNE_STUFF_COMMON_PARAMETER_TREE_HH
#define DUNE_STUFF_COMMON_PARAMETER_TREE_HH

#ifdef HAVE_CMAKE_CONFIG
  #include "cmake_config.h"
#else
  #include "config.h"
#endif // ifdef HAVE_CMAKE_CONFIG

#include <cstring>
#include <sstream>
#include <vector>

#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

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
  } // ExtendedParameterTree& operator=(const Dune::ParameterTree& other)

  void report(std::ostream& stream = std::cout, const std::string& prefix = "") const
  {
    reportAsSub(stream, prefix, "");
  } // void report(std::ostream& stream = std::cout, const std::string& prefix = "") const

  std::string reportString(const std::string& prefix = "") const
  {
    std::stringstream stream;
    report(stream, prefix);
    return stream.str();
  } // std::stringstream reportString(const std::string& prefix = "") const

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

  template< class T >
  std::vector< T > getVector(const std::string& key, const T def, const unsigned int minSize = 1) const
  {
    std::vector< T > ret;
    if (!hasKey(key))
      ret = std::vector< T >(minSize, def);
    else {
      const std::string str = BaseType::get< std::string >(key, "default_value");
      // the dune parametertree strips any leading and trailing whitespace
      // so we can be sure that the first and last have to be the brackets [] if this is a vector
      if (Dune::Stuff::Common::String::equal(str.substr(0, 1), "[")
          && Dune::Stuff::Common::String::equal(str.substr(str.size() - 1, 1), "]")) {
        const std::vector< std::string > tokens = Dune::Stuff::Common::tokenize< std::string >(str.substr(1, str.size() - 2), ";");
        for (unsigned int i = 0; i < tokens.size(); ++i)
          ret.push_back(Dune::Stuff::Common::fromString< T >(boost::algorithm::trim_copy(tokens[i])));
        for (unsigned int i = ret.size(); i < minSize; ++i)
          ret.push_back(def);
      } else if (Dune::Stuff::Common::String::equal(str.substr(0, 1), "[")
                 || Dune::Stuff::Common::String::equal(str.substr(str.size() - 1, 1), "]")) {
          DUNE_THROW(Dune::InvalidStateException, "Vectors have to be of the form '[entry_0; entry_1; ... ]'!");
      } else {
        ret = std::vector< T >(minSize, Dune::Stuff::Common::fromString< T >(boost::algorithm::trim_copy(str)));
      }
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
  static ParameterTree init(int argc, char** argv, std::string filename)
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

private:
  void reportAsSub(std::ostream& stream, const std::string& prefix, const std::string& subPath) const
  {
    for (auto pair : values)
      stream << prefix << pair.first << " = " << pair.second << std::endl;
//      stream << prefix << pair.first << " = \"" << pair.second << "\"" << std::endl;
    for (auto pair : subs) {
      ExtendedParameterTree subTree(pair.second);
      if (subTree.getValueKeys().size())
        stream << prefix << "[ " << subPath << pair.first << " ]" << std::endl;
      subTree.reportAsSub(stream, prefix, subPath + pair.first + ".");
    }
  } // void report(std::ostream& stream = std::cout, const std::string& prefix = "") const
}; // class ExtendedParameterTree

} // namespace Common
} // namespace Stuff
} // namespace Dune

#endif // DUNE_STUFF_COMMON_PARAMETER_TREE_HH
