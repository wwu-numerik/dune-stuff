#ifndef DUNE_STUFF_FUNCTION_AFFINEPARAMETRIC_CHECKERBOARD_HH
#define DUNE_STUFF_FUNCTION_AFFINEPARAMETRIC_CHECKERBOARD_HH

#include <memory>
#include <vector>

#include <dune/common/shared_ptr.hh>

#include <dune/stuff/common/parameter.hh>
#include <dune/stuff/common/parameter/tree.hh>
#include <dune/stuff/common/string.hh>
#include <dune/stuff/function/checkerboard.hh>

#include "../interface.hh"
#include "../checkerboard.hh"

namespace Dune {
namespace Stuff {


// forward, to allow for specialization
template< class DomainFieldImp, int domainDim, class RangeFieldImp, int rangeDimRows, int rangeDimCols = 1 >
class AffineParametricFunctionCheckerboard;


template< class DomainFieldImp, int domainDim, class RangeFieldImp >
class AffineParametricFunctionCheckerboard< DomainFieldImp, domainDim, RangeFieldImp, 1, 1 >
  : public AffineParametricFunctionInterface< DomainFieldImp, domainDim, RangeFieldImp, 1, 1 >
{
  typedef AffineParametricFunctionInterface< DomainFieldImp, domainDim, RangeFieldImp, 1, 1 > BaseType;
public:
  typedef AffineParametricFunctionCheckerboard< DomainFieldImp, domainDim, RangeFieldImp, 1, 1 > ThisType;

  typedef typename BaseType::DomainFieldType  DomainFieldType;
  static const int                            dimDomain = BaseType::dimDomain;
  typedef typename BaseType::DomainType       DomainType;
  typedef typename BaseType::RangeFieldType   RangeFieldType;
//  static const int                            dimRange = BaseType::dimRange;
  typedef typename BaseType::RangeType        RangeType;

  typedef typename BaseType::ParamFieldType ParamFieldType;
  typedef typename BaseType::ParamType      ParamType;

  typedef typename BaseType::ComponentType    ComponentType;
  typedef typename BaseType::CoefficientType  CoefficientType;

  static const std::string id()
  {
    return BaseType::id() + ".checkerboard";
  }

  AffineParametricFunctionCheckerboard(const DomainType& _lowerLeft,
                                       const DomainType& _upperRight,
                                       const std::vector< size_t >& _numElements,
                                       const std::vector< ParamType >& _paramRange,
                                       const std::string _name = id())
    : paramSize_(1)
    , paramRange_(_paramRange)
    , name_(_name)
  {
    // build SeparableCheckerboard structure
    assert(int(_numElements.size()) >= dimDomain && "Please provide at least as many number of elements as dimensions!");
    for (int dd = 0; dd < dimDomain; ++dd) {
      assert(_lowerLeft[dd] <= _upperRight[dd]
             && "Given '_lowerLeft' has to be piecewise <= compared to given '_upperRight'!");
      assert(_numElements[dd] > 0 && "Please provide a positive number of elements per dimension!");
      paramSize_ *= _numElements[dd];
    }
    assert(_paramRange.size() == 2 && "Vector has wrong size!");
    assert(_paramRange[0].size() == paramSize_ && "Vector has wrong size!");
    assert(_paramRange[1].size() == paramSize_ && "Vector has wrong size!");
    for (size_t qq = 0; qq < paramSize_; ++qq) {
      assert(paramRange_[0][qq] <= paramRange_[1][qq]
          && "Given minimal parameter has to be piecewise <= maximum parameter!");
    }
    // create the coefficients and components
    typedef FunctionCheckerboard< DomainFieldType, dimDomain, RangeFieldType, 1 > NonparametricType;
    for (size_t ii = 0; ii < paramSize_; ++ii) {
      std::vector< RangeType > indicator(paramSize_, RangeType(0));
      indicator[ii] = RangeType(1);
      components_.emplace_back(new NonparametricType(_lowerLeft, _upperRight, _numElements, indicator));
      coefficients_.emplace_back(new CoefficientType("mu[" + DSC::toString(ii) + "]"));
    } // create the coefficients and components
    parameterExplanation_ = std::vector< std::string >(paramSize_, "");
    // create the explanations
    for (size_t ii = 0; ii < _numElements[0]; ++ii) {
      if (dimDomain == 1)
        parameterExplanation_[ii] = "value_in_subdomain_" + DSC::toString(ii);
      else
        for (unsigned int jj = 0; jj < _numElements[1]; ++jj) {
          if (dimDomain == 2)
            parameterExplanation_[ii + jj*_numElements[0]] = "value_in_subdomain_"
                + DSC::toString(ii)
                + "_" + DSC::toString(jj);
          else
            for (unsigned int kk = 0; kk < _numElements[2]; ++kk) {
              parameterExplanation_[ii + jj*_numElements[0] + kk*_numElements[0]*_numElements[1]] = "value_in_subdomain_"
                  + DSC::toString(ii)
                  + "_" + DSC::toString(jj)
                  + "_" + DSC::toString(kk);
            }
        }
    }
  } // AffineParametricFunctionCheckerboard(...)

  static Dune::ParameterTree defaultSettings(const std::string subName = "")
  {
    Dune::ParameterTree description;
    description["lowerLeft"] = "[0.0; 0.0; 0.0]";
    description["upperRight"] = "[1.0; 1.0; 1.0]";
    description["numElements"] = "[2; 2; 2]";
    description["paramMin"] = "[1.0; 2.0; 3.0; 4.0; 5.0; 6.0; 7.0; 8.0]";
    description["paramMax"] = "[10.0; 20.0; 30.0; 40.0; 50.0; 60.0; 70.0; 80.0]";
    description["name"] = id();
    if (subName.empty())
      return description;
    else {
      DSC::ExtendedParameterTree extendedDescription;
      extendedDescription.add(description, subName);
      return extendedDescription;
    }
  } // ... defaultSettings(...)

  static ThisType* create(const DSC::ExtendedParameterTree settings)
  {
    // get data
    const std::string _name = settings.get< std::string >("name", id());
    const std::vector< DomainFieldType > lowerLefts = settings.getVector("lowerLeft", DomainFieldType(0), dimDomain);
    const std::vector< DomainFieldType > upperRights = settings.getVector("upperRight",
                                                                             DomainFieldType(1),
                                                                             dimDomain);
    const std::vector< size_t > numElements = settings.getVector("numElements", size_t(1), dimDomain);
    // get paramSize
    size_t paramSize = 1u;
    for (int dd = 0; dd < dimDomain; ++dd) {
      assert(numElements[dd] > 0 && "Please provide positive number of elements per dim!");
      paramSize *= numElements[dd];
    }
    const std::vector< ParamFieldType > paramMins = settings.getVector("paramMin",
                                                                          ParamFieldType(1),
                                                                          paramSize);
    const std::vector< ParamFieldType > paramMaxs = settings.getVector("paramMax",
                                                                          ParamFieldType(1),
                                                                          paramSize);
    // convert and leave the checks to the constructor
    DomainType lowerLeft;
    DomainType upperRight;
    for (int dd = 0; dd < dimDomain; ++dd) {
      lowerLeft[dd] = lowerLefts[dd];
      upperRight[dd] = upperRights[dd];
    }
    ParamType paramMin(paramSize);
    ParamType paramMax(paramSize);
    for (size_t qq = 0; qq < paramSize; ++qq) {
      paramMin[qq] = paramMins[qq];
      paramMax[qq] = paramMaxs[qq];
    }
    const std::vector< ParamType > paramRange = {paramMin, paramMax};
    // create and return
    return new ThisType(lowerLeft, upperRight, numElements, paramRange, _name);
  } // ... create(...)

  virtual int order() const
  {
    return 0;
  }

  virtual std::string name() const
  {
    return name_;
  }

  virtual size_t paramSize() const
  {
    return paramSize_;
  }

  virtual const std::vector< ParamType >& paramRange() const
  {
    return paramRange_;
  }

  virtual const std::vector< std::string >& paramExplanation() const
  {
    return parameterExplanation_;
  }

  virtual const std::vector< std::shared_ptr< const ComponentType > >& components() const
  {
    return components_;
  }

  virtual const std::vector< std::shared_ptr< const CoefficientType > >& coefficients() const
  {
    return coefficients_;
  }

  virtual void evaluate(const DomainType& _x, const ParamType& _mu, RangeType& _ret) const
  {
    assert(_mu.size() == paramSize());
    _ret = RangeFieldType(0);
    RangeType tmpComponentValue;
    for (unsigned int qq = 0; qq < paramSize_; ++qq) {
      components_[qq]->evaluate(_x, tmpComponentValue);
      tmpComponentValue *= coefficients_[qq]->evaluate(_mu);
      _ret += tmpComponentValue;
    }
  } // virtual void evaluate(const DomainType& x, const ParamType& mu, RangeType& ret) const

private:
  unsigned int paramSize_;
  std::vector< ParamType > paramRange_;
  std::string name_;
  std::vector< std::string > parameterExplanation_;
  std::vector< std::shared_ptr< const ComponentType > > components_;
  std::vector< std::shared_ptr< const CoefficientType > > coefficients_;
}; // class AffineParametricFunctionCheckerboard


} // namespace Stuff
} // namespace Dune

#endif // DUNE_STUFF_FUNCTION_AFFINEPARAMETRIC_CHECKERBOARD_HH
