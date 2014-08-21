// This file is part of the dune-stuff project:
//   https://users.dune-project.org/projects/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#warning This header is deprecated! Include <dune/stuff/test/main.hh> instead (20.08.2014)!

int
  DUNE_DEPRECATED_MSG("use -DDUNE_STUFF_TEST_MAIN_CATCH_EXCEPTIONS=1")
    main(int argc, char** argv)
{
  try {
    testing::InitGoogleTest(&argc, argv);
    DSC_CONFIG.read_options(argc, argv);
  #if HAVE_DUNE_FEM
    Dune::Fem::MPIManager::initialize(argc, argv);
  #else
    Dune::MPIHelper::instance(argc, argv);
  #endif
    DSC::Logger().create(DSC::LOG_CONSOLE | DSC::LOG_ERROR);

    return RUN_ALL_TESTS();
  } catch (Dune::Exception& e) {
    std::cerr << "\nDune reported error: " << e.what() << std::endl;
    std::abort();
  } catch (std::exception& e) {
    std::cerr << "\n" << e.what() << std::endl;
    std::abort();
  } catch (...) {
    std::cerr << "Unknown exception thrown!" << std::endl;
    std::abort();
  } // try
} // ... main(...)
