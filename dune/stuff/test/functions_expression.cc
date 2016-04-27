// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#include "main.hxx"

#include <memory>

#include <dune/common/exceptions.hh>

#include <dune/stuff/functions/interfaces.hh>
#include <dune/stuff/functions/expression.hh>


// we need this nasty code generation because the testing::Types< ... > only accepts 50 arguments
// and all combinations of functions and entities and dimensions and fieldtypes would be way too much
#define TEST_STRUCT_GENERATOR(ftype, etype) \
  template< class LocalizableFunctionType > \
  struct ftype ## etype ## Test \
    : public ::testing::Test \
  { \
    typedef typename LocalizableFunctionType::EntityType EntityType; \
    typedef typename LocalizableFunctionType::LocalfunctionType LocalfunctionType; \
    typedef typename LocalizableFunctionType::DomainFieldType DomainFieldType; \
    static const size_t                                       dimDomain = LocalizableFunctionType::dimDomain; \
    typedef typename LocalizableFunctionType::DomainType      DomainType; \
    typedef typename LocalizableFunctionType::RangeFieldType  RangeFieldType; \
    static const size_t                                       dimRange = LocalizableFunctionType::dimRange; \
    static const size_t                                       dimRangeCols = LocalizableFunctionType::dimRangeCols; \
    typedef typename LocalizableFunctionType::RangeType       RangeType; \
    typedef typename LocalizableFunctionType::JacobianRangeType JacobianRangeType; \
 \
    void check() const \
    { \
      Dune::Stuff::Common::Configuration config = LocalizableFunctionType::default_config(); \
      const std::unique_ptr< const LocalizableFunctionType > function(LocalizableFunctionType::create(config)); \
      config["expression"] = "[2*x[0] 3*x[0] 4*x[0]; 1 sin(x[0]) 0; cos(x[0]) x[0] 0]"; \
      config["gradient"] = "[2 0 0; 0 0 0; -sin(x[0]) 0 0]"; \
      config["gradient.1"] = "[3 0 0; cos(x[0]) 0 0; 1 0 0]"; \
      config["gradient.2"] = "[4 0 0; 0 0 0; 0 0 0]"; \
      const std::unique_ptr< const LocalizableFunctionType > function2(LocalizableFunctionType::create(config)); \
      const std::unique_ptr< const LocalizableFunctionType > function3( \
                                                new LocalizableFunctionType("x", \
                                                                            "sin(x[0])", \
                                                                            3, \
                                                                            LocalizableFunctionType::static_id(), \
                                                                            {"cos(x[0])", "0", "0"})); \
    } \
  };
// TEST_STRUCT_GENERATOR


#if HAVE_DUNE_GRID

# include <dune/grid/sgrid.hh>

typedef Dune::SGrid< 1, 1 >::Codim< 0 >::Entity DuneSGrid1dEntityType;
typedef Dune::SGrid< 2, 2 >::Codim< 0 >::Entity DuneSGrid2dEntityType;
typedef Dune::SGrid< 3, 3 >::Codim< 0 >::Entity DuneSGrid3dEntityType;

// the matrix valued version is missing the create() and default_config() method
typedef testing::Types< Dune::Stuff::Functions::Expression< DuneSGrid1dEntityType, double, 1, double, 1, 1 >
                      , Dune::Stuff::Functions::Expression< DuneSGrid1dEntityType, double, 1, double, 1, 2 >
                      , Dune::Stuff::Functions::Expression< DuneSGrid1dEntityType, double, 1, double, 1, 3 >
                      , Dune::Stuff::Functions::Expression< DuneSGrid1dEntityType, double, 1, double, 2, 1 >
                      , Dune::Stuff::Functions::Expression< DuneSGrid1dEntityType, double, 1, double, 2, 2 >
                      , Dune::Stuff::Functions::Expression< DuneSGrid1dEntityType, double, 1, double, 2, 3 >
                      , Dune::Stuff::Functions::Expression< DuneSGrid1dEntityType, double, 1, double, 3, 1 >
                      , Dune::Stuff::Functions::Expression< DuneSGrid1dEntityType, double, 1, double, 3, 2 >
                      , Dune::Stuff::Functions::Expression< DuneSGrid1dEntityType, double, 1, double, 3, 3 >
                      , Dune::Stuff::Functions::Expression< DuneSGrid2dEntityType, double, 2, double, 1, 1 >
                      , Dune::Stuff::Functions::Expression< DuneSGrid2dEntityType, double, 2, double, 1, 2 >
                      , Dune::Stuff::Functions::Expression< DuneSGrid2dEntityType, double, 2, double, 1, 3 >
                      , Dune::Stuff::Functions::Expression< DuneSGrid2dEntityType, double, 2, double, 2, 1 >
                      , Dune::Stuff::Functions::Expression< DuneSGrid2dEntityType, double, 2, double, 2, 2 >
                      , Dune::Stuff::Functions::Expression< DuneSGrid2dEntityType, double, 2, double, 2, 3 >
                      , Dune::Stuff::Functions::Expression< DuneSGrid2dEntityType, double, 2, double, 3, 1 >
                      , Dune::Stuff::Functions::Expression< DuneSGrid2dEntityType, double, 2, double, 3, 2 >
                      , Dune::Stuff::Functions::Expression< DuneSGrid2dEntityType, double, 2, double, 3, 3 >
                      , Dune::Stuff::Functions::Expression< DuneSGrid3dEntityType, double, 3, double, 1, 1 >
                      , Dune::Stuff::Functions::Expression< DuneSGrid3dEntityType, double, 3, double, 1, 2 >
                      , Dune::Stuff::Functions::Expression< DuneSGrid3dEntityType, double, 3, double, 1, 3 >
                      , Dune::Stuff::Functions::Expression< DuneSGrid3dEntityType, double, 3, double, 2, 1 >
                      , Dune::Stuff::Functions::Expression< DuneSGrid3dEntityType, double, 3, double, 2, 2 >
                      , Dune::Stuff::Functions::Expression< DuneSGrid3dEntityType, double, 3, double, 2, 3 >
                      , Dune::Stuff::Functions::Expression< DuneSGrid3dEntityType, double, 3, double, 3, 1 >
                      , Dune::Stuff::Functions::Expression< DuneSGrid3dEntityType, double, 3, double, 3, 2 >
                      , Dune::Stuff::Functions::Expression< DuneSGrid3dEntityType, double, 3, double, 3, 3 >
                        > ExpressionFunctionSGridEntityTypes;

TEST_STRUCT_GENERATOR(ExpressionFunction, SGridEntity)
TYPED_TEST_CASE(ExpressionFunctionSGridEntityTest, ExpressionFunctionSGridEntityTypes);
TYPED_TEST(ExpressionFunctionSGridEntityTest, provides_required_methods) {
  this->check();
}

# include <dune/grid/yaspgrid.hh>

typedef Dune::YaspGrid< 1 >::Codim< 0 >::Entity DuneYaspGrid1dEntityType;
typedef Dune::YaspGrid< 2 >::Codim< 0 >::Entity DuneYaspGrid2dEntityType;
typedef Dune::YaspGrid< 3 >::Codim< 0 >::Entity DuneYaspGrid3dEntityType;

typedef testing::Types< Dune::Stuff::Functions::Expression< DuneYaspGrid1dEntityType, double, 1, double, 1, 1 >
                      , Dune::Stuff::Functions::Expression< DuneYaspGrid1dEntityType, double, 1, double, 1, 2 >
                      , Dune::Stuff::Functions::Expression< DuneYaspGrid1dEntityType, double, 1, double, 1, 3 >
                      , Dune::Stuff::Functions::Expression< DuneYaspGrid1dEntityType, double, 1, double, 2, 1 >
                      , Dune::Stuff::Functions::Expression< DuneYaspGrid1dEntityType, double, 1, double, 2, 2 >
                      , Dune::Stuff::Functions::Expression< DuneYaspGrid1dEntityType, double, 1, double, 2, 3 >
                      , Dune::Stuff::Functions::Expression< DuneYaspGrid1dEntityType, double, 1, double, 3, 1 >
                      , Dune::Stuff::Functions::Expression< DuneYaspGrid1dEntityType, double, 1, double, 3, 2 >
                      , Dune::Stuff::Functions::Expression< DuneYaspGrid1dEntityType, double, 1, double, 3, 3 >
                      , Dune::Stuff::Functions::Expression< DuneYaspGrid2dEntityType, double, 2, double, 1, 1 >
                      , Dune::Stuff::Functions::Expression< DuneYaspGrid2dEntityType, double, 2, double, 1, 2 >
                      , Dune::Stuff::Functions::Expression< DuneYaspGrid2dEntityType, double, 2, double, 1, 3 >
                      , Dune::Stuff::Functions::Expression< DuneYaspGrid2dEntityType, double, 2, double, 2, 1 >
                      , Dune::Stuff::Functions::Expression< DuneYaspGrid2dEntityType, double, 2, double, 2, 2 >
                      , Dune::Stuff::Functions::Expression< DuneYaspGrid2dEntityType, double, 2, double, 2, 3 >
                      , Dune::Stuff::Functions::Expression< DuneYaspGrid2dEntityType, double, 2, double, 3, 1 >
                      , Dune::Stuff::Functions::Expression< DuneYaspGrid2dEntityType, double, 2, double, 3, 2 >
                      , Dune::Stuff::Functions::Expression< DuneYaspGrid2dEntityType, double, 2, double, 3, 3 >
                      , Dune::Stuff::Functions::Expression< DuneYaspGrid3dEntityType, double, 3, double, 1, 1 >
                      , Dune::Stuff::Functions::Expression< DuneYaspGrid3dEntityType, double, 3, double, 1, 2 >
                      , Dune::Stuff::Functions::Expression< DuneYaspGrid3dEntityType, double, 3, double, 1, 3 >
                      , Dune::Stuff::Functions::Expression< DuneYaspGrid3dEntityType, double, 3, double, 2, 1 >
                      , Dune::Stuff::Functions::Expression< DuneYaspGrid3dEntityType, double, 3, double, 2, 2 >
                      , Dune::Stuff::Functions::Expression< DuneYaspGrid3dEntityType, double, 3, double, 2, 3 >
                      , Dune::Stuff::Functions::Expression< DuneYaspGrid3dEntityType, double, 3, double, 3, 1 >
                      , Dune::Stuff::Functions::Expression< DuneYaspGrid3dEntityType, double, 3, double, 3, 2 >
                      , Dune::Stuff::Functions::Expression< DuneYaspGrid3dEntityType, double, 3, double, 3, 3 >
                        > ExpressionFunctionYaspGridEntityTypes;

TEST_STRUCT_GENERATOR(ExpressionFunction, YaspGridEntity)
TYPED_TEST_CASE(ExpressionFunctionYaspGridEntityTest, ExpressionFunctionYaspGridEntityTypes);
TYPED_TEST(ExpressionFunctionYaspGridEntityTest, provides_required_methods) {
  this->check();
}

# if HAVE_DUNE_ALUGRID
#   include <dune/alugrid/dgf.hh>

typedef Dune::ALUGrid< 2, 2, Dune::simplex, Dune::nonconforming>::Codim< 0 >::Entity  DuneAluSimplexGrid2dEntityType;
typedef Dune::ALUGrid< 3, 3, Dune::simplex, Dune::nonconforming>::Codim< 0 >::Entity  DuneAluSimplexGrid3dEntityType;
typedef Dune::ALUGrid< 3, 3, Dune::cube, Dune::nonconforming>::Codim< 0 >::Entity     DuneAluCubeGrid3dEntityType;

typedef testing::Types< Dune::Stuff::Functions::Expression< DuneAluSimplexGrid2dEntityType, double, 2, double, 1, 1 >
                      , Dune::Stuff::Functions::Expression< DuneAluSimplexGrid2dEntityType, double, 2, double, 1, 2 >
                      , Dune::Stuff::Functions::Expression< DuneAluSimplexGrid2dEntityType, double, 2, double, 1, 3 >
                      , Dune::Stuff::Functions::Expression< DuneAluSimplexGrid2dEntityType, double, 2, double, 2, 1 >
                      , Dune::Stuff::Functions::Expression< DuneAluSimplexGrid2dEntityType, double, 2, double, 2, 2 >
                      , Dune::Stuff::Functions::Expression< DuneAluSimplexGrid2dEntityType, double, 2, double, 2, 3 >
                      , Dune::Stuff::Functions::Expression< DuneAluSimplexGrid2dEntityType, double, 2, double, 3, 1 >
                      , Dune::Stuff::Functions::Expression< DuneAluSimplexGrid2dEntityType, double, 2, double, 3, 2 >
                      , Dune::Stuff::Functions::Expression< DuneAluSimplexGrid2dEntityType, double, 2, double, 3, 3 >

                      , Dune::Stuff::Functions::Expression< DuneAluSimplexGrid3dEntityType, double, 3, double, 1, 1 >
                      , Dune::Stuff::Functions::Expression< DuneAluSimplexGrid3dEntityType, double, 3, double, 1, 2 >
                      , Dune::Stuff::Functions::Expression< DuneAluSimplexGrid3dEntityType, double, 3, double, 1, 3 >
                      , Dune::Stuff::Functions::Expression< DuneAluSimplexGrid3dEntityType, double, 3, double, 2, 1 >
                      , Dune::Stuff::Functions::Expression< DuneAluSimplexGrid3dEntityType, double, 3, double, 2, 2 >
                      , Dune::Stuff::Functions::Expression< DuneAluSimplexGrid3dEntityType, double, 3, double, 2, 3 >
                      , Dune::Stuff::Functions::Expression< DuneAluSimplexGrid3dEntityType, double, 3, double, 3, 1 >
                      , Dune::Stuff::Functions::Expression< DuneAluSimplexGrid3dEntityType, double, 3, double, 3, 2 >
                      , Dune::Stuff::Functions::Expression< DuneAluSimplexGrid3dEntityType, double, 3, double, 3, 3 >

                      , Dune::Stuff::Functions::Expression< DuneAluCubeGrid3dEntityType, double, 3, double, 1, 1 >
                      , Dune::Stuff::Functions::Expression< DuneAluCubeGrid3dEntityType, double, 3, double, 1, 2 >
                      , Dune::Stuff::Functions::Expression< DuneAluCubeGrid3dEntityType, double, 3, double, 1, 3 >
                      , Dune::Stuff::Functions::Expression< DuneAluCubeGrid3dEntityType, double, 3, double, 2, 1 >
                      , Dune::Stuff::Functions::Expression< DuneAluCubeGrid3dEntityType, double, 3, double, 2, 2 >
                      , Dune::Stuff::Functions::Expression< DuneAluCubeGrid3dEntityType, double, 3, double, 2, 3 >
                      , Dune::Stuff::Functions::Expression< DuneAluCubeGrid3dEntityType, double, 3, double, 3, 1 >
                      , Dune::Stuff::Functions::Expression< DuneAluCubeGrid3dEntityType, double, 3, double, 3, 2 >
                      , Dune::Stuff::Functions::Expression< DuneAluCubeGrid3dEntityType, double, 3, double, 3, 3 >
                        > ExpressionFunctionAluGridEntityTypes;

TEST_STRUCT_GENERATOR(ExpressionFunction, AluGridEntity)
TYPED_TEST_CASE(ExpressionFunctionAluGridEntityTest, ExpressionFunctionAluGridEntityTypes);
TYPED_TEST(ExpressionFunctionAluGridEntityTest, provides_required_methods) {
  this->check();
}

# endif // HAVE_DUNE_ALUGRID
#endif // HAVE_DUNE_GRID


