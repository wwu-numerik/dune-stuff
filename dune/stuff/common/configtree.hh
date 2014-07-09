// This file is part of the dune-stuff project:
//   https://users.dune-project.org/projects/dune-stuff/
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#ifndef DUNE_STUFF_CONFIGTREE_HH
#define DUNE_STUFF_CONFIGTREE_HH

#include <iostream>
#include <sstream>
#include <vector>
#include <limits>
#include <initializer_list>
#include <exception>

#include <dune/common/parametertree.hh>
#include <dune/common/parametertreeparser.hh>
#include <dune/common/fvector.hh>
#include <dune/common/fmatrix.hh>
#include <dune/common/dynvector.hh>
#include <dune/common/dynmatrix.hh>
#include <dune/common/densevector.hh>
#include <dune/common/densematrix.hh>

#include <dune/stuff/common/exceptions.hh>
#include <dune/stuff/common/string.hh>
#include <dune/stuff/common/type_utils.hh>
#include <dune/stuff/common/fvector.hh>
#include <dune/stuff/common/fmatrix.hh>
#include <dune/stuff/la/container/common.hh>
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
    template< class M >
    struct MatrixConstructor
    {
      static M create(const size_t rows, const size_t cols)
      {
        return M(rows, cols);
      }
    };

    template< class K, int r, int c >
    struct MatrixConstructor< Dune::FieldMatrix< K, r, c > >
    {
      static Dune::FieldMatrix< K, r, c > create(const size_t /*rows*/, const size_t /*cols*/)
      {
        return Dune::FieldMatrix< K, r, c >();
      }
    };

    template< class M >
    struct MatrixSetter
    {
      template< class V >
      static void set_entry(M& mat, const size_t rr, const size_t cc, const V& val)
      {
        mat[rr][cc] = val;
      }
    };

    template< class S >
    struct MatrixSetter< Stuff::LA::CommonDenseMatrix< S > >
    {
      template< class V >
      static void set_entry(Stuff::LA::CommonDenseMatrix< S >& mat, const size_t rr, const size_t cc, const V& val)
      {
        mat.set_entry(rr, cc, val);
      }
    };

#if HAVE_EIGEN
    template< class S >
    struct MatrixSetter< Stuff::LA::EigenDenseMatrix< S > >
    {
      template< class V >
      static void set_entry(Stuff::LA::EigenDenseMatrix< S >& mat, const size_t rr, const size_t cc, const V& val)
      {
        mat.set_entry(rr, cc, val);
      }
    };
#endif // HAVE_EIGEN

    static std::string trim_copy_safely(const ConfigTree& cfg, const std::string& key, const std::string& str_in)
    {
      const std::string str_out = boost::algorithm::trim_copy(str_in);
      if (str_out.find(";") != std::string::npos)
        DUNE_THROW_COLORFULLY(Exceptions::configuration_error,
                              "There was an error while parsing the value for key '" << key << "' in the config below. "
                              << "The value contained a ';': '" << str_out << "'!\n"
                              << "This usually happens if you try to get a matrix expression with a vector type "
                              << "or if you are missing the white space after the ';' in a matrix expression!\n\n"
                              << cfg.report_string());
      return str_out;
    } // ... trim_copy_safely(...)

    template< class S >
    static S convert_from_string_safely(const ConfigTree& cfg, const std::string& key, const std::string& str_in)
    {
      S s_out;
      try {
        s_out = fromString< S >(str_in);
      } catch (std::exception& e) {
        DUNE_THROW_COLORFULLY(Exceptions::external_error,
                              "There was the following error in the stl while parsing the value '" << str_in
                              << "' for key '" << key << "' in the config below: " << e.what() << "\n\n"
                              << cfg.report_string());
      } catch (boost::bad_lexical_cast& e) {
        DUNE_THROW_COLORFULLY(Exceptions::external_error,
                              "There was the following error in boost while parsing the value '" << str_in
                              << "' for key '" << key << "' in the config below: " << e.what() << "\n\n"
                              << cfg.report_string());
      }
      return s_out;
    } // ... convert_from_string_safely(...)

  protected:
    template< class VectorType, class S >
    static VectorType get_vector(const ParameterTree& config, const std::string key, const size_t size)
    {
      std::string vector_str = config.get< std::string >(key);
      // and check if this is a vector
      if (vector_str.substr(0, 1) == "[" && vector_str.substr(vector_str.size() - 1, 1) == "]") {
        vector_str = vector_str.substr(1, vector_str.size() - 2);
        // we treat this as a vector and split along ' '
        const auto tokens = tokenize< std::string >(vector_str, " ", boost::algorithm::token_compress_on);
        if (size > 0 && tokens.size() < size)
          DUNE_THROW_COLORFULLY(Exceptions::configuration_error,
                                "Vector (see below) to given key '" << key << "' has only " << tokens.size()
                                << " elements but " << size << " elements were requested!"
                                << "\n" << "'[" << vector_str << "]'");
        const size_t actual_size = (size > 0) ? std::min(tokens.size(), size) : tokens.size();
        VectorType ret(actual_size);
        for (size_t ii = 0; ii < actual_size; ++ii)
          ret[ii] = convert_from_string_safely< S >(config, key, trim_copy_safely(config, key, tokens[ii]));
        return ret;
      } else {
        // we treat this as a scalar
        const S val = convert_from_string_safely< S >(config, key, trim_copy_safely(config, key, vector_str));
        const size_t actual_size = (size == 0 ? 1 : size);
        VectorType ret(actual_size);
        for (size_t ii = 0; ii < actual_size; ++ii)
          ret[ii] = val;
        return ret;
      }
    } // ... get_vector(...)

    template< class MatrixType, class S >
    static MatrixType get_matrix(const ParameterTree& config,
                                 const std::string key,
                                 const size_t rows,
                                 const size_t cols)
    {
      std::string matrix_str = config.get< std::string >(key);
      // and check if this is a matrix
      if (matrix_str.substr(0, 1) == "[" && matrix_str.substr(matrix_str.size() - 1, 1) == "]") {
        matrix_str = matrix_str.substr(1, matrix_str.size() - 2);
        // we treat this as a matrix and split along ';' to obtain the rows
        const auto row_tokens = tokenize< std::string >(matrix_str, ";", boost::algorithm::token_compress_on);
        if (rows > 0 && row_tokens.size() < rows)
          DUNE_THROW_COLORFULLY(Exceptions::configuration_error,
                                "Matrix (see below) to given key '" << key << "' has only " << row_tokens.size()
                                << " rows but " << rows << " rows were requested!"
                                << "\n" << "'[" << matrix_str << "]'");
        const size_t actual_rows = (rows > 0) ? std::min(row_tokens.size(), rows) : row_tokens.size();
        // compute the number of columns the matrix will have
        size_t min_cols = std::numeric_limits< size_t >::max();
        for (size_t rr = 0; rr < actual_rows; ++rr) {
          const auto row_token = boost::algorithm::trim_copy(row_tokens[rr]);
          // we treat this as a vector, so we split along ' '
          const auto column_tokens = tokenize< std::string >(row_token, " ", boost::algorithm::token_compress_on);
          min_cols = std::min(min_cols, column_tokens.size());
        }
        if (cols > 0 && min_cols < cols)
          DUNE_THROW_COLORFULLY(Exceptions::configuration_error,
                                "Matrix (see below) to given key '" << key << "' has only " << min_cols
                                << " columns but " << cols << " columns were requested!"
                                << "\n" << "'[" << matrix_str << "]'");
        const size_t actual_cols = (cols > 0) ? std::min(min_cols, cols) : min_cols;
        MatrixType ret = MatrixConstructor< MatrixType >::create(actual_rows, actual_cols);
        // now we do the same again and build the actual matrix
        for (size_t rr = 0; rr < actual_rows; ++rr) {
          const std::string row_token = boost::algorithm::trim_copy(row_tokens[rr]);
          const auto column_tokens = tokenize< std::string >(row_token, " ", boost::algorithm::token_compress_on);
          for (size_t cc = 0; cc < actual_cols; ++cc)
            MatrixSetter< MatrixType >::set_entry(ret, rr, cc,
                                                  convert_from_string_safely< S >(config, key, trim_copy_safely(config, key, column_tokens[cc])));
        }
        return ret;
      } else {
        // we treat this as a scalar
        const S val = convert_from_string_safely< S >(config, key, trim_copy_safely(config, key, matrix_str));
        const size_t actual_rows = (rows == 0 ? 1 : rows);
        const size_t actual_cols = (cols == 0 ? 1 : cols);
        MatrixType ret = MatrixConstructor< MatrixType >::create(actual_rows, actual_cols);
        for (size_t rr = 0; rr < actual_rows; ++rr)
          for (size_t cc = 0; cc < actual_cols; ++cc)
            MatrixSetter< MatrixType >::set_entry(ret, rr, cc, val);
        return ret;
      }
    } // ... get_matrix(...)
  }; // class ChooseBase

  /**
   *  This version is used for non containers. It just calls the get method of the ParameterTree. Specialize this for
   *  any vector or matrix you want to add und make use of ChooseBase::get_vector().
   */
  template< class T >
  class Choose
    : ChooseBase< T >
  {
  public:
    static T get(const ParameterTree& config, const std::string& key,
                 const size_t /*size*/ = 0, const size_t /*cols*/ = 0)
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
    static VectorType get(const ParameterTree& config, const std::string& key,
                          const size_t size = 0, const size_t /*cols*/ = 0)
    {
      return BaseType::template get_vector< VectorType, T >(config, key, size);
    }
  }; // class Choose< std::vector< ... > >

  template< class K, int d >
  class Choose< Dune::FieldVector< K, d > >
    : ChooseBase< Dune::FieldVector< K, d > >
  {
    typedef ChooseBase< Dune::FieldVector< K, d > > BaseType;
    typedef Dune::FieldVector< K, d > VectorType;
  public:
    static VectorType get(const ParameterTree& config, const std::string& key,
                          const size_t size = 0, const size_t /*cols*/ = 0)
    {
      if (size > 0 && size != d)
        DUNE_THROW_COLORFULLY(Exceptions::configuration_error,
                              "You requested a '" << Typename< VectorType >::value() << "' for key '" << key
                              << "' with a 'size' of " << size
                              << " but this type of vector can not have any size other than " << d << "!");
      return BaseType::template get_vector< VectorType, K >(config, key, d);
    }
  }; // class Choose< Dune::FieldVector< ... > >

  template< class K, int d >
  class Choose< Dune::Stuff::Common::FieldVector< K, d > >
    : ChooseBase< Dune::Stuff::Common::FieldVector< K, d > >
  {
    typedef ChooseBase< Dune::Stuff::Common::FieldVector< K, d > > BaseType;
    typedef Dune::Stuff::Common::FieldVector< K, d > VectorType;
  public:
    static VectorType get(const ParameterTree& config, const std::string& key,
                          const size_t size = 0, const size_t /*cols*/ = 0)
    {
      if (size > 0 && size != d)
        DUNE_THROW_COLORFULLY(Exceptions::configuration_error,
                              "You requested a '" << Typename< VectorType >::value() << "' for key '" << key
                              << "' with a 'size' of " << size
                              << " but this type of vector can not have any size other than " << d << "!");
      return BaseType::template get_vector< VectorType, K >(config, key, d);
    }
  }; // class Choose< Dune::Stuff::Common::FieldVector< ... > >

  template< class K, int r, int c >
  class Choose< Dune::FieldMatrix< K, r, c > >
    : ChooseBase< Dune::FieldMatrix< K, r, c > >
  {
    typedef ChooseBase< Dune::FieldMatrix< K, r, c > > BaseType;
    typedef Dune::FieldMatrix< K, r, c > MatrixType;
  public:
    static MatrixType get(const ParameterTree& config, const std::string& key,
                          const size_t size = 0, const size_t cols = 0)
    {
      if ((size > 0 && size != r) || (cols > 0 && cols != c))
        DUNE_THROW_COLORFULLY(Exceptions::configuration_error,
                              "You requested a '" << Typename< MatrixType >::value() << "' for key '" << key
                              << "' with a size of " << size << "x" << cols
                              << " but this type of matrix can not have any size other than " << r << "x" << c << "!");
      return BaseType::template get_matrix< MatrixType, K >(config, key, r, c);
    }
  }; // class Choose< Dune::FieldMatrix< ... > >

  template< class K, int r >
  class Choose< Dune::FieldMatrix< K, r, 1 > >
    : ChooseBase< Dune::FieldMatrix< K, r, 1 > >
  {
    static const int c = 1;
    typedef ChooseBase< Dune::FieldMatrix< K, r, c > > BaseType;
    typedef Dune::FieldMatrix< K, r, c > MatrixType;
  public:
    static MatrixType get(const ParameterTree& config, const std::string& key, const size_t size = 0, const size_t cols = 0)
    {
      if ((size > 0 && size != r) || (cols > 0 && cols != c))
        DUNE_THROW_COLORFULLY(Exceptions::configuration_error,
                              "You requested a '" << Typename< MatrixType >::value() << "' for key '" << key
                              << "' with a size of " << size << "x" << cols
                              << " but this type of matrix can not have any size other than " << r << "x" << c << "!");
      auto vector = BaseType::template get_vector< Dune::FieldVector< K, r >, K >(config, key, r);
      MatrixType ret;
      for (size_t ii = 0; ii < r; ++ii)
        ret[ii][0] = vector[ii];
      return ret;
    }
  }; // class Choose< Dune::FieldMatrix< ..., 1 > >

  template< class K, int c >
  class Choose< Dune::FieldMatrix< K, 1, c > >
    : ChooseBase< Dune::FieldMatrix< K, 1, c > >
  {
    static const int r = 1;
    typedef ChooseBase< Dune::FieldMatrix< K, r, c > > BaseType;
    typedef Dune::FieldMatrix< K, r, c > MatrixType;
  public:
    static MatrixType get(const ParameterTree& config, const std::string& key, const size_t size = 0, const size_t cols = 0)
    {
      if ((size > 0 && size != r) || (cols > 0 && cols != c))
        DUNE_THROW_COLORFULLY(Exceptions::configuration_error,
                              "You requested a '" << Typename< MatrixType >::value() << "' for key '" << key
                              << "' with a size of " << size << "x" << cols
                              << " but this type of matrix can not have any size other than " << r << "x" << c << "!");
      auto vector = BaseType::template get_vector< Dune::FieldVector< K, c >, K >(config, key, c);
      MatrixType ret;
      for (size_t ii = 0; ii < c; ++ii)
        ret[0][ii] = vector[ii];
      return ret;
    }
  }; // class Choose< Dune::FieldMatrix< ..., 1, ... > >

  template< class K, int r, int c >
  class Choose< Dune::Stuff::Common::FieldMatrix< K, r, c > >
    : ChooseBase< Dune::Stuff::Common::FieldMatrix< K, r, c > >
  {
    typedef ChooseBase< Dune::Stuff::Common::FieldMatrix< K, r, c > > BaseType;
    typedef Dune::Stuff::Common::FieldMatrix< K, r, c > MatrixType;
  public:
    static MatrixType get(const ParameterTree& config, const std::string& key,
                          const size_t size = 0, const size_t cols = 0)
    {
      if ((size > 0 && size != r) || (cols > 0 && cols != c))
        DUNE_THROW_COLORFULLY(Exceptions::configuration_error,
                              "You requested a '" << Typename< MatrixType >::value() << "' for key '" << key
                              << "' with a size of " << size << "x" << cols
                              << " but this type of matrix can not have any size other than " << r << "x" << c << "!");
      return BaseType::template get_matrix< MatrixType, K >(config, key, r, c);
    }
  }; // class Choose< Dune::Stuff::Common::FieldMatrix< ... > >

  template< class T >
  class Choose< DynamicVector< T > >
    : ChooseBase< DynamicVector< T > >
  {
    typedef ChooseBase< DynamicVector< T > > BaseType;
    typedef DynamicVector< T > VectorType;
  public:
    static VectorType get(const ParameterTree& config, const std::string& key,
                          const size_t size = 0, const size_t /*cols*/ = 0)
    {
      return BaseType::template get_vector< VectorType, T >(config, key, size);
    }
  }; // class Choose< DynamicVector< ... > >

  template< class T >
  class Choose< DynamicMatrix< T > >
    : ChooseBase< DynamicMatrix< T > >
  {
    typedef ChooseBase< DynamicMatrix< T > > BaseType;
    typedef DynamicMatrix< T > MatrixType;
  public:
    static MatrixType get(const ParameterTree& config,
                          const std::string& key,
                          const size_t rows = 0,
                          const size_t cols = 0)
    {
      return BaseType::template get_matrix< MatrixType, T >(config, key, rows, cols);
    }
  }; // class Choose< DynamicVector< ... > >

  template< class S >
  class Choose< LA::CommonDenseVector< S > >
    : ChooseBase< LA::CommonDenseVector< S > >
  {
    typedef ChooseBase< LA::CommonDenseVector< S > > BaseType;
    typedef LA::CommonDenseVector< S > VectorType;
  public:
    static VectorType get(const ParameterTree& config, const std::string& key,
                          const size_t size = 0, const size_t /*cols*/ = 0)
    {
      return BaseType::template get_vector< VectorType, S >(config, key, size);
    }
  }; // class Choose< LA::CommonDenseVector< ... > >

  template< class S >
  class Choose< LA::CommonDenseMatrix< S > >
    : ChooseBase< LA::CommonDenseMatrix< S > >
  {
    typedef ChooseBase< LA::CommonDenseMatrix< S > > BaseType;
    typedef LA::CommonDenseMatrix< S > MatrixType;
  public:
    static MatrixType get(const ParameterTree& config, const std::string& key,
                          const size_t size = 0, const size_t cols = 0)
    {
      return BaseType::template get_matrix< MatrixType, S >(config, key, size, cols);
    }
  }; // class Choose< LA::CommonDenseMatrix< ... > >

#if HAVE_DUNE_ISTL
  template< class S >
  class Choose< LA::IstlDenseVector< S > >
    : ChooseBase< LA::IstlDenseVector< S > >
  {
    typedef ChooseBase< LA::IstlDenseVector< S > > BaseType;
    typedef LA::IstlDenseVector< S > VectorType;
  public:
    static VectorType get(const ParameterTree& config, const std::string& key,
                          const size_t size = 0, const size_t /*cols*/ = 0)
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
    static VectorType get(const ParameterTree& config, const std::string& key,
                          const size_t size = 0, const size_t /*cols*/ = 0)
    {
      return BaseType::template get_vector< VectorType, S >(config, key, size);
    }
  }; // class Choose< LA::EigenDenseVector< ... > >

  template< class S >
  class Choose< LA::EigenDenseMatrix< S > >
    : ChooseBase< LA::EigenDenseMatrix< S > >
  {
    typedef ChooseBase< LA::EigenDenseMatrix< S > > BaseType;
    typedef LA::EigenDenseMatrix< S > MatrixType;
  public:
    static MatrixType get(const ParameterTree& config, const std::string& key,
                          const size_t size = 0, const size_t cols = 0)
    {
      return BaseType::template get_matrix< MatrixType, S >(config, key, size, cols);
    }
  }; // class Choose< LA::EigenDenseMatrix< ... > >

  template< class S >
  class Choose< LA::EigenMappedDenseVector< S > >
    : ChooseBase< LA::EigenMappedDenseVector< S > >
  {
    typedef ChooseBase< LA::EigenMappedDenseVector< S > > BaseType;
    typedef LA::EigenMappedDenseVector< S > VectorType;
  public:
    static VectorType get(const ParameterTree& config, const std::string& key,
                          const size_t size = 0, const size_t /*cols*/ = 0)
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
    set(key, value);
  }

  template< class T >
  ConfigTree(const std::string key, const char* value)
    : BaseType()
  {
    set(key, value);
  }

  template< class T >
  ConfigTree(const std::vector< std::string > keys, const std::vector< T > values)
    : BaseType()
  {
    if (keys.size() != values.size())
      DUNE_THROW_COLORFULLY(Exceptions::shapes_do_not_match,
                            "The size of 'keys' (" << keys.size() << ") does not match the size of 'values' ("
                            << values.size() << ")!");
    for (size_t ii = 0; ii < keys.size(); ++ii)
      set(keys[ii], values[ii]);
  }

  ConfigTree(const std::vector< std::string > keys, const std::vector< std::string > values)
    : BaseType()
  {
    if (keys.size() != values.size())
      DUNE_THROW_COLORFULLY(Exceptions::shapes_do_not_match,
                            "The size of 'keys' (" << keys.size() << ") does not match the size of 'values' ("
                            << values.size() << ")!");
    for (size_t ii = 0; ii < keys.size(); ++ii)
      set(keys[ii], values[ii]);
  }

  template< class T >
  ConfigTree(const std::vector< std::string > keys, const std::initializer_list< T > value_list)
    : BaseType()
  {
    std::vector< T > values(value_list);
    if (keys.size() != values.size())
      DUNE_THROW_COLORFULLY(Exceptions::shapes_do_not_match,
                            "The size of 'keys' (" << keys.size() << ") does not match the size of 'value_list' ("
                            << values.size() << ")!");
    for (size_t ii = 0; ii < keys.size(); ++ii)
      set(keys[ii], values[ii]);
  }

  explicit ConfigTree(const std::string filename)
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
          DUNE_THROW_COLORFULLY(Exceptions::configuration_error,
                                "While adding 'other' to this (see below), the key '" << key
                                << "' already exists and you requested no overwrite!"
                                << "\n==== this ============\n" << report_string()
                                << "\n==== other ===========\n" << other.report_string());
        set(key, other.get< std::string >(key));
      }
    } else {
      if (has_key(sub_id) && !overwrite)
        DUNE_THROW_COLORFULLY(Exceptions::configuration_error,
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

  ConfigTree& operator+=(const ConfigTree& other)
  {
    add(other);
    return *this;
  }

  ConfigTree operator+(const ConfigTree& other) const
  {
    ConfigTree ret(*this);
    ret += other;
    return ret;
  }

  template< typename T >
  void set(const std::string& key, const T& value, const bool overwrite = false)
  {
    if (has_key(key) && !overwrite)
      DUNE_THROW_COLORFULLY(Exceptions::configuration_error,
                            "While adding '" << key << "' = '" << value << "' to this (see below), the key '" << key
                            << "' already exists and you requested no overwrite!"
                            << "\n======================\n" << report_string());
    const std::string value_string = toString(value);
    BaseType::operator[](key) = value_string;
  } // ... set(...)

  void set(const std::string& key, const char* value, const bool overwrite = false)
  {
    if (has_key(key) && !overwrite)
      DUNE_THROW_COLORFULLY(Exceptions::configuration_error,
                            "While adding '" << key << "' = '" << value << "' to this (see below), the key '" << key
                            << "' already exists and you requested no overwrite!"
                            << "\n======================\n" << report_string());
    const std::string value_string = toString(value);
    BaseType::operator[](key) = value_string;
  } // ... set(...)

  bool has_sub(const std::string& sub_id) const
  {
    return BaseType::hasSub(sub_id);
  }

  ConfigTree sub(const std::string& sub_id) const
  {
    if (empty())
      DUNE_THROW_COLORFULLY(Exceptions::configuration_error,
                            "You can not get anything from an empty ConfigTree, use has_sub(\"" << sub_id
                            << "\") to check first!");
    if (sub_id.empty())
      DUNE_THROW_COLORFULLY(Exceptions::configuration_error, "Given sub_id must not be empty!");
    if (!has_sub(sub_id))
      DUNE_THROW_COLORFULLY(Exceptions::configuration_error,
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

  /**
   * \param size Determines the size of the returning container (size if T is a vector type, rows if T is a matrix type, 0 means automatic).
   * \param cols Determines the number of columns of the returning matrix if T is a matrix type (0 means automatic, ignored, if T is a vector type).
   */
  template< typename T >
  T get(const std::string& key, const T& default_value, const size_t size = 0, const size_t cols = 0) const
  {
    if (has_key(key))
      return Choose< T >::get(*this, key, size, cols);
    else
      return default_value;
  } // ... get(...)

  std::string get(const std::string& key, const char* default_value, const size_t /*size*/ = 0) const
  {
    return BaseType::get(key, default_value);
  }

  /**
   * \param size Determines the size of the returning container (size if T is a vector type, rows if T is a matrix type, 0 means automatic).
   * \param cols Determines the number of columns of the returning matrix if T is a matrix type (0 means automatic, ignored, if T is a vector type).
   */
  template< typename T >
  T get(const std::string& key, const size_t size = 0, const size_t cols = 0) const
  {
    if (empty())
      DUNE_THROW_COLORFULLY(Exceptions::configuration_error,
                            "You can not get anything from an empty ConfigTree, use has_key(\"" << key
                            << "\") to check first!");
    if (!has_key(key))
      DUNE_THROW_COLORFULLY(Exceptions::configuration_error,
                            "Given key '" << key << "' does not exist in this ConfigTree (see below), use has_key(\""
                            << key
                            << "\") to check first!"
                            << "\n======================\n" << report_string());
    return Choose< T >::get(*this, key, size, cols);
  } // ... get(...)

  void report(std::ostream& out = std::cout, const std::string& prefix = "") const
  {
    if (!empty()) {
      if (subKeys.size() == 0) {
        report_as_sub(out, prefix, "");
      } else if (valueKeys.size() == 0) {
        const std::string common_prefix = find_common_prefix(*this, "");
        if (!common_prefix.empty()) {
          out << prefix << "[" << common_prefix << "]" << std::endl;
          const ConfigTree& commonSub = sub(common_prefix);
          report_flatly(commonSub, prefix, out);
        } else
          report_as_sub(out, prefix, "");
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


inline std::ostream& operator<<(std::ostream& out, const ConfigTree& config)
{
  config.report(out);
  return out;
}


} // namespace Common
} // namespace Stuff
} // namespace Dune

#endif // DUNE_STUFF_CONFIGTREE_HH
