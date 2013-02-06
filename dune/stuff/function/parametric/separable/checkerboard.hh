#ifndef DUNE_STUFF_FUNCTION_PARAMETRIC_SEPARABLE_CHECKERBOARD_HH
#define DUNE_STUFF_FUNCTION_PARAMETRIC_SEPARABLE_CHECKERBOARD_HH

#ifdef HAVE_CMAKE_CONFIG
  #include "cmake_config.h"
#elif defined (HAVE_CONFIG_H)
  #include <config.h>
#endif // ifdef HAVE_CMAKE_CONFIG

#include <vector>

#include <dune/common/shared_ptr.hh>

#include <dune/stuff/common/parameter.hh>
#include <dune/stuff/common/parameter/tree.hh>
#include <dune/stuff/common/string.hh>
#include <dune/stuff/function/checkerboard.hh>

#include "../../interface.hh"
#include "../../checkerboard.hh"

namespace Dune {
namespace Stuff {
namespace Function {


template< class DomainFieldImp, int domainDim,
          class RangeFieldImp, int rangeDim >
class SeparableCheckerboard;


template< class DomainFieldImp, int domainDim,
          class RangeFieldImp >
class SeparableCheckerboard< DomainFieldImp, domainDim, RangeFieldImp, 1 >
  : public Interface< DomainFieldImp, domainDim, RangeFieldImp, 1 >
{
public:
  typedef SeparableCheckerboard< DomainFieldImp, domainDim, RangeFieldImp, 1 > ThisType;
  typedef Interface< DomainFieldImp, domainDim, RangeFieldImp, 1 >    BaseType;

  typedef typename BaseType::DomainFieldType  DomainFieldType;
  static const int                            dimDomain = BaseType::dimDomain;
  typedef typename BaseType::DomainType       DomainType;
  typedef typename BaseType::RangeFieldType   RangeFieldType;
  static const int                            dimRange = BaseType::dimRange;
  typedef typename BaseType::RangeType        RangeType;

  typedef Common::Parameter::FieldType  ParamFieldType;
  typedef Common::Parameter::Type       ParamType;

  typedef typename BaseType::ComponentType    ComponentType;
  typedef typename BaseType::CoefficientType  CoefficientType;

  static const std::string id()
  {
    return "function.parametric.separable.checkerboard";
  }

  SeparableCheckerboard(const DomainType& _lowerLeft,
                        const DomainType& _upperRight,
                        const std::vector< size_t >& _numElements,
                        const std::vector< ParamType >& _paramRange,
                        const std::string _name = id())
    : paramSize_(1u)
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
    for (size_t qq = 0; qq < paramSize_; ++qq)
      assert(paramRange_[0][qq] <= paramRange_[1][qq]
          && "Given minimal parameter has to be piecewise <= maximum parameter!");
    // create the coefficients and components
    typedef Function::Checkerboard< DomainFieldType, dimDomain, RangeFieldType, dimRange > NonparametricType;
    for (size_t ii = 0; ii < paramSize_; ++ii) {
      std::vector< RangeFieldType > indicator(paramSize_, RangeFieldType(0));
      indicator[ii] = RangeFieldType(1);
      const Dune::shared_ptr< const NonparametricType > indicatorFunction
          = Dune::make_shared< NonparametricType >(_lowerLeft, _upperRight, _numElements, indicator);
      components_.push_back(indicatorFunction);
      coefficients_.push_back(Dune::make_shared< CoefficientType >("mu[" + Dune::Stuff::Common::toString(ii) + "]"));
    } // create the coefficients and components
    parameterExplanation_ = std::vector< std::string >(paramSize_, "");
    // create the explanations
    for (size_t ii = 0; ii < _numElements[0]; ++ii) {
      if (dimDomain == 1)
        parameterExplanation_[ii] = "value_in_subdomain_" + Dune::Stuff::Common::toString(ii);
      else
        for (unsigned int jj = 0; jj < _numElements[1]; ++jj) {
          if (dimDomain == 2)
            parameterExplanation_[ii + jj*_numElements[0]] = "value_in_subdomain_"
                + Dune::Stuff::Common::toString(ii)
                + "_" + Dune::Stuff::Common::toString(jj);
          else
            for (unsigned int kk = 0; kk < _numElements[2]; ++kk) {
              parameterExplanation_[ii + jj*_numElements[0] + kk*_numElements[0]*_numElements[1]] = "value_in_subdomain_"
                  + Dune::Stuff::Common::toString(ii)
                  + "_" + Dune::Stuff::Common::toString(jj)
                  + "_" + Dune::Stuff::Common::toString(kk);
            }
        }
    }
  } // SeparableCheckerboard(...)

  SeparableCheckerboard(const ThisType& other)
    : paramSize_(other.paramSize_)
    , paramRange_(other.paramRange_)
    , name_(other.name_)
    , parameterExplanation_(other.parameterExplanation_)
    , components_(other.components_)
    , coefficients_(other.coefficients_)
  {}

  ThisType& operator=(ThisType& other)
  {
    if (this != &other) {
      paramSize_ = other.paramSize();
      paramRange_ = other.paramRange();
      name_ = other.name();
      parameterExplanation_ = other.parameterExplanation();
      for (size_t q; q < paramSize_; ++q) {
        components_.push_back(other.component(q));
        coefficients_.push_back(other.coefficient(q));
      }
    }
    return *this;
  } // ThisType& operator=(ThisType& other)

  static Dune::ParameterTree createSampleDescription(const std::string subName = "")
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
      Dune::Stuff::Common::ExtendedParameterTree extendedDescription;
      extendedDescription.add(description, subName);
      return extendedDescription;
    }
  }

  static ThisType createFromDescription(const Dune::ParameterTree _description)
  {
    // get correct paramTree
    Stuff::Common::ExtendedParameterTree description(_description);
    // get data
    const std::string _name = description.get< std::string >("name", id());
    const std::vector< DomainFieldType > lowerLefts = description.getVector("lowerLeft", DomainFieldType(0), dimDomain);
    const std::vector< DomainFieldType > upperRights = description.getVector("upperRight",
                                                                             DomainFieldType(1),
                                                                             dimDomain);
    const std::vector< size_t > numElements = description.getVector("numElements", size_t(1), dimDomain);
    // get paramSize
    size_t paramSize = 1u;
    for (int dd = 0; dd < dimDomain; ++dd) {
      assert(numElements[dd] > 0 && "Please provide positive number of elements per dim!");
      paramSize *= numElements[dd];
    }
    const std::vector< ParamFieldType > paramMins = description.getVector("paramMin",
                                                                          ParamFieldType(1),
                                                                          paramSize);
    const std::vector< ParamFieldType > paramMaxs = description.getVector("paramMax",
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
    std::vector< ParamType > paramRange;
    paramRange.push_back(paramMin);
    paramRange.push_back(paramMax);
    // create and return
    return ThisType(lowerLeft, upperRight, numElements, paramRange, _name);
  } // static ThisType createFromParamTree(const Dune::ParameterTree paramTree)

  virtual bool parametric() const
  {
    return true;
  }

  virtual bool separable() const
  {
    return true;
  }

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

  virtual size_t numComponents() const
  {
    return paramSize_;
  }

  virtual size_t numCoefficients() const
  {
    return paramSize_;
  }

  virtual const std::vector< Dune::shared_ptr< const ComponentType > >& components() const
  {
    return components_;
  }

  virtual const std::vector< Dune::shared_ptr< const CoefficientType > >& coefficients() const
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
  std::vector< shared_ptr< const ComponentType > > components_;
  std::vector< shared_ptr< const CoefficientType > > coefficients_;
}; // class SeparableCheckerboard


} // namespace Function
} // namespace Stuff
} // namespace Dune

#endif // DUNE_STUFF_FUNCTION_PARAMETRIC_SEPARABLE_CHECKERBOARD_HH
