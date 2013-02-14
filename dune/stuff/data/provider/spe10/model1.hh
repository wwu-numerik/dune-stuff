#ifndef DUNE_STUFF_DATA_PROVIDER_SPE10_HH
#define DUNE_STUFF_DATA_PROVIDER_SPE10_HH

#ifdef HAVE_CMAKE_CONFIG
 #include "cmake_config.h"
#else
 #include "config.h"
#endif // ifdef HAVE_CMAKE_CONFIG

#include <iostream>
#include <sstream>
#include <memory>

#ifdef HAVE_EIGEN
#include <Eigen/Core>
#endif // HAVE_EIGEN

#include <dune/common/exceptions.hh>
#include <dune/common/shared_ptr.hh>
#include <dune/common/parametertree.hh>
#include <dune/common/dynmatrix.hh>
#include <dune/common/densevector.hh>

#include <dune/stuff/common/logging.hh>
#include <dune/stuff/common/parameter/tree.hh>
#include <dune/stuff/common/parameter/configcontainer.hh>

namespace Dune {
namespace Stuff {
namespace Data {
namespace Provider {
namespace Spe10 {
namespace Model1 {

template< class DomainFieldImp, int domainDimension, class RangeFieldImp, int rangeDimension >
class Permeability;

template< class DomainFieldImp, class RangeFieldImp >
class Permeability< DomainFieldImp, 2, RangeFieldImp, 1 >
{
public:
  typedef DomainFieldImp DomainFieldType;

  static const int dimDomain = 2;

  typedef RangeFieldImp RangeFieldType;

  static const int dimRange = 1;

  typedef Permeability< DomainFieldType, dimDomain, RangeFieldType, dimRange > ThisType;

  static const std::string id;

  class Function
  {
  public:
    Function(const DomainFieldType& lowerLeftX,
             const DomainFieldType& lowerLeftY,
             const DomainFieldType& upperRightX,
             const DomainFieldType& upperRightY,
             const unsigned int& numElementsX,
             const unsigned int& numElementsY,
             const Dune::shared_ptr< const Dune::DynamicMatrix< RangeFieldType > >& data)
      : lowerLeftX_(lowerLeftX)
      , lowerLeftY_(lowerLeftY)
      , upperRightX_(upperRightX)
      , upperRightY_(upperRightY)
      , numElementsX_(numElementsX)
      , numElementsY_(numElementsY)
      , data_(data)
    {}

    template< class DomainVectorType, class RangeVectorType >
    void evaluate(const Dune::DenseVector< DomainVectorType >& arg, Dune::DenseVector< RangeVectorType >& ret) const
    {
      // ensure right dimensions
      assert(arg.size() == dimDomain);
      assert(ret.size() == dimRange);
      // find i and j
      unsigned int i = std::floor(numElementsX_*((arg[0] - lowerLeftX_)/(upperRightX_ - lowerLeftX_)));
      unsigned int j = std::floor(numElementsY_*((arg[1] - lowerLeftY_)/(upperRightY_ - lowerLeftY_)));
      // return
      ret[0] = data_->operator[](i)[j];
    }

#ifdef HAVE_EIGEN
  void evaluate(const Eigen::VectorXd& arg, Eigen::VectorXd& ret) const
  {
    // ensure right dimensions
    assert(arg.rows() == dimDomain);
    assert(ret.rows() == dimRange);
    // find i and j
    unsigned int i = std::floor(numElementsX_*((arg(0) - lowerLeftX_)/(upperRightX_ - lowerLeftX_)));
    unsigned int j = std::floor(numElementsY_*((arg(1) - lowerLeftY_)/(upperRightY_ - lowerLeftY_)));
    // return
    ret(0) = data_->operator[](i)[j];
  }
#endif // HAVE_EIGEN

  private:
    const DomainFieldType lowerLeftX_;
    const DomainFieldType lowerLeftY_;
    const DomainFieldType upperRightX_;
    const DomainFieldType upperRightY_;
    const unsigned int numElementsX_;
    const unsigned int numElementsY_;
    const Dune::shared_ptr< const Dune::DynamicMatrix< RangeFieldType > > data_;
  }; // class Function

  const Dune::shared_ptr< const Function > create(const Dune::ParameterTree& paramTree,
                                                  const std::string prefix = "",
                                                  std::ostream& out = Dune::Stuff::Common::Logger().debug()) const
  {
    // preparations
    std::string filename;
    DomainFieldType lowerLeftX, lowerLeftY;
    DomainFieldType upperRightX, upperRightY;
    unsigned int numElementsX, numElementsY;
    readAndAssertParamTree(paramTree, filename,
                           lowerLeftX, lowerLeftY,
                           upperRightX, upperRightY,
                           numElementsX, numElementsY);
    out << prefix << "reading 'spe10.model1.permeability' from " << filename << "... ";
    RangeFieldType* rawData = new RangeFieldType[100*20];
    readRawDataFromFile(filename, rawData);
    Dune::shared_ptr< Dune::DynamicMatrix< RangeFieldType > >
        data = Dune::shared_ptr< Dune::DynamicMatrix< RangeFieldType > >(new Dune::DynamicMatrix< RangeFieldType >(numElementsX, numElementsY, 0.0));
    fillDataFromRaw(rawData, *data);
    out << "done" << std::endl;
    const Dune::shared_ptr< const Function >
        function = Dune::shared_ptr< const Function >(new Function(lowerLeftX,
                                                                   lowerLeftY,
                                                                   upperRightX,
                                                                   upperRightY,
                                                                   numElementsX,
                                                                   numElementsY,
                                                                   data));
    return function;
  } // const Dune::shared_ptr< const Function > create() const

private:
  void readAndAssertParamTree(const DSC::ExtendedParameterTree& paramTree,
                              std::string& filename,
                              DomainFieldType& lowerLeftX,
                              DomainFieldType& lowerLeftY,
                              DomainFieldType& upperRightX,
                              DomainFieldType& upperRightY,
                              unsigned int& numElementsX,
                              unsigned int& numElementsY) const
  {
    //!TODO no idea what these asserts were supposed to achieve. commented for complete compile fail
//    paramTree.assertKey(paramTree, "filename", id);
    filename = paramTree.get("filename", "../../../../../data/spe10/model1/spe10_model1_permeability.dat");
//    paramTree.assertKey(paramTree, "lowerLeft.0", id);
    lowerLeftX = paramTree.get("lowerLeft.0", 0.0);
//    paramTree.assertKey(paramTree, "lowerLeft.1", id);
    lowerLeftY = paramTree.get("lowerLeft.1", 0.0);
//    paramTree.assertKey(paramTree, "upperRight.0", id);
    upperRightX = paramTree.get("upperRight.0", 762.0);
//    paramTree.assertKey(paramTree, "upperRight.1", id);
    upperRightY = paramTree.get("upperRight.1", 15.24);
//    paramTree.assertKey(paramTree, "numElements.0", id);
    numElementsX = paramTree.get("numElements.0", 100);
//    paramTree.assertKey(paramTree, "numElements.1", id);
    numElementsY = paramTree.get("numElements.1", 20);
    // make sure everything is all right
    bool kaboom = false;
    std::stringstream msg;
    msg << "Error in " << id << ": the following errors were found while reading the Dune::ParameterTree given below!" << std::endl;
    if (!(lowerLeftX < upperRightX)) {
      kaboom = true;
      msg << "- !(lowerLeft.0 < upperRight.0): !(" << lowerLeftX << " < " << upperRightX << ")" << std::endl;
    }
    if (!(lowerLeftY < upperRightY)) {
      kaboom = true;
      msg << "- !(lowerLeft.1 < upperRight.1): !(" << lowerLeftY << " < " << upperRightY << ")" << std::endl;
    }
    if (!(numElementsX > 0)) {
      kaboom = true;
      msg << "- !(numElements.0 > 0): !(" << numElementsX << " > 0)" << std::endl;
    }
    if (!(numElementsX <= 100)) {
      kaboom = true;
      msg << "- !(numElements.0 <= 100): !(" << numElementsX << " <= 100)" << std::endl;
    }
    if (!(numElementsY <= 20)) {
      kaboom = true;
      msg << "- !(numElements.Y <= 20): !(" << numElementsY << " <= 20)" << std::endl;
    }
    // test if we can open the file
    const bool can_open(std::ifstream(filename.c_str()).is_open());
    if (!can_open) {
      kaboom = true;
      msg << "- could not open file given by 'filename': '" << filename << "'" << std::endl;
    }
    // throw up if we have to
    if (kaboom) {
      msg << "given in the following Dune::ParameterTree:" << std::endl;
      paramTree.report(msg);
      DUNE_THROW(Dune::InvalidStateException, msg.str());
    }
  } // void readAndAssertParamTree() const

  void readRawDataFromFile(const std::string filename, RangeFieldType* rawData) const
  {
    std::ifstream file(filename.c_str());
    assert(file && "After we checked before, this should really not blow up right now!");
    RangeFieldType val;
    file >> val;
    unsigned int counter = 0;
    while (!file.eof()) {
      rawData[counter++] = val;
      file >> val;
    }
    file.close();
  } // void readRawDataFromFile(const std::string filename, DomainFieldType* rawData) const

  void fillDataFromRaw(RangeFieldType* rawData, Dune::DynamicMatrix< RangeFieldType >& data) const
  {
    unsigned int counter = 0;
    for (unsigned int i = 0; i < data.rows(); ++ i) {
      for (unsigned int j = 0; j < data.cols(); ++j) {
        data[i][j] = rawData[counter];
        ++counter;
      }
    }
  } // void fillDatFromRaw(RangeFieldType* rawData, Dune::DynamicMatrix< RangeFieldType >& data) const
}; // class Permeability

template< class DomainFieldType, class RangeFieldType >
const std::string Permeability< DomainFieldType, 2, RangeFieldType, 1 >::id = "stuff.data.provider.spe10.model1.permeability";

} // namespace Model1
} // namespace Spe10
} // namespace Provider
} // namespace Data
} // namespace Stuff
} // namespace Dune

#endif // DUNE_STUFF_DATA_PROVIDER_SPE10_HH
