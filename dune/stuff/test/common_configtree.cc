// This file is part of the dune-stuff project:
//   http://users.dune-project.org/projects/dune-stuff/
// Copyright Holders: Felix Albrecht, Rene Milk
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

// This one has to come first (includes the config.h)!
#include <dune/stuff/test/test_common.hh>

#include <dune/stuff/common/configtree.hh>
#include <dune/stuff/common/logging.hh>
#include <dune/stuff/la/container.hh>
#include <dune/stuff/common/exceptions.hh>
#include <dune/stuff/common/type_utils.hh>

// uncomment this for output
//std::ostream& out = std::cout;
std::ostream& out = DSC_LOG.devnull();

using namespace Dune;
using Dune::Stuff::Common::ConfigTree;
using Dune::Stuff::Exceptions::results_are_not_as_expected;


struct CreateByOperator { static ConfigTree create() {
    ConfigTree config;
    config["string"] = "string";
    config["sub1.int"] = "1";
    config["sub2.size_t"] = "1";
    config["sub2.subsub1.vector"] = "[0 1]";
    return config;
} };

struct CreateByKeyAndValueAndAdd { static ConfigTree create() {
    ConfigTree config("string", "string");
    config.set("sub1.int", "1");
    config.set("sub2.size_t", 1);
    config.add(ConfigTree("vector", "[0 1]"), "sub2.subsub1");
    return config;
} };

struct CreateByKeysAndValues { static ConfigTree create() {
    return ConfigTree({"string", "sub1.int", "sub2.size_t", "sub2.subsub1.vector"},
                      {"string", "1",        "1",           "[0 1]"});
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
    if (vec[0] != 0) DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                                           vec[0] << " vs. 0 with VectorType = "
                                                  << Stuff::Common::Typename< VectorType >::value());
    vec = config.get("vector", VectorType(), 2);
    if (vec.size() != 2) DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                                               vec.size() << " vs. 2 with VectorType = "
                                               << Stuff::Common::Typename< VectorType >::value());
    for (auto ii : {0, 1})
      if (vec[ii] != ii) DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                                               vec[ii] << " vs. " << ii << " with VectorType = "
                                               << Stuff::Common::Typename< VectorType >::value());

    vec = config.get< VectorType >("vector", 1);
    if (vec.size() != 1) DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                                               vec.size() << " vs. 1 with VectorType = "
                                               << Stuff::Common::Typename< VectorType >::value());
    if (vec[0] != 0) DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                                           vec[0] << " vs. 0 with VectorType = "
                                                  << Stuff::Common::Typename< VectorType >::value());
    vec = config.get< VectorType >("vector", 2);
    if (vec.size() != 2) DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                                               vec.size() << " vs. 2 with VectorType = "
                                               << Stuff::Common::Typename< VectorType >::value());
    for (auto ii : {0, 1})
      if (vec[ii] != ii) DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                                               vec[ii] << " vs. " << ii << " with VectorType = "
                                               << Stuff::Common::Typename< VectorType >::value());

    vec = config.get< VectorType >("vector");
    if (vec.size() != 2) DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                                               vec.size() << " vs. 2 with VectorType = "
                                               << Stuff::Common::Typename< VectorType >::value());
    for (auto ii : {0, 1})
      if (vec[ii] != ii) DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                                               vec[ii] << " vs. " << ii << " with VectorType = "
                                               << Stuff::Common::Typename< VectorType >::value());
  } // ... check_vector< ... >(...)

  template< class K, int d >
  static void check_vector(const ConfigTree& config)
  {
    typedef FieldVector< K, d > VectorType;
    VectorType vec = config.get("vector", VectorType(), 2);
    if (vec.size() != 2) DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                                               vec.size() << " vs. 2 with VectorType = "
                                               << Stuff::Common::Typename< VectorType >::value());
    for (auto ii : {0, 1})
      if (vec[ii] != ii) DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                                               vec[ii] << " vs. " << ii << " with VectorType = "
                                               << Stuff::Common::Typename< VectorType >::value());

    vec = config.get< VectorType >("vector", 2);
    if (vec.size() != 2) DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                                               vec.size() << " vs. 2 with VectorType = "
                                               << Stuff::Common::Typename< VectorType >::value());
    for (auto ii : {0, 1})
      if (vec[ii] != ii) DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                                               vec[ii] << " vs. " << ii << " with VectorType = "
                                               << Stuff::Common::Typename< VectorType >::value());

    vec = config.get< VectorType >("vector");
    if (vec.size() != 2) DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                                               vec.size() << " vs. 2 with VectorType = "
                                               << Stuff::Common::Typename< VectorType >::value());
    for (auto ii : {0, 1})
      if (vec[ii] != ii) DUNE_THROW_COLORFULLY(results_are_not_as_expected,
                                               vec[ii] << " vs. " << ii << " with VectorType = "
                                               << Stuff::Common::Typename< VectorType >::value());
  } // ... check_vector< K, d >(...)

  static void behaves_correctly()
  {
    const ConfigTree config = ConfigTreeCreator::create();
//    config.report(); // <- this works as well but will produce output
    config.report(out);
    config.report(out, "'prefix '");
    out << config << std::endl;
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
    check_vector< double, 2 >(config.sub("sub2.subsub1"));
    check_vector< DynamicVector< double > >(config.sub("sub2.subsub1"));
    check_vector< Stuff::LA::DuneDynamicVector< double > >(config.sub("sub2.subsub1"));
#if HAVE_DUNE_ISTL
    check_vector< Stuff::LA::IstlDenseVector< double > >(config.sub("sub2.subsub1"));
#endif
#if HAVE_EIGEN
    check_vector< Stuff::LA::EigenDenseVector< double > >(config.sub("sub2.subsub1"));
    check_vector< Stuff::LA::EigenMappedDenseVector< double > >(config.sub("sub2.subsub1"));
#endif // HAVE_EIGEN
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
