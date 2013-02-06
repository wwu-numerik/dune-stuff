#ifndef DUNE_STUFF_FUNCTION_PARAMETRIC_SEPARABLE_DEFAULT_HH
#define DUNE_STUFF_FUNCTION_PARAMETRIC_SEPARABLE_DEFAULT_HH

#ifdef HAVE_CMAKE_CONFIG
  #include "cmake_config.h"
#elif defined (HAVE_CONFIG_H)
  #include <config.h>
#endif // ifdef HAVE_CMAKE_CONFIG

#include <vector>

#include <dune/common/shared_ptr.hh>

#include <dune/stuff/common/parameter.hh>
#include <dune/stuff/common/string.hh>
#include <dune/stuff/common/color.hh>
#include <dune/stuff/common/parameter/tree.hh>

#include <dune/stuff/function.hh>
#include "coefficient.hh"

namespace Dune {
namespace Stuff {
namespace Function {


template< class DomainFieldImp, int domainDim, class RangeFieldImp, int rangeDim >
class SeparableDefault
  : public Interface< DomainFieldImp, domainDim, RangeFieldImp, rangeDim >
{
public:
  typedef SeparableDefault< DomainFieldImp, domainDim, RangeFieldImp, rangeDim >  ThisType;
  typedef Interface< DomainFieldImp, domainDim, RangeFieldImp, rangeDim >         BaseType;

  typedef typename BaseType::DomainFieldType  DomainFieldType;
  static const int                            dimDomain = BaseType::dimDomain;
  typedef typename BaseType::DomainType       DomainType;
  typedef typename BaseType::RangeFieldType   RangeFieldType;
  static const int                            dimRange = BaseType::dimRange;
  typedef typename BaseType::RangeType        RangeType;
  typedef typename BaseType::ParamFieldType   ParamFieldType;
  typedef typename BaseType::ParamType        ParamType;

  typedef typename BaseType::ComponentType    ComponentType;
  typedef typename BaseType::CoefficientType  CoefficientType;

  static const std::string id()
  {
    return "function.parametric.separable.default";
  }

  SeparableDefault(const size_t _paramSize,
                   const std::vector< ParamType > _paramRange,
                   const std::vector< Dune::shared_ptr< const ComponentType > > _components,
                   const std::vector< Dune::shared_ptr< const CoefficientType > > _coefficients,
                   const std::vector< std::string > _parameterExplanation = std::vector< std::string >(),
                   const int _order = -1,
                   const std::string _name = "function.parametric.separable.default")
    : paramSize_(_paramSize)
    , paramRange_(_paramRange)
    , components_(_components)
    , coefficients_(_coefficients)
    , order_(_order)
    , name_(_name)
  {
    // some checks
    assert(paramSize_ > 0);
    assert(components_.size() > 0);
    assert(coefficients_.size() > 0);
    assert((components_.size() == coefficients_.size())
           || (components_.size() == coefficients_.size() + 1));
    assert(paramRange_.size() == 2 && "Vector has wrong size!");
    assert(paramRange_[0].size() == paramSize_ && "Vector has wrong size!");
    assert(paramRange_[1].size() == paramSize_ && "Vector has wrong size!");
    for (size_t qq = 0; qq < paramSize_; ++qq)
      assert(paramRange_[0][qq] <= paramRange_[1][qq]
          && "Given minimum parameter has to be piecewise <= maximum parameter!");
    for (size_t qq = 0; qq < components_.size(); ++qq)
      if (components_[qq]->parametric())
        DUNE_THROW(Dune::RangeError,
                   "\n" << Dune::Stuff::Common::colorStringRed("ERROR:")
                   << " component " << qq << " is parametric!");
    // process parameter explanation
    for (size_t qq = 0; qq < std::min(paramSize_, _parameterExplanation.size()); ++qq)
      parameterExplanation_.push_back(_parameterExplanation[qq]);
    for (size_t qq = std::min(paramSize_, _parameterExplanation.size()); qq < paramSize_; ++qq)
      parameterExplanation_.push_back("parameter_component_" + Dune::Stuff::Common::toString(qq));
  }

  SeparableDefault(const ThisType& other)
    : paramSize_(other.paramSize_)
    , paramRange_(other.paramRange_)
    , components_(other.components_)
    , coefficients_(other.coefficients_)
    , order_(other.order_)
    , name_(other.name_)
    , parameterExplanation_(other.parameterExplanation_)
  {}

  ThisType& operator=(const ThisType& other)
  {
    if (this != &other) {
      paramSize_ = other.paramSize();
      paramRange_ = other.paramRange();
      components_ = other.components();
      coefficients_ = other.coefficients();
      order_ = other.order();
      name_ = other.name();
      parameterExplanation_ = other.parameterExplanation();
    }
    return this;
  }

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
    return order_;
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
    return components_.size();
  }

  virtual size_t numCoefficients() const
  {
    return coefficients_.size();
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
    assert(_mu.size() == paramSize_);
    _ret = RangeFieldType(0);
    RangeType tmpComponentValue;
    for (size_t qq = 0; qq < numCoefficients(); ++qq) {
      components_[qq]->evaluate(_x, tmpComponentValue);
      tmpComponentValue *= coefficients_[qq]->evaluate(_mu);
      _ret += tmpComponentValue;
    }
    if (numComponents() > numCoefficients()) {
      components_[numCoefficients()]->evaluate(_x, tmpComponentValue);
      _ret += tmpComponentValue;
    }
  } // virtual void evaluate(const DomainType& _x, const ParamType& _mu, RangeType& _ret) const

private:
  size_t paramSize_;
  std::vector< ParamType > paramRange_;
  std::vector< Dune::shared_ptr< const ComponentType > > components_;
  std::vector< Dune::shared_ptr< const CoefficientType > > coefficients_;
  const int order_;
  const std::string name_;
  std::vector< std::string > parameterExplanation_;
}; // class SeparableDefault

} // namespace Function
} // namespace Stuff
} // namespace Dune

#endif // DUNE_STUFF_FUNCTION_PARAMETRIC_SEPARABLE_DEFAULT_HH
