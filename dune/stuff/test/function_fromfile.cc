#include "test_common.hh"

#include <dune/stuff/common/string.hh>
#include <dune/stuff/function/fromfile.hh>
#include <dune/common/fvector.hh>

using namespace Dune::Stuff;


TEST(FUNCTION_FROMFILE, All) {
  typedef double      DomainFieldType;
  const int           dimDomain = 3;
  typedef double      RangeFieldType;
  typedef Dune::FieldMatrix< RangeFieldType, 3, 3> RangeType;

  std::string filename = "tensors.txt";
  // hier noch die richtigen Abmessungen des Gitters nehmen!
  Dune::FieldVector<DomainFieldType, dimDomain> lowerLeft(0.0);
  Dune::FieldVector<DomainFieldType, dimDomain> upperRight(10.0);
  std::vector< size_t > numElements = {117, 142, 124};

  FunctionFromFile< DomainFieldType, dimDomain, RangeFieldType, dimDomain, dimDomain >
                                                  fct(filename, lowerLeft, upperRight, numElements);

  Dune::FieldVector<DomainFieldType, dimDomain> x(4);
  RangeType ret;
  fct.evaluate(x, ret);
  std::cout << ret << std::endl;


  // jetzt nochmal für die brainmask:
  std::string filename2 = "brainmask.txt";
  typedef int      RangeFieldType2;
  typedef Dune::FieldVector< RangeFieldType2, 1> RangeType2;
  FunctionFromFile< DomainFieldType, dimDomain, RangeFieldType2, 1, 1 >
                                                  fct2(filename2, lowerLeft, upperRight, numElements);
  RangeType2 ret2;
  fct2.evaluate(x, ret2);
  std::cout << ret2 << std::endl;

}

int main(int argc, char** argv)
{
  try {
    test_init(argc, argv);
    return RUN_ALL_TESTS();
  } catch(Dune::Exception& e) {
    std::cerr << "Dune reported error: " << e.what() << std::endl;
  } catch(std::exception& e) {
    std::cerr << e.what() << std::endl;
  } catch(...) {
    std::cerr << "Unknown exception thrown!" << std::endl;
  } // try
}
