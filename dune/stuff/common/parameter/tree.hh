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

#if HAVE_EIGEN
  #include <Eigen/Core>
#endif // HAVE_EIGEN

#include <dune/common/exceptions.hh>
#include <dune/common/parametertreeparser.hh>
#include <dune/common/dynvector.hh>

#include <dune/stuff/common/string.hh>
#include <dune/stuff/common/color.hh>
#include <dune/stuff/common/logging.hh>
#include <dune/stuff/aliases.hh>

namespace Dune {
namespace Stuff {
namespace Common {

//! ParameterTree extension for nicer output
//! \todo TODO The report method should go into dune-common
class ExtendedParameterTree
  : public Dune::ParameterTree {
public:
  typedef Dune::ParameterTree BaseType;

  ExtendedParameterTree()
  {}

  ExtendedParameterTree(const std::string filename)
    : BaseType(init(filename))
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
      BaseType::operator=(other);
    }
    return *this;
  } // ExtendedParameterTree& operator=(const Dune::ParameterTree& other)

  /**
   *  \brief adds another Dune::ParameterTree
   */
  void add(const Dune::ParameterTree& _other, const std::string _subName = "")
  {
    if (_subName.empty()) {
      // copy each key/value pair and append subName
      const Dune::ParameterTree::KeyVector& keyVector = _other.getValueKeys();
      for (size_t ii = 0; ii < keyVector.size(); ++ii) {
        const std::string key = keyVector[ii];
        if (BaseType::hasKey(key))
          DUNE_THROW(Dune::InvalidStateException,
                     "\n" << DSC::colorStringRed("ERROR:")
                     << " key '" << key << "' already exists in the follingDune::ParameterTree:\n"
                     << reportString("  "));
        BaseType::operator[](key) = _other.get< std::string >(key);
      }
    } else {
      if (BaseType::hasKey(_subName))
        DUNE_THROW(Dune::InvalidStateException,
                   "\n" << DSC::colorStringRed("ERROR:")
                   << " key '" << _subName << "' already exists in the follingDune::ParameterTree:\n"
                   << reportString("  "));
      else if (BaseType::hasSub(_subName)) {
        ExtendedParameterTree _sub = BaseType::sub(_subName);
        _sub.add(_other);
        BaseType::sub(_subName) = _sub;
      } else
        BaseType::sub(_subName) = _other;
    }
  } // ... add(...)

  ExtendedParameterTree sub(const std::string& _sub) const
  {
    if (!hasSub(_sub))
        DUNE_THROW(Dune::RangeError,
                   "\n" << DSC::colorStringRed("ERROR:")
                   << " sub '" << _sub << "' missing in the following Dune::ParameterTree:\n" << reportString("  "));
    return ExtendedParameterTree(BaseType::sub(_sub));
  }

  void report(std::ostream& stream = std::cout, const std::string& prefix = "") const
  {
    reportAsSub(stream, prefix, "");
  } // void report(std::ostream& stream = std::cout, const std::string& prefix = "") const

  void reportNicely(std::ostream& stream = std::cout) const
  {
    if (valueKeys.size() == 0 || subKeys.size() == 0) {
      const std::string commonPrefix = findCommonPrefix(*this);
      if (!commonPrefix.empty()) {
        stream << "[" << commonPrefix << "]" << std::endl;
        const ExtendedParameterTree& commonSub = sub(commonPrefix);
        reportFlatly(commonSub, "", stream);
      }
    } else
      reportAsSub(stream, "", "");
  }

  std::string reportString(const std::string& prefix = "") const
  {
    std::stringstream stream;
    report(stream, prefix);
    return stream.str();
  } // std::stringstream reportString(const std::string& prefix = "") const

  std::string get(const std::string& key, const char* defaultValue) const
  {
    return this->get< std::string >(key, std::string(defaultValue));
  }

  template< typename T >
  T get(const std::string& key, const T& defaultValue) const
  {
    if (!BaseType::hasKey(key)) {
      DSC::Logger().debug() << DSC::colorString("WARNING:") << " missing key '" << key << "' is replaced by given default value!" << std::endl;
    }
    return BaseType::get< T >(key, defaultValue);
  }

  template< class T >
  T get(const std::string& key) const
  {
    if (!BaseType::hasKey(key))
      DUNE_THROW(Dune::RangeError,
                 "\n" << Dune::Stuff::Common::colorStringRed("ERROR:") << " key '" << key << "' missing  in the following Dune::ParameterTree:\n" << reportString("  "));
    return BaseType::get< T >(key);
  }

  bool hasVector(const std::string& key) const
  {
    if (hasKey(key)) {
      const std::string str = BaseType::get< std::string >(key, "meaningless_default_value");
      return (str.substr(0, 1) ==  "["
          && str.substr(str.size() - 1, 1) == "]");
    }
    return false;
  } // bool hasVector(const std::string& vector) const

  template< class T >
  Dune::DynamicVector< T > getDynVector(const std::string& key, const T& def, const size_t minSize) const
  {
    const std::vector< T > vector = getVector< T >(key, def, minSize);
    Dune::DynamicVector< T > ret(vector.size());
    for (size_t ii = 0; ii < vector.size(); ++ii)
      ret[ii] = vector[ii];
    return ret;
  }

  template< class T >
  std::vector< T > getVector(const std::string& key, const T& def, const unsigned int minSize) const
  {
    if (!hasKey(key)) {
      DSC::Logger().debug() << DSC::colorString("WARNING:") << " missing key '" << key << "' is replaced by given default value!" << std::endl;
      return std::vector< T >(minSize, def);
    } else {
      const std::string str = BaseType::get(key, "meaningless_default_value");
      if (str.empty()) {
        if (minSize > 0) {
          DSC::Logger().debug() << DSC::colorString("WARNING:") << " vector '" << key << "' was too small (0) and has been enlarged to size " << minSize << "!" << std::endl;
        }
        return std::vector< T >(minSize, def);
      } else if (str.size() < 3) {
        std::vector< T > ret;
        ret.push_back(DSC::fromString< T >(str));
        if (ret.size() < minSize) {
          DSC::Logger().debug() << DSC::colorString("WARNING:") << " vector '" << key << "' was too small (" << ret.size() << ") and has been enlarged to size " << minSize << "!" << std::endl;
          for (auto i = ret.size(); i < minSize; ++i)
            ret.push_back(def);
        }
        return ret;
      } else {
        // the dune parametertree strips any leading and trailing whitespace
        // so we can be sure that the first and last have to be the brackets [] if this is a vector
        std::vector< T > ret;
        if (str.substr(0, 1) == "["
            && str.substr(str.size() - 1, 1) == "]") {
          std::vector< std::string > tokens;
          if (str.size() > 2)
            tokens = DSC::tokenize< std::string >(str.substr(1, str.size() - 2), ";");
          for (unsigned int i = 0; i < tokens.size(); ++i)
            ret.push_back(DSC::fromString< T >(boost::algorithm::trim_copy(tokens[i])));
          for (auto i = ret.size(); i < minSize; ++i)
            ret.push_back(def);
        } else if (str.substr(0, 1) == "["
                   || str.substr(str.size() - 1, 1) == "]") {
            DUNE_THROW(Dune::RangeError, "Vectors have to be of the form '[entry_0; entry_1; ... ]'!");
        } else {
          ret = std::vector< T >(minSize, DSC::fromString< T >(boost::algorithm::trim_copy(str)));
        }
        return ret;
      }
    }
  } // std::vector< T > getVector(const std::string& key, const T def) const

  template< class T >
  Dune::DynamicVector< T > getDynVector(const std::string& _key, const size_t minSize) const
  {
    const std::vector< T > vector = getVector< T >(_key, minSize);
    Dune::DynamicVector< T > ret(vector.size());
    for (size_t ii = 0; ii < vector.size(); ++ii)
      ret[ii] = vector[ii];
    return ret;
  }

  template< class T >
  std::vector< T > getVector(const std::string& key, const unsigned int minSize) const
  {
    if (!hasKey(key)) {
      DUNE_THROW(Dune::RangeError,
                 "\n" << DSC::colorStringRed("ERROR:")
                 << " key '" << key << "' missing  in the following Dune::ParameterTree:\n" << reportString("  "));
    } else {
      std::vector< T > ret;
      const std::string str = BaseType::get< std::string >(key, "meaningless_default_value");
      // the dune parametertree strips any leading and trailing whitespace
      // so we can be sure that the first and last have to be the brackets [] if this is a vector
      if (str.substr(0, 1) == "[" && str.substr(str.size() - 1, 1) == "]") {
        const std::vector< std::string > tokens = DSC::tokenize< std::string >(str.substr(1, str.size() - 2), ";");
        for (unsigned int i = 0; i < tokens.size(); ++i)
          ret.push_back(DSC::fromString< T >(boost::algorithm::trim_copy(tokens[i])));
      } else if (minSize == 1)
        ret.push_back(DSC::fromString< T >(str));
      else
          DUNE_THROW(Dune::RangeError, "Vectors have to be of the form '[entry_0; entry_1; ... ]'!");
      if (ret.size() < minSize)
        DUNE_THROW(Dune::RangeError,
                   "\n" << DSC::colorStringRed("ERROR:")
                   << " vector '" << key
                   << "' too short (is " << ret.size() << ", should be at least " << minSize
                   << ") in the following Dune::ParameterTree :\n" << reportString("  "));
      return ret;
    }
  } // std::vector< T > getVector(const std::string& key, const T def) const

#if HAVE_EIGEN
  template< class T >
  Eigen::Matrix< T, Eigen::Dynamic, 1 > getEigenVector(const std::string& key,
                                                       const T& def,
                                                       const unsigned int minSize) const
  {
    // get correspongin vector
    std::vector< T > vec = getVector< T >(key, def, minSize);
    // create eigen vector and return
    Eigen::Matrix< T, Eigen::Dynamic, 1 > ret(vec.size());
    for (unsigned int i = 0; i < vec.size(); ++i)
      ret(i) = vec[i];
    return ret;
  }

  template< class T >
  Eigen::Matrix< T, Eigen::Dynamic, 1 > getEigenVector(const std::string& key, const unsigned int minSize) const
  {
    // get correspongin vector
    std::vector< T > vec = getVector< T >(key, minSize);
    // create eigen vector and return
    Eigen::Matrix< T, Eigen::Dynamic, 1 > ret(vec.size());
    for (unsigned int i = 0; i < vec.size(); ++i)
      ret(i) = vec[i];
    return ret;
  }
#endif // HAVE_EIGEN

  void assertKey(const std::string& key) const
  {
    if (!BaseType::hasKey(key))
      DUNE_THROW(Dune::RangeError,
                 "\n" << DSC::colorStringRed("ERROR:")
                 << " key '" << key << "' missing  in the following Dune::ParameterTree:\n" << reportString("  "));
  }

  void assertSub(const std::string& _sub) const
  {
    if (!BaseType::hasSub(_sub))
      DUNE_THROW(Dune::RangeError,
                 "\n" << DSC::colorStringRed("ERROR:")
                 << " sub '" << _sub << "' missing  in the following Dune::ParameterTree:\n" << reportString("  "));
  }

  /**
    \brief      Fills a Dune::ParameterTree given a parameter file or command line arguments.
    \param[in]  argc
                From \c main()
    \param[in]  argv
                From \c main()
    \return     The Dune::ParameterTree that is to be filled.
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

  static ParameterTree init(const std::string filename)
  {
    Dune::ParameterTree paramTree;
    Dune::ParameterTreeParser::readINITree(filename, paramTree);
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
        stream << prefix << "[" << subPath << pair.first << "]" << std::endl;
      subTree.reportAsSub(stream, prefix, subPath + pair.first + ".");
    }
  } // void report(std::ostream& stream = std::cout, const std::string& prefix = "") const

  std::string findCommonPrefix(const BaseType& sub, const std::string previousPrefix = "") const
  {
    const auto& valuekeys = sub.getValueKeys();
    const auto& subkeys = sub.getSubKeys();
    if (valuekeys.size() == 0 && subkeys.size() == 1) {
      // we append the subs name
      if (previousPrefix.empty())
        return findCommonPrefix(sub.sub(subkeys[0]), subkeys[0]);
      else
        return findCommonPrefix(sub.sub(subkeys[0]), previousPrefix + "." + subkeys[0]);
    } else {
      // end of the recursion, return the previous prefix
      return previousPrefix;
    }
  }

  void reportFlatly(const BaseType& sub, const std::string& prefix = "", std::ostream& stream = std::cout) const
  {
    // report all the keys
    for (auto key : sub.getValueKeys())
      stream << prefix << key << " = " << sub[key] << std::endl;
    // report all the subs
    for (auto subkey : sub.getSubKeys()) {
      if (prefix.empty())
        reportFlatly(sub.sub(subkey), subkey + ".", stream);
      else
        reportFlatly(sub.sub(subkey), prefix + subkey+ "." , stream);
    }
  }
}; // class ExtendedParameterTree

} // namespace Common
} // namespace Stuff
} // namespace Dune

#endif // DUNE_STUFF_COMMON_PARAMETER_TREE_HH

/** Copyright (c) 2012, Rene Milk, Felix Albrecht
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
