// This file is part of the dune-stuff project:
//   http://users.dune-project.org/projects/dune-stuff/
// Copyright Holders: Felix Albrecht, Rene Milk
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

// This one has to come first (includes the config.h)!
#include <dune/stuff/test/test_common.hh>

#include <dune/stuff/common/configtree.hh>

using namespace Dune;
using Dune::Stuff::Common::ConfigTree;


TEST(ConfigTree, works)
{
  ConfigTree config;
  config["string"] = "string";
  config["int"] = "1";
  config["size_t"] = "1";
  config["vector"] = "[0; 1; 2]";

  std::string str = config.get("string", std::string("string"));
  str = config.get< std::string >("string");

  int nt = config.get("int", int(1));
  nt = config.get< int >("int");

  size_t st = config.get("size_t", size_t(1));
  st = config.get< size_t >("size_t");

  std::vector< double > vctr = config.get("vector", std::vector< double >());
  vctr = config.get< std::vector< double > >("vector", 3);
  vctr = config.get< std::vector< double > >("vector");

  FieldVector< double, 3 > f_vctr = config.get("vector", FieldVector< double, 3 >());
  f_vctr = config.get< FieldVector< double, 3 > >("vector", 3);
  f_vctr = config.get< FieldVector< double, 3 > >("vector");

  DynamicVector< double > dyn_vctr = config.get("vector", DynamicVector< double >());
  dyn_vctr = config.get< DynamicVector< double > >("vector", 3);
  dyn_vctr = config.get< DynamicVector< double > >("vector");

  Stuff::LA::DuneDynamicVector< double > la_dyn_vctr = config.get("vector", Stuff::LA::DuneDynamicVector< double >());
  la_dyn_vctr = config.get< Stuff::LA::DuneDynamicVector< double > >("vector", 3);
  la_dyn_vctr = config.get< Stuff::LA::DuneDynamicVector< double > >("vector");

#if HAVE_DUNE_ISTL
  Stuff::LA::IstlDenseVector< double > la_istl_vctr = config.get("vector", Stuff::LA::IstlDenseVector< double >());
  la_istl_vctr = config.get< Stuff::LA::IstlDenseVector< double > >("vector", 3);
  la_istl_vctr = config.get< Stuff::LA::IstlDenseVector< double > >("vector");
#endif // HAVE_DUNE_ISTL

#if HAVE_EIGEN
  Stuff::LA::EigenDenseVector< double > la_eig_vctr = config.get("vector", Stuff::LA::EigenDenseVector< double >());
  la_eig_vctr = config.get< Stuff::LA::EigenDenseVector< double > >("vector", 3);
  la_eig_vctr = config.get< Stuff::LA::EigenDenseVector< double > >("vector");

  Stuff::LA::EigenMappedDenseVector< double > la_eigmap_vctr = config.get("vector", Stuff::LA::EigenMappedDenseVector< double >());
  la_eigmap_vctr = config.get< Stuff::LA::EigenMappedDenseVector< double > >("vector", 3);
  la_eigmap_vctr = config.get< Stuff::LA::EigenMappedDenseVector< double > >("vector");
#endif // HAVE_EIGEN
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
