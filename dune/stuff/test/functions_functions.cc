// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#include "main.hxx"

#include <memory>

#include <dune/common/exceptions.hh>

#include <dune/stuff/functions.hh>

// we need this nasty code generation because the testing::Types< ... > only accepts 50 arguments
// and all combinations of functions and entities and dimensions and fieldtypes would be way too much
#define TEST_STRUCT_GENERATOR(ftype, etype)                                                                            \
  /* we just take the constant function as a container for the types we need */                                        \
  /* since this one always exists for all combinations */                                                              \
  template <class ConstantFunctionType>                                                                                \
  struct ftype##etype##Test : public ::testing::Test                                                                   \
  {                                                                                                                    \
    typedef typename ConstantFunctionType::EntityType E;                                                               \
    typedef typename ConstantFunctionType::DomainFieldType D;                                                          \
    static const size_t d = ConstantFunctionType::dimDomain;                                                           \
    typedef typename ConstantFunctionType::RangeFieldType R;                                                           \
    static const size_t r  = ConstantFunctionType::dimRange;                                                           \
    static const size_t rC = ConstantFunctionType::dimRangeCols;                                                       \
    typedef Dune::Stuff::FunctionsProvider<E, D, d, R, r, rC> FunctionsProvider;                                       \
    typedef typename FunctionsProvider::InterfaceType InterfaceType;                                                   \
                                                                                                                       \
    void check() const                                                                                                 \
    {                                                                                                                  \
      for (const auto& type : FunctionsProvider::available()) {                                                        \
        const Dune::Stuff::Common::Configuration config = FunctionsProvider::default_config(type);                     \
        try {                                                                                                          \
          const std::unique_ptr<InterfaceType> function = FunctionsProvider::create(type, config);                     \
        } catch (Dune::Stuff::Exceptions::spe10_data_file_missing&) {                                                  \
        }                                                                                                              \
      }                                                                                                                \
    }                                                                                                                  \
  };
// TEST_STRUCT_GENERATOR

#if HAVE_DUNE_GRID

#include <dune/grid/yaspgrid.hh>

typedef Dune::YaspGrid<1, Dune::EquidistantOffsetCoordinates<double, 1>>::Codim<0>::Entity DuneYaspGrid1dEntityType;
typedef Dune::YaspGrid<2, Dune::EquidistantOffsetCoordinates<double, 2>>::Codim<0>::Entity DuneYaspGrid2dEntityType;
typedef Dune::YaspGrid<3, Dune::EquidistantOffsetCoordinates<double, 3>>::Codim<0>::Entity DuneYaspGrid3dEntityType;

typedef testing::Types<Dune::Stuff::Functions::Constant<DuneYaspGrid1dEntityType, double, 1, double, 1, 1>,
                       Dune::Stuff::Functions::Constant<DuneYaspGrid1dEntityType, double, 1, double, 1, 2>,
                       Dune::Stuff::Functions::Constant<DuneYaspGrid1dEntityType, double, 1, double, 1, 3>,
                       Dune::Stuff::Functions::Constant<DuneYaspGrid1dEntityType, double, 1, double, 2, 1>,
                       Dune::Stuff::Functions::Constant<DuneYaspGrid1dEntityType, double, 1, double, 2, 2>,
                       Dune::Stuff::Functions::Constant<DuneYaspGrid1dEntityType, double, 1, double, 2, 3>,
                       Dune::Stuff::Functions::Constant<DuneYaspGrid1dEntityType, double, 1, double, 3, 1>,
                       Dune::Stuff::Functions::Constant<DuneYaspGrid1dEntityType, double, 1, double, 3, 2>,
                       Dune::Stuff::Functions::Constant<DuneYaspGrid1dEntityType, double, 1, double, 3, 3>,
                       Dune::Stuff::Functions::Constant<DuneYaspGrid2dEntityType, double, 2, double, 1, 1>,
                       Dune::Stuff::Functions::Constant<DuneYaspGrid2dEntityType, double, 2, double, 1, 2>,
                       Dune::Stuff::Functions::Constant<DuneYaspGrid2dEntityType, double, 2, double, 1, 3>,
                       Dune::Stuff::Functions::Constant<DuneYaspGrid2dEntityType, double, 2, double, 2, 1>,
                       Dune::Stuff::Functions::Constant<DuneYaspGrid2dEntityType, double, 2, double, 2, 2>,
                       Dune::Stuff::Functions::Constant<DuneYaspGrid2dEntityType, double, 2, double, 2, 3>,
                       Dune::Stuff::Functions::Constant<DuneYaspGrid2dEntityType, double, 2, double, 3, 1>,
                       Dune::Stuff::Functions::Constant<DuneYaspGrid2dEntityType, double, 2, double, 3, 2>,
                       Dune::Stuff::Functions::Constant<DuneYaspGrid2dEntityType, double, 2, double, 3, 3>,
                       Dune::Stuff::Functions::Constant<DuneYaspGrid3dEntityType, double, 3, double, 1, 1>,
                       Dune::Stuff::Functions::Constant<DuneYaspGrid3dEntityType, double, 3, double, 1, 2>,
                       Dune::Stuff::Functions::Constant<DuneYaspGrid3dEntityType, double, 3, double, 1, 3>,
                       Dune::Stuff::Functions::Constant<DuneYaspGrid3dEntityType, double, 3, double, 2, 1>,
                       Dune::Stuff::Functions::Constant<DuneYaspGrid3dEntityType, double, 3, double, 2, 2>,
                       Dune::Stuff::Functions::Constant<DuneYaspGrid3dEntityType, double, 3, double, 2, 3>,
                       Dune::Stuff::Functions::Constant<DuneYaspGrid3dEntityType, double, 3, double, 3, 1>,
                       Dune::Stuff::Functions::Constant<DuneYaspGrid3dEntityType, double, 3, double, 3, 2>,
                       Dune::Stuff::Functions::Constant<DuneYaspGrid3dEntityType, double, 3, double, 3, 3>>
    FunctionsYaspGridEntityTypes;

TEST_STRUCT_GENERATOR(Functions, YaspGridEntity)
TYPED_TEST_CASE(FunctionsYaspGridEntityTest, FunctionsYaspGridEntityTypes);
TYPED_TEST(FunctionsYaspGridEntityTest, provides_required_methods) { this->check(); }

#if HAVE_ALUGRID
#include <dune/grid/alugrid.hh>

typedef Dune::ALUGrid<2, 2, Dune::simplex, Dune::nonconforming>::Codim<0>::Entity DuneAluSimplexGrid2dEntityType;
typedef Dune::ALUGrid<3, 3, Dune::simplex, Dune::nonconforming>::Codim<0>::Entity DuneAluSimplexGrid3dEntityType;
typedef Dune::ALUGrid<3, 3, Dune::cube, Dune::nonconforming>::Codim<0>::Entity DuneAluCubeGrid3dEntityType;

typedef testing::Types<Dune::Stuff::Functions::Constant<DuneAluSimplexGrid2dEntityType, double, 2, double, 1, 1>,
                       Dune::Stuff::Functions::Constant<DuneAluSimplexGrid2dEntityType, double, 2, double, 1, 2>,
                       Dune::Stuff::Functions::Constant<DuneAluSimplexGrid2dEntityType, double, 2, double, 1, 3>,
                       Dune::Stuff::Functions::Constant<DuneAluSimplexGrid2dEntityType, double, 2, double, 2, 1>,
                       Dune::Stuff::Functions::Constant<DuneAluSimplexGrid2dEntityType, double, 2, double, 2, 2>,
                       Dune::Stuff::Functions::Constant<DuneAluSimplexGrid2dEntityType, double, 2, double, 2, 3>,
                       Dune::Stuff::Functions::Constant<DuneAluSimplexGrid2dEntityType, double, 2, double, 3, 1>,
                       Dune::Stuff::Functions::Constant<DuneAluSimplexGrid2dEntityType, double, 2, double, 3, 2>,
                       Dune::Stuff::Functions::Constant<DuneAluSimplexGrid2dEntityType, double, 2, double, 3, 3>

                       ,
                       Dune::Stuff::Functions::Constant<DuneAluSimplexGrid3dEntityType, double, 3, double, 1, 1>,
                       Dune::Stuff::Functions::Constant<DuneAluSimplexGrid3dEntityType, double, 3, double, 1, 2>,
                       Dune::Stuff::Functions::Constant<DuneAluSimplexGrid3dEntityType, double, 3, double, 1, 3>,
                       Dune::Stuff::Functions::Constant<DuneAluSimplexGrid3dEntityType, double, 3, double, 2, 1>,
                       Dune::Stuff::Functions::Constant<DuneAluSimplexGrid3dEntityType, double, 3, double, 2, 2>,
                       Dune::Stuff::Functions::Constant<DuneAluSimplexGrid3dEntityType, double, 3, double, 2, 3>,
                       Dune::Stuff::Functions::Constant<DuneAluSimplexGrid3dEntityType, double, 3, double, 3, 1>,
                       Dune::Stuff::Functions::Constant<DuneAluSimplexGrid3dEntityType, double, 3, double, 3, 2>,
                       Dune::Stuff::Functions::Constant<DuneAluSimplexGrid3dEntityType, double, 3, double, 3, 3>

                       ,
                       Dune::Stuff::Functions::Constant<DuneAluCubeGrid3dEntityType, double, 3, double, 1, 1>,
                       Dune::Stuff::Functions::Constant<DuneAluCubeGrid3dEntityType, double, 3, double, 1, 2>,
                       Dune::Stuff::Functions::Constant<DuneAluCubeGrid3dEntityType, double, 3, double, 1, 3>,
                       Dune::Stuff::Functions::Constant<DuneAluCubeGrid3dEntityType, double, 3, double, 2, 1>,
                       Dune::Stuff::Functions::Constant<DuneAluCubeGrid3dEntityType, double, 3, double, 2, 2>,
                       Dune::Stuff::Functions::Constant<DuneAluCubeGrid3dEntityType, double, 3, double, 2, 3>,
                       Dune::Stuff::Functions::Constant<DuneAluCubeGrid3dEntityType, double, 3, double, 3, 1>,
                       Dune::Stuff::Functions::Constant<DuneAluCubeGrid3dEntityType, double, 3, double, 3, 2>,
                       Dune::Stuff::Functions::Constant<DuneAluCubeGrid3dEntityType, double, 3, double, 3, 3>>
    FunctionsAluGridEntityTypes;

TEST_STRUCT_GENERATOR(Functions, AluGridEntity)
TYPED_TEST_CASE(FunctionsAluGridEntityTest, FunctionsAluGridEntityTypes);
TYPED_TEST(FunctionsAluGridEntityTest, provides_required_methods) { this->check(); }

#endif // HAVE_ALUGRID
#endif // HAVE_DUNE_GRID
