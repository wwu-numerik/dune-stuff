// This file is part of the dune-stuff project:
//   https://users.dune-project.org/projects/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

// This one has to come first (includes the config.h)!
#include <dune/stuff/test/test_common.hh>

#include <dune/stuff/common/configtree.hh>
#include <dune/stuff/common/logging.hh>
#include <dune/stuff/la/container.hh>
#include <dune/stuff/common/exceptions.hh>
#include <dune/stuff/common/type_utils.hh>
#include <dune/stuff/common/float_cmp.hh>

// uncomment this for output
//std::ostream& test_out = std::cout;
std::ostream& test_out = DSC_LOG.devnull();

using namespace Dune;
using Dune::Stuff::Common::ConfigTree;
using Dune::Stuff::Exceptions::results_are_not_as_expected;
using namespace Dune::Stuff::Common::FloatCmp;


struct CreateByOperator { static ConfigTree create() {
    ConfigTree config;
    config["string"] = "string";
    config["sub1.int"] = "1";
    config["sub2.size_t"] = "1";
    config["sub2.subsub1.vector"] = "[0 1]";
    config["sub2.subsub1.matrix"] = "[0 1; 1 2]";
    return config;
} };

struct CreateByKeyAndValueAndAdd { static ConfigTree create() {
    ConfigTree config("string", "string");
    config.set("sub1.int", "1");
    config.set("sub2.size_t", 1);
    config.add(ConfigTree("vector", "[0 1]"), "sub2.subsub1");
    config.add(ConfigTree("matrix", "[0 1; 1 2]"), "sub2.subsub1");
    return config;
} };

struct CreateByKeysAndValues { static ConfigTree create() {
    return ConfigTree({"string", "sub1.int", "sub2.size_t", "sub2.subsub1.vector", "sub2.subsub1.matrix"},
                      {"string", "1",        "1",           "[0 1]",               "[0 1; 1 2]"});
} };


typedef testing::Types< CreateByOperator, CreateByKeyAndValueAndAdd, CreateByKeysAndValues > ConfigTreeCreators;

template< class ConfigTreeCreator >
struct ConfigTreeTest
  : public ::testing::Test
{

  template< class VectorType >
  static void check_vector(const ConfigTree& config)
  {
    VectorType vec = config.get("vector", VectorType(), 1);
    if (vec.size() != 1) DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                                               vec.size() << " vs. 1 with VectorType = "
                                               << Stuff::Common::Typename< VectorType >::value());
    if (FloatCmp::ne(vec[0], 0.0)) DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                                                         vec[0] << " vs. 0 with VectorType = "
                                                                << Stuff::Common::Typename< VectorType >::value());
    vec = config.get("vector", VectorType(), 2);
    if (vec.size() != 2) DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                                               vec.size() << " vs. 2 with VectorType = "
                                               << Stuff::Common::Typename< VectorType >::value());
    for (auto ii : {0.0, 1.0})
      if (FloatCmp::ne(vec[ii], ii)) DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                                                           vec[ii] << " vs. " << ii << " with VectorType = "
                                                           << Stuff::Common::Typename< VectorType >::value());

    vec = config.get< VectorType >("vector", 1);
    if (vec.size() != 1) DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                                               vec.size() << " vs. 1 with VectorType = "
                                               << Stuff::Common::Typename< VectorType >::value());
    if (FloatCmp::ne(vec[0], 0.0)) DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                                                         vec[0] << " vs. 0 with VectorType = "
                                                                << Stuff::Common::Typename< VectorType >::value());
    vec = config.get< VectorType >("vector", 2);
    if (vec.size() != 2) DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                                               vec.size() << " vs. 2 with VectorType = "
                                               << Stuff::Common::Typename< VectorType >::value());
    for (auto ii : {0.0, 1.0})
      if (FloatCmp::ne(vec[ii], ii)) DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                                                           vec[ii] << " vs. " << ii << " with VectorType = "
                                                           << Stuff::Common::Typename< VectorType >::value());

    vec = config.get< VectorType >("vector");
    if (vec.size() != 2) DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                                               vec.size() << " vs. 2 with VectorType = "
                                               << Stuff::Common::Typename< VectorType >::value());
    for (auto ii : {0.0, 1.0})
      if (FloatCmp::ne(vec[ii], ii)) DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                                               vec[ii] << " vs. " << ii << " with VectorType = "
                                               << Stuff::Common::Typename< VectorType >::value());
  } // ... check_vector< ... >(...)

  template< class K, int d >
  static void check_field_vector(const ConfigTree& config)
  {
    typedef FieldVector< K, d > VectorType;
    VectorType vec = config.get("vector", VectorType(), d);
    if (vec.size() != d) DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                                               vec.size() << " vs. 2 with VectorType = "
                                               << Stuff::Common::Typename< VectorType >::value());
    for (size_t ii = 0; ii < d; ++ii)
      if (FloatCmp::ne(vec[ii], double(ii)))
        DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                              vec[ii] << " vs. " << ii << " with VectorType = "
                              << Stuff::Common::Typename< VectorType >::value());

    vec = config.get< VectorType >("vector", d);
    if (vec.size() != d) DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                                               vec.size() << " vs. 2 with VectorType = "
                                               << Stuff::Common::Typename< VectorType >::value());
    for (size_t ii = 0; ii < d; ++ii)
      if (FloatCmp::ne(vec[ii], double(ii)))
        DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                              vec[ii] << " vs. " << ii << " with VectorType = "
                              << Stuff::Common::Typename< VectorType >::value());

    vec = config.get< VectorType >("vector");
    if (vec.size() != d) DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                                               vec.size() << " vs. 2 with VectorType = "
                                               << Stuff::Common::Typename< VectorType >::value());
    for (size_t ii = 0; ii < d; ++ii)
      if (FloatCmp::ne(vec[ii], double(ii)))
        DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                              vec[ii] << " vs. " << ii << " with VectorType = "
                              << Stuff::Common::Typename< VectorType >::value());
  } // ... check_field_vector< ... >(...)

  template< class MatrixType >
  static void check_matrix(const ConfigTree& config)
  {
    MatrixType mat = config.get("matrix", MatrixType(), 1, 1);
    if (mat.rows() != 1 || mat.cols() != 1)
      DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                            mat.rows() << " vs. 1 and " << mat.cols() << "vs. 1 with MatrixType = "
                            << Stuff::Common::Typename< MatrixType >::value());
    if (FloatCmp::ne(mat[0][0], 0.0)) DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                                                            mat[0][0] << " vs. 0 with MatrixType = "
                                                                      << Stuff::Common::Typename< MatrixType >::value());
    mat = config.get("matrix", MatrixType(), 1, 2);
    if (mat.rows() != 1 || mat.cols() != 2)
      DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                            mat.rows() << " vs. 1 and " << mat.cols() << "vs. 2 with MatrixType = "
                            << Stuff::Common::Typename< MatrixType >::value());
    if (FloatCmp::ne(mat[0][0], 0.0)) DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                                                            mat[0][0] << " vs. 0 with MatrixType = "
                                                                      << Stuff::Common::Typename< MatrixType >::value());
    if (FloatCmp::ne(mat[0][1], 1.0)) DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                                                            mat[0][1] << " vs. 1 with MatrixType = "
                                                                      << Stuff::Common::Typename< MatrixType >::value());
    mat = config.get("matrix", MatrixType(), 2, 1);
    if (mat.rows() != 2 || mat.cols() != 1)
      DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                            mat.rows() << " vs. 2 and " << mat.cols() << "vs. 1 with MatrixType = "
                            << Stuff::Common::Typename< MatrixType >::value());
    if (FloatCmp::ne(mat[0][0], 0.0)) DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                                                            mat[0][0] << " vs. 0 with MatrixType = "
                                                                      << Stuff::Common::Typename< MatrixType >::value());
    if (FloatCmp::ne(mat[1][0], 1.0)) DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                                                            mat[1][0] << " vs. 1 with MatrixType = "
                                                                      << Stuff::Common::Typename< MatrixType >::value());
    mat = config.get("matrix", MatrixType(), 2, 2);
    if (mat.rows() != 2 || mat.cols() != 2)
      DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                            mat.rows() << " vs. 2 and " << mat.cols() << "vs. 2 with MatrixType = "
                            << Stuff::Common::Typename< MatrixType >::value());
    if (FloatCmp::ne(mat[0][0], 0.0)) DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                                                            mat[0][0] << " vs. 0 with MatrixType = "
                                                                      << Stuff::Common::Typename< MatrixType >::value());
    if (FloatCmp::ne(mat[0][1], 1.0)) DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                                                            mat[0][1] << " vs. 1 with MatrixType = "
                                                                      << Stuff::Common::Typename< MatrixType >::value());
    if (FloatCmp::ne(mat[1][0], 1.0)) DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                                                            mat[1][0] << " vs. 1 with MatrixType = "
                                                                      << Stuff::Common::Typename< MatrixType >::value());
    if (FloatCmp::ne(mat[1][1], 2.0)) DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                                                            mat[1][1] << " vs. 2 with MatrixType = "
                                                                      << Stuff::Common::Typename< MatrixType >::value());
    mat = config.get("matrix", MatrixType());
    if (mat.rows() != 2 || mat.cols() != 2)
      DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                            mat.rows() << " vs. 2 and " << mat.cols() << "vs. 2 with MatrixType = "
                            << Stuff::Common::Typename< MatrixType >::value());
    if (FloatCmp::ne(mat[0][0], 0.0)) DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                                                            mat[0][0] << " vs. 0 with MatrixType = "
                                                                      << Stuff::Common::Typename< MatrixType >::value());
    if (FloatCmp::ne(mat[0][1], 1.0)) DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                                                            mat[0][1] << " vs. 1 with MatrixType = "
                                                                      << Stuff::Common::Typename< MatrixType >::value());
    if (FloatCmp::ne(mat[1][0], 1.0)) DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                                                            mat[1][0] << " vs. 1 with MatrixType = "
                                                                      << Stuff::Common::Typename< MatrixType >::value());
    if (FloatCmp::ne(mat[1][1], 2.0)) DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                                                            mat[1][1] << " vs. 2 with MatrixType = "
                                                                      << Stuff::Common::Typename< MatrixType >::value());

    mat = config.get< MatrixType >("matrix", 1, 1);
    if (mat.rows() != 1 || mat.cols() != 1)
      DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                            mat.rows() << " vs. 1 and " << mat.cols() << "vs. 1 with MatrixType = "
                            << Stuff::Common::Typename< MatrixType >::value());
    if (FloatCmp::ne(mat[0][0], 0.0)) DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                                                            mat[0][0] << " vs. 0 with MatrixType = "
                                                                      << Stuff::Common::Typename< MatrixType >::value());
    mat = config.get< MatrixType >("matrix", 1, 2);
    if (mat.rows() != 1 || mat.cols() != 2)
      DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                            mat.rows() << " vs. 1 and " << mat.cols() << "vs. 2 with MatrixType = "
                            << Stuff::Common::Typename< MatrixType >::value());
    if (FloatCmp::ne(mat[0][0], 0.0)) DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                                                            mat[0][0] << " vs. 0 with MatrixType = "
                                                                      << Stuff::Common::Typename< MatrixType >::value());
    if (FloatCmp::ne(mat[0][1], 1.0)) DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                                                            mat[0][1] << " vs. 1 with MatrixType = "
                                                                      << Stuff::Common::Typename< MatrixType >::value());
    mat = config.get< MatrixType >("matrix", 2, 1);
    if (mat.rows() != 2 || mat.cols() != 1)
      DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                            mat.rows() << " vs. 2 and " << mat.cols() << "vs. 1 with MatrixType = "
                            << Stuff::Common::Typename< MatrixType >::value());
    if (FloatCmp::ne(mat[0][0], 0.0)) DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                                                            mat[0][0] << " vs. 0 with MatrixType = "
                                                                      << Stuff::Common::Typename< MatrixType >::value());
    if (FloatCmp::ne(mat[1][0], 1.0)) DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                                                            mat[1][0] << " vs. 1 with MatrixType = "
                                                                      << Stuff::Common::Typename< MatrixType >::value());
    mat = config.get< MatrixType >("matrix", 2, 2);
    if (mat.rows() != 2 || mat.cols() != 2)
      DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                            mat.rows() << " vs. 2 and " << mat.cols() << "vs. 2 with MatrixType = "
                            << Stuff::Common::Typename< MatrixType >::value());
    if (FloatCmp::ne(mat[0][0], 0.0)) DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                                                            mat[0][0] << " vs. 0 with MatrixType = "
                                                                      << Stuff::Common::Typename< MatrixType >::value());
    if (FloatCmp::ne(mat[0][1], 1.0)) DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                                                            mat[0][1] << " vs. 1 with MatrixType = "
                                                                      << Stuff::Common::Typename< MatrixType >::value());
    if (FloatCmp::ne(mat[1][0], 1.0)) DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                                                            mat[1][0] << " vs. 1 with MatrixType = "
                                                                      << Stuff::Common::Typename< MatrixType >::value());
    if (FloatCmp::ne(mat[1][1], 2.0)) DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                                                            mat[1][1] << " vs. 2 with MatrixType = "
                                                                      << Stuff::Common::Typename< MatrixType >::value());
    mat = config.get< MatrixType >("matrix");
    if (mat.rows() != 2 || mat.cols() != 2)
      DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                            mat.rows() << " vs. 2 and " << mat.cols() << "vs. 2 with MatrixType = "
                            << Stuff::Common::Typename< MatrixType >::value());
    if (FloatCmp::ne(mat[0][0], 0.0)) DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                                                            mat[0][0] << " vs. 0 with MatrixType = "
                                                                      << Stuff::Common::Typename< MatrixType >::value());
    if (FloatCmp::ne(mat[0][1], 1.0)) DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                                                            mat[0][1] << " vs. 1 with MatrixType = "
                                                                      << Stuff::Common::Typename< MatrixType >::value());
    if (FloatCmp::ne(mat[1][0], 1.0)) DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                                                            mat[1][0] << " vs. 1 with MatrixType = "
                                                                      << Stuff::Common::Typename< MatrixType >::value());
    if (FloatCmp::ne(mat[1][1], 2.0)) DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                                                            mat[1][1] << " vs. 2 with MatrixType = "
                                                                      << Stuff::Common::Typename< MatrixType >::value());
  } // ... check_matrix< ... >(...)

  template< class MatrixType >
  static void check_stuff_matrix(const ConfigTree& config)
  {
    MatrixType mat = config.get("matrix", MatrixType(), 1, 1);
    if (mat.rows() != 1 || mat.cols() != 1)
      DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                            mat.rows() << " vs. 1 and " << mat.cols() << "vs. 1 with MatrixType = "
                            << Stuff::Common::Typename< MatrixType >::value());
    if (FloatCmp::ne(mat.get_entry(0, 0), 0.0))
      DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                            mat.get_entry(0, 0) << " vs. 0 with MatrixType = "
                            << Stuff::Common::Typename< MatrixType >::value());
    mat = config.get("matrix", MatrixType(), 1, 2);
    if (mat.rows() != 1 || mat.cols() != 2)
      DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                            mat.rows() << " vs. 1 and " << mat.cols() << "vs. 2 with MatrixType = "
                            << Stuff::Common::Typename< MatrixType >::value());
    if (FloatCmp::ne(mat.get_entry(0, 0), 0.0))
      DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                            mat.get_entry(0, 0) << " vs. 0 with MatrixType = "
                            << Stuff::Common::Typename< MatrixType >::value());
    if (FloatCmp::ne(mat.get_entry(0, 1), 1.0))
      DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                            mat.get_entry(0, 1) << " vs. 1 with MatrixType = "
                            << Stuff::Common::Typename< MatrixType >::value());
    mat = config.get("matrix", MatrixType(), 2, 1);
    if (mat.rows() != 2 || mat.cols() != 1)
      DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                            mat.rows() << " vs. 2 and " << mat.cols() << "vs. 1 with MatrixType = "
                            << Stuff::Common::Typename< MatrixType >::value());
    if (FloatCmp::ne(mat.get_entry(0, 0), 0.0)) DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                                                            mat.get_entry(0, 0) << " vs. 0 with MatrixType = "
                                                                      << Stuff::Common::Typename< MatrixType >::value());
    if (FloatCmp::ne(mat.get_entry(1, 0), 1.0)) DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                                                            mat.get_entry(1, 0) << " vs. 1 with MatrixType = "
                                                                      << Stuff::Common::Typename< MatrixType >::value());
    mat = config.get("matrix", MatrixType(), 2, 2);
    if (mat.rows() != 2 || mat.cols() != 2)
      DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                            mat.rows() << " vs. 2 and " << mat.cols() << "vs. 2 with MatrixType = "
                            << Stuff::Common::Typename< MatrixType >::value());
    if (FloatCmp::ne(mat.get_entry(0, 0), 0.0)) DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                                                            mat.get_entry(0, 0) << " vs. 0 with MatrixType = "
                                                                      << Stuff::Common::Typename< MatrixType >::value());
    if (FloatCmp::ne(mat.get_entry(0, 1), 1.0)) DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                                                            mat.get_entry(0, 1) << " vs. 1 with MatrixType = "
                                                                      << Stuff::Common::Typename< MatrixType >::value());
    if (FloatCmp::ne(mat.get_entry(1, 0), 1.0)) DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                                                            mat.get_entry(1, 0) << " vs. 1 with MatrixType = "
                                                                      << Stuff::Common::Typename< MatrixType >::value());
    if (FloatCmp::ne(mat.get_entry(1, 1), 2.0)) DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                                                            mat.get_entry(1, 1) << " vs. 2 with MatrixType = "
                                                                      << Stuff::Common::Typename< MatrixType >::value());
    mat = config.get("matrix", MatrixType());
    if (mat.rows() != 2 || mat.cols() != 2)
      DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                            mat.rows() << " vs. 2 and " << mat.cols() << "vs. 2 with MatrixType = "
                            << Stuff::Common::Typename< MatrixType >::value());
    if (FloatCmp::ne(mat.get_entry(0, 0), 0.0)) DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                                                            mat.get_entry(0, 0) << " vs. 0 with MatrixType = "
                                                                      << Stuff::Common::Typename< MatrixType >::value());
    if (FloatCmp::ne(mat.get_entry(0, 1), 1.0)) DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                                                            mat.get_entry(0, 1) << " vs. 1 with MatrixType = "
                                                                      << Stuff::Common::Typename< MatrixType >::value());
    if (FloatCmp::ne(mat.get_entry(1, 0), 1.0)) DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                                                            mat.get_entry(1, 0) << " vs. 1 with MatrixType = "
                                                                      << Stuff::Common::Typename< MatrixType >::value());
    if (FloatCmp::ne(mat.get_entry(1, 1), 2.0)) DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                                                            mat.get_entry(1, 1) << " vs. 2 with MatrixType = "
                                                                      << Stuff::Common::Typename< MatrixType >::value());

    mat = config.get< MatrixType >("matrix", 1, 1);
    if (mat.rows() != 1 || mat.cols() != 1)
      DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                            mat.rows() << " vs. 1 and " << mat.cols() << "vs. 1 with MatrixType = "
                            << Stuff::Common::Typename< MatrixType >::value());
    if (FloatCmp::ne(mat.get_entry(0, 0), 0.0)) DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                                                            mat.get_entry(0, 0) << " vs. 0 with MatrixType = "
                                                                      << Stuff::Common::Typename< MatrixType >::value());
    mat = config.get< MatrixType >("matrix", 1, 2);
    if (mat.rows() != 1 || mat.cols() != 2)
      DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                            mat.rows() << " vs. 1 and " << mat.cols() << "vs. 2 with MatrixType = "
                            << Stuff::Common::Typename< MatrixType >::value());
    if (FloatCmp::ne(mat.get_entry(0, 0), 0.0)) DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                                                            mat.get_entry(0, 0) << " vs. 0 with MatrixType = "
                                                                      << Stuff::Common::Typename< MatrixType >::value());
    if (FloatCmp::ne(mat.get_entry(0, 1), 1.0)) DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                                                            mat.get_entry(0, 1) << " vs. 1 with MatrixType = "
                                                                      << Stuff::Common::Typename< MatrixType >::value());
    mat = config.get< MatrixType >("matrix", 2, 1);
    if (mat.rows() != 2 || mat.cols() != 1)
      DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                            mat.rows() << " vs. 2 and " << mat.cols() << "vs. 1 with MatrixType = "
                            << Stuff::Common::Typename< MatrixType >::value());
    if (FloatCmp::ne(mat.get_entry(0, 0), 0.0)) DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                                                            mat.get_entry(0, 0) << " vs. 0 with MatrixType = "
                                                                      << Stuff::Common::Typename< MatrixType >::value());
    if (FloatCmp::ne(mat.get_entry(1, 0), 1.0)) DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                                                            mat.get_entry(1, 0) << " vs. 1 with MatrixType = "
                                                                      << Stuff::Common::Typename< MatrixType >::value());
    mat = config.get< MatrixType >("matrix", 2, 2);
    if (mat.rows() != 2 || mat.cols() != 2)
      DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                            mat.rows() << " vs. 2 and " << mat.cols() << "vs. 2 with MatrixType = "
                            << Stuff::Common::Typename< MatrixType >::value());
    if (FloatCmp::ne(mat.get_entry(0, 0), 0.0)) DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                                                            mat.get_entry(0, 0) << " vs. 0 with MatrixType = "
                                                                      << Stuff::Common::Typename< MatrixType >::value());
    if (FloatCmp::ne(mat.get_entry(0, 1), 1.0)) DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                                                            mat.get_entry(0, 1) << " vs. 1 with MatrixType = "
                                                                      << Stuff::Common::Typename< MatrixType >::value());
    if (FloatCmp::ne(mat.get_entry(1, 0), 1.0)) DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                                                            mat.get_entry(1, 0) << " vs. 1 with MatrixType = "
                                                                      << Stuff::Common::Typename< MatrixType >::value());
    if (FloatCmp::ne(mat.get_entry(1, 1), 2.0)) DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                                                            mat.get_entry(1, 1) << " vs. 2 with MatrixType = "
                                                                      << Stuff::Common::Typename< MatrixType >::value());
    mat = config.get< MatrixType >("matrix");
    if (mat.rows() != 2 || mat.cols() != 2)
      DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                            mat.rows() << " vs. 2 and " << mat.cols() << "vs. 2 with MatrixType = "
                            << Stuff::Common::Typename< MatrixType >::value());
    if (FloatCmp::ne(mat.get_entry(0, 0), 0.0)) DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                                                            mat.get_entry(0, 0) << " vs. 0 with MatrixType = "
                                                                      << Stuff::Common::Typename< MatrixType >::value());
    if (FloatCmp::ne(mat.get_entry(0, 1), 1.0)) DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                                                            mat.get_entry(0, 1) << " vs. 1 with MatrixType = "
                                                                      << Stuff::Common::Typename< MatrixType >::value());
    if (FloatCmp::ne(mat.get_entry(1, 0), 1.0)) DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                                                            mat.get_entry(1, 0) << " vs. 1 with MatrixType = "
                                                                      << Stuff::Common::Typename< MatrixType >::value());
    if (FloatCmp::ne(mat.get_entry(1, 1), 2.0)) DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                                                            mat.get_entry(1, 1) << " vs. 2 with MatrixType = "
                                                                      << Stuff::Common::Typename< MatrixType >::value());
  } // ... check_stuff_matrix< ... >(...)

  template< class K, int r, int c >
  static void check_field_matrix(const ConfigTree& config)
  {
    typedef FieldMatrix< K, r, c > MatrixType;
    MatrixType mat = config.get("matrix", MatrixType(), r, c);
    for (size_t cc = 0; cc < c; ++cc) {
      for (size_t rr = 0; rr < r; ++rr) {
        if (FloatCmp::ne(mat[rr][cc], double(rr + cc)))
          DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                                mat[rr][cc] << " vs. " << rr + cc << " with MatrixType = "
                                << Stuff::Common::Typename< MatrixType >::value());
      }
    }
    mat = config.get("matrix", MatrixType());
    for (size_t cc = 0; cc < c; ++cc) {
      for (size_t rr = 0; rr < r; ++rr) {
        if (FloatCmp::ne(mat[rr][cc], double(rr + cc)))
          DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                                mat[rr][cc] << " vs. " << rr + cc << " with MatrixType = "
                                << Stuff::Common::Typename< MatrixType >::value());
      }
    }

    mat = config.get< MatrixType >("matrix", r, c);
    for (size_t cc = 0; cc < c; ++cc) {
      for (size_t rr = 0; rr < r; ++rr) {
        if (FloatCmp::ne(mat[rr][cc], double(rr + cc)))
          DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                                mat[rr][cc] << " vs. " << rr + cc << " with MatrixType = "
                                << Stuff::Common::Typename< MatrixType >::value());
      }
    }
    mat = config.get< MatrixType >("matrix");
    for (size_t cc = 0; cc < c; ++cc) {
      for (size_t rr = 0; rr < r; ++rr) {
        if (FloatCmp::ne(mat[rr][cc], double(rr + cc)))
          DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                                mat[rr][cc] << " vs. " << rr + cc << " with MatrixType = "
                                << Stuff::Common::Typename< MatrixType >::value());
      }
    }
  } // ... check_field_matrix< ... >(...)

  static void behaves_correctly()
  {
    const ConfigTree config = ConfigTreeCreator::create();
//    config.report(); // <- this works as well but will produce output
    config.report(test_out);
    config.report(test_out, "'prefix '");
    test_out << config << std::endl;
    std::string DUNE_UNUSED(report_str) = config.report_string();

    std::string str = config.get("string", std::string("foo"));
    if (str != "string") DUNE_THROW_COLORFULLY(results_are_not_as_expected, "'" << str << "'' vs. 'string'");
    str = config.get("foo", std::string("string"));
    if (str != "string") DUNE_THROW_COLORFULLY(results_are_not_as_expected, "'" << str << "'' vs. 'string'");
    str = config.get< std::string >("string");
    if (str != "string") DUNE_THROW_COLORFULLY(results_are_not_as_expected, "'" << str << "'' vs. 'string'");

    if (!config.has_sub("sub1")) DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                                                       "Sub 'sub1' does not exists in this config:\n" << config);
    ConfigTree sub1_config = config.sub("sub1");
    int nt = sub1_config.get("int", int(0));
    if (nt != 1) DUNE_THROW_COLORFULLY(results_are_not_as_expected, "'" << nt << "'' vs. '1'");
    nt = sub1_config.get("intt", int(1));
    if (nt != 1) DUNE_THROW_COLORFULLY(results_are_not_as_expected, "'" << nt << "'' vs. '1'");
    nt = sub1_config.get< int >("int");
    if (nt != 1) DUNE_THROW_COLORFULLY(results_are_not_as_expected, "'" << nt << "'' vs. '1'");

    size_t st = config.get("sub2.size_t", size_t(0));
    if (st != 1) DUNE_THROW_COLORFULLY(results_are_not_as_expected, "'" << st << "'' vs. '1'");
    st = config.get("sub2.size_tt", size_t(1));
    if (st != 1) DUNE_THROW_COLORFULLY(results_are_not_as_expected, "'" << st << "'' vs. '1'");
    st = config.get< size_t >("sub2.size_t");
    if (st != 1) DUNE_THROW_COLORFULLY(results_are_not_as_expected, "'" << st << "'' vs. '1'");

    check_vector< std::vector< double > >(config.sub("sub2.subsub1"));
    check_field_vector< double, 1 >(config.sub("sub2.subsub1"));
    check_field_vector< double, 2 >(config.sub("sub2.subsub1"));
    check_vector< DynamicVector< double > >(config.sub("sub2.subsub1"));
    check_vector< Stuff::LA::CommonDenseVector< double > >(config.sub("sub2.subsub1"));
#if HAVE_DUNE_ISTL
    check_vector< Stuff::LA::IstlDenseVector< double > >(config.sub("sub2.subsub1"));
#endif
#if HAVE_EIGEN
    check_vector< Stuff::LA::EigenDenseVector< double > >(config.sub("sub2.subsub1"));
    check_vector< Stuff::LA::EigenMappedDenseVector< double > >(config.sub("sub2.subsub1"));
#endif // HAVE_EIGEN
    check_field_matrix< double, 2, 2 >(config.sub("sub2.subsub1"));
    check_matrix< DynamicMatrix< double > >(config.sub("sub2.subsub1"));
    check_stuff_matrix< Stuff::LA::CommonDenseMatrix< double > >(config.sub("sub2.subsub1"));
    check_stuff_matrix< Stuff::LA::EigenDenseMatrix< double > >(config.sub("sub2.subsub1"));
  } // ... behaves_correctly(...)
}; // struct ConfigTreeTest

TYPED_TEST_CASE(ConfigTreeTest, ConfigTreeCreators);
TYPED_TEST(ConfigTreeTest, behaves_correctly) {
  this->behaves_correctly();
}


int main(int argc, char** argv)
{
  try {
    test_init(argc, argv);
    return RUN_ALL_TESTS();
  } catch (Dune::Exception& e) {
    std::cerr << "Dune reported error: " << e.what() << std::endl;
    std::abort();
  } catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
    std::abort();
  } catch (...) {
    std::cerr << "Unknown exception thrown!" << std::endl;
    std::abort();
  } // try
}
