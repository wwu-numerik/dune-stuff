// This file is part of the dune-stuff project:
//   http://users.dune-project.org/projects/dune-stuff/
// Copyright Holders: Felix Albrecht, Rene Milk
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#ifndef DUNE_STUFF_CONFIGTREE_HH
#define DUNE_STUFF_CONFIGTREE_HH

#include <iostream>
#include <sstream>
#include <vector>
#include <initializer_list>

#include <dune/common/parametertree.hh>
#include <dune/common/parametertreeparser.hh>
#include <dune/common/fvector.hh>
#include <dune/common/dynvector.hh>
#include <dune/common/densevector.hh>

#include <dune/stuff/common/exceptions.hh>
#include <dune/stuff/common/string.hh>
#include <dune/stuff/common/type_utils.hh>
#include <dune/stuff/la/container/dunedynamic.hh>
#if HAVE_EIGEN
# include <dune/stuff/la/container/eigen.hh>
#endif
#if HAVE_DUNE_ISTL
# include <dune/stuff/la/container/istl.hh>
#endif

namespace Dune {
namespace Stuff {
namespace Common {


class ConfigTree
  : public ParameterTree
{
  typedef ParameterTree BaseType;

  template< class T >
  class ChooseBase
  {
  protected:
    template< class VectorType, class S >
    static VectorType get_vector(const ParameterTree& config, const std::string key, const size_t size)
    {
      std::string vector_str = config.get< std::string >(key);
      // and check if this is a vector
      if (vector_str.substr(0, 1) == "[" && vector_str.substr(vector_str.size() - 1, 1) == "]") {
        vector_str = vector_str.substr(1, vector_str.size() - 2);
        // we treat this as a vector and split along ';'
        const auto tokens = tokenize< std::string >(vector_str, " ", boost::algorithm::token_compress_off);
        if (size > 0 && tokens.size() < size)
          DUNE_THROW_COLORFULLY(Exception::configuration_error,
                                "Vector (see below) to given key '" << key << "' has only " << tokens.size()
                                << " elements but " << size << " elements were requested!"
                                << "\n" << "'[" << vector_str << "]'");
        const size_t actual_size = (size > 0) ? std::min(tokens.size(), size) : tokens.size();
        VectorType ret(actual_size);
        for (size_t ii = 0; ii < actual_size; ++ii)
          ret[ii] = fromString< S >(boost::algorithm::trim_copy(tokens[ii]));
        return ret;
      } else {
        // we treat this as a scalar
        if (size > 0 && size != 1)
          DUNE_THROW_COLORFULLY(Exception::configuration_error,
                                "Vector (see below) to given key " << key << " has 1 element but " << size
                                << " elements were requested!"
                                << "\n" << "'" << vector_str << "'");
       VectorType ret(1);
        ret[0] = fromString< S >(vector_str);
        return ret;
      }
    } // ... get_vector(...)
  }; // class ChooseBase

  /**
   *  This version is used for non vectors. It just calls the get method of the ParameterTree. Specialize this for any
   *  vector you want to add und make use of ChooseBase::get_vector().
   */
  template< class T >
  class Choose
    : ChooseBase< T >
  {
  public:
    static T get(const ParameterTree& config, const std::string& key, const size_t size = 0)
    {
      return config.get< T >(key);
    }
  }; // class Choose< ... >

  template< class T >
  class Choose< std::vector< T > >
    : ChooseBase< std::vector< T > >
  {
    typedef ChooseBase< std::vector< T > > BaseType;
    typedef std::vector< T > VectorType;
  public:
    static VectorType get(const ParameterTree& config, const std::string& key, const size_t size = 0)
    {
      return BaseType::template get_vector< VectorType, T >(config, key, size);
    }
  }; // class Choose< std::vector< ... > >

  template< class K, int d >
  class Choose< FieldVector< K, d > >
    : ChooseBase< FieldVector< K, d > >
  {
    typedef ChooseBase< FieldVector< K, d > > BaseType;
    typedef FieldVector< K, d > VectorType;
  public:
    static VectorType get(const ParameterTree& config, const std::string& key, const size_t size = 0)
    {
      if (size > 0 && size != d)
        DUNE_THROW_COLORFULLY(Exception::configuration_error,
                              "You requested a '" << Typename< VectorType >::value() << "' for key '" << key
                              << "' with a 'size' of " << size
                              << " but this type of vector can not have any size other than " << d << "!");
      return BaseType::template get_vector< VectorType, K >(config, key, size);
    }
  }; // class Choose< FieldVector< ... > >

  template< class T >
  class Choose< DynamicVector< T > >
    : ChooseBase< DynamicVector< T > >
  {
    typedef ChooseBase< DynamicVector< T > > BaseType;
    typedef DynamicVector< T > VectorType;
  public:
    static VectorType get(const ParameterTree& config, const std::string& key, const size_t size = 0)
    {
      return BaseType::template get_vector< VectorType, T >(config, key, size);
    }
  }; // class Choose< DynamicVector< ... > >

  template< class S >
  class Choose< LA::DuneDynamicVector< S > >
    : ChooseBase< LA::DuneDynamicVector< S > >
  {
    typedef ChooseBase< LA::DuneDynamicVector< S > > BaseType;
    typedef LA::DuneDynamicVector< S > VectorType;
  public:
    static VectorType get(const ParameterTree& config, const std::string& key, const size_t size = 0)
    {
      return BaseType::template get_vector< VectorType, S >(config, key, size);
    }
  }; // class Choose< LA::DuneDynamicVector< ... > >

#if HAVE_DUNE_ISTL
  template< class S >
  class Choose< LA::IstlDenseVector< S > >
    : ChooseBase< LA::IstlDenseVector< S > >
  {
    typedef ChooseBase< LA::IstlDenseVector< S > > BaseType;
    typedef LA::IstlDenseVector< S > VectorType;
  public:
    static VectorType get(const ParameterTree& config, const std::string& key, const size_t size = 0)
    {
      return BaseType::template get_vector< VectorType, S >(config, key, size);
    }
  }; // class Choose< LA::IstlDenseVector< ... > >
#endif // HAVE_DUNE_ISTL

#if HAVE_EIGEN
  template< class S >
  class Choose< LA::EigenDenseVector< S > >
    : ChooseBase< LA::EigenDenseVector< S > >
  {
    typedef ChooseBase< LA::EigenDenseVector< S > > BaseType;
    typedef LA::EigenDenseVector< S > VectorType;
  public:
    static VectorType get(const ParameterTree& config, const std::string& key, const size_t size = 0)
    {
      return BaseType::template get_vector< VectorType, S >(config, key, size);
    }
  }; // class Choose< LA::EigenDenseVector< ... > >

  template< class S >
  class Choose< LA::EigenMappedDenseVector< S > >
    : ChooseBase< LA::EigenMappedDenseVector< S > >
  {
    typedef ChooseBase< LA::EigenMappedDenseVector< S > > BaseType;
    typedef LA::EigenMappedDenseVector< S > VectorType;
  public:
    static VectorType get(const ParameterTree& config, const std::string& key, const size_t size = 0)
    {
      return BaseType::template get_vector< VectorType, S >(config, key, size);
    }
  }; // class Choose< LA::EigenMappedDenseVector< ... > >
#endif // HAVE_EIGEN

public:
  ConfigTree()
    : BaseType()
  {}

  template< class T >
  ConfigTree(const std::string key, const T& value)
    : BaseType()
  {
    add(key, value);
  }

  template< class T >
  ConfigTree(const std::string key, const char* value)
    : BaseType()
  {
    add(key, value);
  }

  template< class T >
  ConfigTree(const std::vector< std::string > keys, const std::vector< T > values)
    : BaseType()
  {
    if (keys.size() != values.size())
      DUNE_THROW_COLORFULLY(Exception::shapes_do_not_match,
                            "The size of 'keys'' (" << keys.size() << ") does not match the size of 'values' ("
                            << values.size() << ")!");
    for (size_t ii = 0; ii < keys.size(); ++ii)
      add(keys[ii], values[ii]);
  }

  template< class T >
  ConfigTree(const std::vector< std::string > keys, const std::initializer_list< T > value_list)
    : BaseType()
  {
    std::vector< T > values(value_list);
    if (keys.size() != values.size())
      DUNE_THROW_COLORFULLY(Exception::shapes_do_not_match,
                            "The size of 'keys'' (" << keys.size() << ") does not match the size of 'value_list' ("
                            << values.size() << ")!");
    for (size_t ii = 0; ii < keys.size(); ++ii)
      add(keys[ii], values[ii]);
  }

  ConfigTree(const std::string filename)
    : BaseType(initialize(filename))
  {}

  ConfigTree(int argc, char** argv)
    : BaseType(initialize(argc, argv))
  {}

  ConfigTree(int argc, char** argv, const std::string filename)
    : BaseType(initialize(argc, argv, filename))
  {}

  ConfigTree(const Dune::ParameterTree& other)
    : BaseType(other)
  {}

  ConfigTree& operator=(const Dune::ParameterTree& other)
  {
    if (this != &other) {
      BaseType::operator=(other);
    }
    return *this;
  } // ... operator=(...)

  bool empty() const
  {
    return getValueKeys().empty() && BaseType::getSubKeys().empty();
  }

  void add(const ConfigTree& other, const std::string sub_id = "", const bool overwrite = false)
  {
    if (sub_id.empty()) {
      const auto& keys = other.getValueKeys();
      for (const std::string& key : keys) {
        if (has_key(key) && !overwrite)
          DUNE_THROW_COLORFULLY(Exception::configuration_error,
                                "While adding 'other' to this (see below), the key '" << key
                                << "' already exists and you requested no overwrite!"
                                << "\n==== this ============\n" << report_string()
                                << "\n==== other ===========\n" << other.report_string());
        add(key, other.get< std::string >(key));
      }
    } else {
      if (has_key(sub_id) && !overwrite)
        DUNE_THROW_COLORFULLY(Exception::configuration_error,
                              "While adding 'other' to this (see below), the key '" << sub_id
                              << "' already exists and you requested no overwrite!"
                              << "\n==== this ============\n" << report_string()
                              << "\n==== other ===========\n" << other.report_string());
      else if (has_sub(sub_id)) {
        ConfigTree sub_tree = BaseType::sub(sub_id);
        sub_tree.add(other);
        BaseType::sub(sub_id) = sub_tree;
      } else
        BaseType::sub(sub_id) = other;
    }
  } // ... add(...)

  template< typename T >
  void add(const std::string& key, const T& value, const bool overwrite = false)
  {
    if (has_key(key) && !overwrite)
      DUNE_THROW_COLORFULLY(Exception::configuration_error,
                            "While adding '" << key << "' = '" << value << "' to this (see below), the key '" << key
                            << "' already exists and you requested no overwrite!"
                            << "\n======================\n" << report_string());
    const std::string value_string = toString(value);
    BaseType::operator[](key) = value_string;
  } // ... add(...)

  void add(const std::string& key, const char* value, const bool overwrite = false)
  {
    if (has_key(key) && !overwrite)
      DUNE_THROW_COLORFULLY(Exception::configuration_error,
                            "While adding '" << key << "' = '" << value << "' to this (see below), the key '" << key
                            << "' already exists and you requested no overwrite!"
                            << "\n======================\n" << report_string());
    const std::string value_string = toString(value);
    BaseType::operator[](key) = value_string;
  } // ... add(...)

  bool has_sub(const std::string& sub_id) const
  {
    return BaseType::hasSub(sub_id);
  }

  ConfigTree sub(const std::string& sub_id) const
  {
    if (empty())
      DUNE_THROW_COLORFULLY(Exception::configuration_error,
                            "You can not get anything from an empty ConfigTree, use has_sub(\"" << sub_id
                            << "\") to check first!");
    if (sub_id.empty())
      DUNE_THROW_COLORFULLY(Exception::configuration_error, "Given sub_id must not be empty!");
    if (!has_sub(sub_id))
      DUNE_THROW_COLORFULLY(Exception::configuration_error,
                            "Subtree '" << sub_id << "' does not exist in this ConfigTree (see below), use has_sub(\""
                            << sub_id
                            << "\") to check first!"
                            << "\n======================\n" << report_string());
    return ConfigTree(BaseType::sub(sub_id));
  } // ... sub(...)

  bool has_key(const std::string& key) const
  {
    return BaseType::hasKey(key);
  }

  template< typename T >
  T get(const std::string& key, const T& default_value, const size_t size = 0) const
  {
    if (has_key(key))
      return Choose< T >::get(*this, key, size);
    else
      return default_value;
  } // ... get(...)

  std::string get(const std::string& key, const char* default_value, const size_t /*size*/ = 0) const
  {
    return BaseType::get(key, default_value);
  }

  /**
   * \param size Determines the size of the returning vector if T is a vector type (0 means automatic).
   */
  template< typename T >
  T get(const std::string& key, const size_t size = 0) const
  {
    if (empty())
      DUNE_THROW_COLORFULLY(Exception::configuration_error,
                            "You can not get anything from an empty ConfigTree, use has_key(\"" << key
                            << "\") to check first!");
    if (!has_key(key))
      DUNE_THROW_COLORFULLY(Exception::configuration_error,
                            "Given key '" << key << "' does not exist in this ConfigTree (see below), use has_key(\""
                            << key
                            << "\") to check first!"
                            << "\n======================\n" << report_string());
    return Choose< T >::get(*this, key, size);
  } // ... get(...)

  void report(std::ostream& out = std::cout, const std::string& prefix = "") const
  {
    if (!empty()) {
      if (subKeys.size() == 0) {
        report_as_sub(out, prefix, "");
      } else if (valueKeys.size() == 0) {
        const std::string common_prefix = find_common_prefix(*this, "");
        if (!common_prefix.empty()) {
          out << "[" << common_prefix << "]" << std::endl;
          const ConfigTree& commonSub = sub(common_prefix);
          report_flatly(commonSub, prefix, out);
        }
      } else {
        report_as_sub(out, prefix, "");
      }
    }
  } // ... report(...)

  std::string report_string(const std::string& prefix = "") const
  {
    std::stringstream stream;
    report(stream, prefix);
    return stream.str();
  } // ... report_string(...)

private:
  static BaseType initialize(const std::string filename)
  {
    BaseType param_tree;
    Dune::ParameterTreeParser::readINITree(filename, param_tree);
    return param_tree;
  } // ... initialize(...)

  static BaseType initialize(int argc, char** argv)
  {
    Dune::ParameterTree param_tree;
    if (argc == 2) {
      Dune::ParameterTreeParser::readINITree(argv[1], param_tree);
    } else if (argc > 2){
      Dune::ParameterTreeParser::readOptions(argc, argv, param_tree);
    }
    if (param_tree.hasKey("paramfile")) {
      Dune::ParameterTreeParser::readINITree(param_tree.get< std::string >("paramfile"), param_tree, false);
    }
    return param_tree;
  } // ... initialize(...)

  static BaseType initialize(int argc, char** argv, std::string filename)
  {
    Dune::ParameterTree param_tree;
    if (argc == 1) {
      Dune::ParameterTreeParser::readINITree(filename, param_tree);
    } else if (argc == 2) {
      Dune::ParameterTreeParser::readINITree(argv[1], param_tree);
    } else {
      Dune::ParameterTreeParser::readOptions(argc, argv, param_tree);
    }
    if (param_tree.hasKey("paramfile")) {
      Dune::ParameterTreeParser::readINITree(param_tree.get< std::string >("paramfile"), param_tree, false);
    }
    return param_tree;
  } // ... initialize(...)

  void report_as_sub(std::ostream& out, const std::string& prefix, const std::string& sub_path) const
  {
    for (auto pair : values)
      out << prefix << pair.first << " = " << pair.second << std::endl;
    for (auto pair : subs) {
      ConfigTree sub_tree(pair.second);
      if (sub_tree.getValueKeys().size())
        out << prefix << "[" << sub_path << pair.first << "]" << std::endl;
      sub_tree.report_as_sub(out, prefix, sub_path + pair.first + ".");
    }
  } // ... report_as_sub(...)

  std::string find_common_prefix(const BaseType& subtree, const std::string previous_prefix) const
  {
    const auto& valuekeys = subtree.getValueKeys();
    const auto& subkeys = subtree.getSubKeys();
    if (valuekeys.size() == 0 && subkeys.size() == 1) {
      // we append the subs name
      if (previous_prefix.empty())
        return find_common_prefix(subtree.sub(subkeys[0]), subkeys[0]);
      else
        return find_common_prefix(subtree.sub(subkeys[0]), previous_prefix + "." + subkeys[0]);
    } else {
      // end of the recursion, return the previous prefix
      return previous_prefix;
    }
  } // ... find_common_prefix(...)

  void report_flatly(const BaseType& subtree, const std::string& prefix, std::ostream& out) const
  {
    // report all the keys
    for (auto key : subtree.getValueKeys())
      out << prefix << key << " = " << subtree[key] << std::endl;
    // report all the subs
    for (auto subkey : subtree.getSubKeys()) {
      if (prefix.empty())
        report_flatly(subtree.sub(subkey), subkey + ".", out);
      else
        report_flatly(subtree.sub(subkey), prefix + subkey+ "." , out);
    }
  } // ... report_flatly(...)

}; // class ConfigTree


std::ostream& operator<<(std::ostream& out, const ConfigTree& config)
{
  config.report(out);
  return out;
}


} // namespace Common
} // namespace Stuff
} // namespace Dune

#endif // DUNE_STUFF_CONFIGTREE_HH
