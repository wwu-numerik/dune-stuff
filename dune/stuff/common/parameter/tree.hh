#ifndef DUNE_STUFF_COMMON_PARAMETER_TREE_HH
#define DUNE_STUFF_COMMON_PARAMETER_TREE_HH

#include <cstring>
#include <sstream>
#include <vector>

#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

#if HAVE_EIGEN
# include <dune/stuff/common/disable_warnings.hh>
#   include <Eigen/Core>
# include <dune/stuff/common/reenable_warnings.hh>
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

  typedef Dune::ParameterTree BaseType;

public:
  ExtendedParameterTree();
  ExtendedParameterTree(const std::string filename);
  ExtendedParameterTree(int argc, char** argv, std::string filename);
  ExtendedParameterTree(const Dune::ParameterTree& other);

  ExtendedParameterTree& operator=(const Dune::ParameterTree& other);

  /**
   *  \brief adds another Dune::ParameterTree
   */
  void add(const Dune::ParameterTree& _other, const std::string _subName = "");
  ExtendedParameterTree sub(const std::string& _sub) const;

  void report(std::ostream& stream = std::cout, const std::string& prefix = "") const;
  void reportNicely(std::ostream& stream = std::cout) const;
  std::string reportString(const std::string& prefix = "") const;

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

  bool hasVector(const std::string& key) const;

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
    // get corresponding vector
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
    // get corresponding vector
    std::vector< T > vec = getVector< T >(key, minSize);
    // create eigen vector and return
    Eigen::Matrix< T, Eigen::Dynamic, 1 > ret(vec.size());
    for (unsigned int i = 0; i < vec.size(); ++i)
      ret(i) = vec[i];
    return ret;
  }
#endif // HAVE_EIGEN

  void assertKey(const std::string& key) const;
  void assertSub(const std::string& _sub) const;

  /**
    \brief      Fills a Dune::ParameterTree given a parameter file or command line arguments.
    \param[in]  argc
                From \c main()
    \param[in]  argv
                From \c main()
    \return     The Dune::ParameterTree that is to be filled.
    **/
  static ParameterTree init(int argc, char** argv, std::string filename);
  static ParameterTree init(const std::string filename);

  bool empty() const
  {
    return getValueKeys().empty() && this->getSubKeys().empty();
  }

private:
  void reportAsSub(std::ostream& stream, const std::string& prefix, const std::string& subPath) const;
  std::string findCommonPrefix(const BaseType& sub, const std::string previousPrefix = "") const;
  void reportFlatly(const BaseType& sub, const std::string& prefix = "", std::ostream& stream = std::cout) const;
}; // class ExtendedParameterTree

} // namespace Common
} // namespace Stuff
} // namespace Dune

#endif // DUNE_STUFF_COMMON_PARAMETER_TREE_HH

/** Copyright (c) 2012, Rene Milk, Felix Schindler
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
