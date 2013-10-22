#include "test_common.hh"

#include <memory>

#include <dune/common/exceptions.hh>

#include <dune/stuff/functions.hh>


// we need this nasty code generation because the testing::Types< ... > only accepts 50 arguments
// and all combinations of functions and entities and dimensions and fieldtypes would be way too much
#define TEST_STRUCT_GENERATOR(ftype, etype) \
  /* we just take the constant function as a container for the types we need*/ \
  /* since this one always exists for all combinations*/ \
  template< class ConstantFunctionType > \
  struct ftype ## etype ## Test \
    : public ::testing::Test \
  { \
    typedef typename ConstantFunctionType::EntityType       E; \
    typedef typename ConstantFunctionType::DomainFieldType  D; \
    static const unsigned int                               d = ConstantFunctionType::dimDomain; \
    typedef typename ConstantFunctionType::RangeFieldType R; \
    static const unsigned int                             r = ConstantFunctionType::dimRange; \
    static const unsigned int                             rC = ConstantFunctionType::dimRangeCols; \
    typedef Dune::Stuff::Functions< E, D, d, R, r, rC > Functions; \
    typedef Dune::Stuff::LocalizableFunctionInterface< E, D, d, R, r, rC > InterfaceType; \
 \
    void check() const \
    { \
      for (const std::string& type : Functions::available()) { \
        const Dune::ParameterTree& settings = Functions::defaultSettings(type); \
        const InterfaceType* function = Functions::create(type, settings); \
        delete function; \
      } \
    } \
  };
// TEST_STRUCT_GENERATOR


#ifdef HAVE_DUNE_GRID

# include <dune/grid/sgrid.hh>

typedef typename Dune::SGrid< 1, 1 >::template Codim< 0 >::Entity DuneSGrid1dEntityType;
typedef typename Dune::SGrid< 2, 2 >::template Codim< 0 >::Entity DuneSGrid2dEntityType;
typedef typename Dune::SGrid< 3, 3 >::template Codim< 0 >::Entity DuneSGrid3dEntityType;

typedef testing::Types< Dune::Stuff::Function::Constant< DuneSGrid1dEntityType, double, 1, double, 1, 1 >
                      , Dune::Stuff::Function::Constant< DuneSGrid1dEntityType, double, 1, double, 1, 2 >
                      , Dune::Stuff::Function::Constant< DuneSGrid1dEntityType, double, 1, double, 1, 3 >
                      , Dune::Stuff::Function::Constant< DuneSGrid1dEntityType, double, 1, double, 2, 1 >
                      , Dune::Stuff::Function::Constant< DuneSGrid1dEntityType, double, 1, double, 2, 2 >
                      , Dune::Stuff::Function::Constant< DuneSGrid1dEntityType, double, 1, double, 2, 3 >
                      , Dune::Stuff::Function::Constant< DuneSGrid1dEntityType, double, 1, double, 3, 1 >
                      , Dune::Stuff::Function::Constant< DuneSGrid1dEntityType, double, 1, double, 3, 2 >
                      , Dune::Stuff::Function::Constant< DuneSGrid1dEntityType, double, 1, double, 3, 3 >
                      , Dune::Stuff::Function::Constant< DuneSGrid2dEntityType, double, 2, double, 1, 1 >
                      , Dune::Stuff::Function::Constant< DuneSGrid2dEntityType, double, 2, double, 1, 2 >
                      , Dune::Stuff::Function::Constant< DuneSGrid2dEntityType, double, 2, double, 1, 3 >
                      , Dune::Stuff::Function::Constant< DuneSGrid2dEntityType, double, 2, double, 2, 1 >
                      , Dune::Stuff::Function::Constant< DuneSGrid2dEntityType, double, 2, double, 2, 2 >
                      , Dune::Stuff::Function::Constant< DuneSGrid2dEntityType, double, 2, double, 2, 3 >
                      , Dune::Stuff::Function::Constant< DuneSGrid2dEntityType, double, 2, double, 3, 1 >
                      , Dune::Stuff::Function::Constant< DuneSGrid2dEntityType, double, 2, double, 3, 2 >
                      , Dune::Stuff::Function::Constant< DuneSGrid2dEntityType, double, 2, double, 3, 3 >
                      , Dune::Stuff::Function::Constant< DuneSGrid3dEntityType, double, 3, double, 1, 1 >
                      , Dune::Stuff::Function::Constant< DuneSGrid3dEntityType, double, 3, double, 1, 2 >
                      , Dune::Stuff::Function::Constant< DuneSGrid3dEntityType, double, 3, double, 1, 3 >
                      , Dune::Stuff::Function::Constant< DuneSGrid3dEntityType, double, 3, double, 2, 1 >
                      , Dune::Stuff::Function::Constant< DuneSGrid3dEntityType, double, 3, double, 2, 2 >
                      , Dune::Stuff::Function::Constant< DuneSGrid3dEntityType, double, 3, double, 2, 3 >
                      , Dune::Stuff::Function::Constant< DuneSGrid3dEntityType, double, 3, double, 3, 1 >
                      , Dune::Stuff::Function::Constant< DuneSGrid3dEntityType, double, 3, double, 3, 2 >
                      , Dune::Stuff::Function::Constant< DuneSGrid3dEntityType, double, 3, double, 3, 3 >
                        > FunctionsSGridEntityTypes;

TEST_STRUCT_GENERATOR(Functions, SGridEntity)
TYPED_TEST_CASE(FunctionsSGridEntityTest, FunctionsSGridEntityTypes);
TYPED_TEST(FunctionsSGridEntityTest, provides_required_methods) {
  this->check();
}

# include <dune/grid/yaspgrid.hh>

typedef typename Dune::YaspGrid< 1 >::template Codim< 0 >::Entity DuneYaspGrid1dEntityType;
typedef typename Dune::YaspGrid< 2 >::template Codim< 0 >::Entity DuneYaspGrid2dEntityType;
typedef typename Dune::YaspGrid< 3 >::template Codim< 0 >::Entity DuneYaspGrid3dEntityType;

typedef testing::Types< Dune::Stuff::Function::Constant< DuneYaspGrid1dEntityType, double, 1, double, 1, 1 >
                      , Dune::Stuff::Function::Constant< DuneYaspGrid1dEntityType, double, 1, double, 1, 2 >
                      , Dune::Stuff::Function::Constant< DuneYaspGrid1dEntityType, double, 1, double, 1, 3 >
                      , Dune::Stuff::Function::Constant< DuneYaspGrid1dEntityType, double, 1, double, 2, 1 >
                      , Dune::Stuff::Function::Constant< DuneYaspGrid1dEntityType, double, 1, double, 2, 2 >
                      , Dune::Stuff::Function::Constant< DuneYaspGrid1dEntityType, double, 1, double, 2, 3 >
                      , Dune::Stuff::Function::Constant< DuneYaspGrid1dEntityType, double, 1, double, 3, 1 >
                      , Dune::Stuff::Function::Constant< DuneYaspGrid1dEntityType, double, 1, double, 3, 2 >
                      , Dune::Stuff::Function::Constant< DuneYaspGrid1dEntityType, double, 1, double, 3, 3 >
                      , Dune::Stuff::Function::Constant< DuneYaspGrid2dEntityType, double, 2, double, 1, 1 >
                      , Dune::Stuff::Function::Constant< DuneYaspGrid2dEntityType, double, 2, double, 1, 2 >
                      , Dune::Stuff::Function::Constant< DuneYaspGrid2dEntityType, double, 2, double, 1, 3 >
                      , Dune::Stuff::Function::Constant< DuneYaspGrid2dEntityType, double, 2, double, 2, 1 >
                      , Dune::Stuff::Function::Constant< DuneYaspGrid2dEntityType, double, 2, double, 2, 2 >
                      , Dune::Stuff::Function::Constant< DuneYaspGrid2dEntityType, double, 2, double, 2, 3 >
                      , Dune::Stuff::Function::Constant< DuneYaspGrid2dEntityType, double, 2, double, 3, 1 >
                      , Dune::Stuff::Function::Constant< DuneYaspGrid2dEntityType, double, 2, double, 3, 2 >
                      , Dune::Stuff::Function::Constant< DuneYaspGrid2dEntityType, double, 2, double, 3, 3 >
                      , Dune::Stuff::Function::Constant< DuneYaspGrid3dEntityType, double, 3, double, 1, 1 >
                      , Dune::Stuff::Function::Constant< DuneYaspGrid3dEntityType, double, 3, double, 1, 2 >
                      , Dune::Stuff::Function::Constant< DuneYaspGrid3dEntityType, double, 3, double, 1, 3 >
                      , Dune::Stuff::Function::Constant< DuneYaspGrid3dEntityType, double, 3, double, 2, 1 >
                      , Dune::Stuff::Function::Constant< DuneYaspGrid3dEntityType, double, 3, double, 2, 2 >
                      , Dune::Stuff::Function::Constant< DuneYaspGrid3dEntityType, double, 3, double, 2, 3 >
                      , Dune::Stuff::Function::Constant< DuneYaspGrid3dEntityType, double, 3, double, 3, 1 >
                      , Dune::Stuff::Function::Constant< DuneYaspGrid3dEntityType, double, 3, double, 3, 2 >
                      , Dune::Stuff::Function::Constant< DuneYaspGrid3dEntityType, double, 3, double, 3, 3 >
                        > FunctionsYaspGridEntityTypes;

TEST_STRUCT_GENERATOR(Functions, YaspGridEntity)
TYPED_TEST_CASE(FunctionsYaspGridEntityTest, FunctionsYaspGridEntityTypes);
TYPED_TEST(FunctionsYaspGridEntityTest, provides_required_methods) {
  this->check();
}

# if HAVE_ALUGRID_SERIAL || HAVE_ALUGRID_PARALLEL
#   undef HAVE_GRIDTYPE
#   undef WORLDDIM
#   undef GRIDDIM
#   undef GRIDTYPE
#   undef ENABLE_ALUGRID
#   undef HAVE_ALUGRID
#   define ENABLE_ALUGRID 1
#   define HAVE_ALUGRID 1
#   include <dune/grid/alugrid.hh>

typedef typename Dune::ALUSimplexGrid< 2, 2 >::template Codim< 0 >::Entity  DuneAluSimplexGrid2dEntityType;
typedef typename Dune::ALUSimplexGrid< 3, 3 >::template Codim< 0 >::Entity  DuneAluSimplexGrid3dEntityType;
typedef typename Dune::ALUCubeGrid< 3, 3 >::template Codim< 0 >::Entity     DuneAluCubeGrid3dEntityType;

typedef testing::Types< Dune::Stuff::Function::Constant< DuneAluSimplexGrid2dEntityType, double, 2, double, 1, 1 >
                      , Dune::Stuff::Function::Constant< DuneAluSimplexGrid2dEntityType, double, 2, double, 1, 2 >
                      , Dune::Stuff::Function::Constant< DuneAluSimplexGrid2dEntityType, double, 2, double, 1, 3 >
                      , Dune::Stuff::Function::Constant< DuneAluSimplexGrid2dEntityType, double, 2, double, 2, 1 >
                      , Dune::Stuff::Function::Constant< DuneAluSimplexGrid2dEntityType, double, 2, double, 2, 2 >
                      , Dune::Stuff::Function::Constant< DuneAluSimplexGrid2dEntityType, double, 2, double, 2, 3 >
                      , Dune::Stuff::Function::Constant< DuneAluSimplexGrid2dEntityType, double, 2, double, 3, 1 >
                      , Dune::Stuff::Function::Constant< DuneAluSimplexGrid2dEntityType, double, 2, double, 3, 2 >
                      , Dune::Stuff::Function::Constant< DuneAluSimplexGrid2dEntityType, double, 2, double, 3, 3 >

                      , Dune::Stuff::Function::Constant< DuneAluSimplexGrid3dEntityType, double, 3, double, 1, 1 >
                      , Dune::Stuff::Function::Constant< DuneAluSimplexGrid3dEntityType, double, 3, double, 1, 2 >
                      , Dune::Stuff::Function::Constant< DuneAluSimplexGrid3dEntityType, double, 3, double, 1, 3 >
                      , Dune::Stuff::Function::Constant< DuneAluSimplexGrid3dEntityType, double, 3, double, 2, 1 >
                      , Dune::Stuff::Function::Constant< DuneAluSimplexGrid3dEntityType, double, 3, double, 2, 2 >
                      , Dune::Stuff::Function::Constant< DuneAluSimplexGrid3dEntityType, double, 3, double, 2, 3 >
                      , Dune::Stuff::Function::Constant< DuneAluSimplexGrid3dEntityType, double, 3, double, 3, 1 >
                      , Dune::Stuff::Function::Constant< DuneAluSimplexGrid3dEntityType, double, 3, double, 3, 2 >
                      , Dune::Stuff::Function::Constant< DuneAluSimplexGrid3dEntityType, double, 3, double, 3, 3 >

                      , Dune::Stuff::Function::Constant< DuneAluCubeGrid3dEntityType, double, 3, double, 1, 1 >
                      , Dune::Stuff::Function::Constant< DuneAluCubeGrid3dEntityType, double, 3, double, 1, 2 >
                      , Dune::Stuff::Function::Constant< DuneAluCubeGrid3dEntityType, double, 3, double, 1, 3 >
                      , Dune::Stuff::Function::Constant< DuneAluCubeGrid3dEntityType, double, 3, double, 2, 1 >
                      , Dune::Stuff::Function::Constant< DuneAluCubeGrid3dEntityType, double, 3, double, 2, 2 >
                      , Dune::Stuff::Function::Constant< DuneAluCubeGrid3dEntityType, double, 3, double, 2, 3 >
                      , Dune::Stuff::Function::Constant< DuneAluCubeGrid3dEntityType, double, 3, double, 3, 1 >
                      , Dune::Stuff::Function::Constant< DuneAluCubeGrid3dEntityType, double, 3, double, 3, 2 >
                      , Dune::Stuff::Function::Constant< DuneAluCubeGrid3dEntityType, double, 3, double, 3, 3 >
                        > FunctionsAluGridEntityTypes;

TEST_STRUCT_GENERATOR(Functions, AluGridEntity)
TYPED_TEST_CASE(FunctionsAluGridEntityTest, FunctionsAluGridEntityTypes);
TYPED_TEST(FunctionsAluGridEntityTest, provides_required_methods) {
  this->check();
}

# endif // HAVE_ALUGRID_SERIAL || HAVE_ALUGRID_PARALLEL
#endif // HAVE_DUNE_GRID


int main(int argc, char** argv)
{
  test_init(argc, argv);
  return RUN_ALL_TESTS();
}