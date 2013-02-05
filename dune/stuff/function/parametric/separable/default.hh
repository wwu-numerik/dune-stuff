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

#include "../../interface.hh"
#include "coefficient.hh"

namespace Dune {
namespace Stuff {
namespace Function {


template< class DomainFieldImp, int domainDim, class RangeFieldImp, int rangeDim >
class SeparableDefault
  : public Interface< DomainFieldImp, domainDim, RangeFieldImp, rangeDim >
{
public:
  typedef SeparableDefault< DomainFieldImp, domainDim, RangeFieldImp, rangeDim >   ThisType;
  typedef Interface< DomainFieldImp, domainDim, RangeFieldImp, rangeDim > BaseType;

  typedef typename BaseType::DomainType     DomainType;
  typedef typename BaseType::RangeFieldType RangeFieldType;
  typedef typename BaseType::RangeType      RangeType;
  typedef typename BaseType::ParamType      ParamType;

  typedef typename BaseType::ComponentType    ComponentType;
  typedef typename BaseType::CoefficientType  CoefficientType;

  SeparableDefault(const size_t _paramSize,
          const std::vector< ParamType > _paramRange,
          const std::vector< Dune::shared_ptr< const ComponentType > > _components,
          const std::vector< Dune::shared_ptr< const CoefficientType > > _coefficients,
          const std::vector< std::string > _parameterExplanation = std::vector< std::string >(),
          const int _order = -1,
          const std::string _name = "function.parametric.separable.SeparableDefault")
    : paramSize_(_paramSize)
    , paramRange_(_paramRange)
    , numComponents_(_components.size())
    , numCoefficients_(_coefficients.size())
    , components_(_components)
    , coefficients_(_coefficients)
    , order_(_order)
    , name_(_name)
  {
    // some checks
    assert(paramSize_ > 0);
    assert(numComponents_ > 0);
    assert(numCoefficients_ > 0);
    assert((numComponents_ == numCoefficients_)
           || (numComponents_ == numCoefficients_ + 1));
    assert(paramRange_.size() == 2 && "Vector has wrong size!");
    assert(paramRange_[0].size() == paramSize_ && "Vector has wrong size!");
    assert(paramRange_[1].size() == paramSize_ && "Vector has wrong size!");
    for (unsigned int qq = 0; qq < paramSize_; ++qq)
      assert(paramRange_[0][qq] <= paramRange_[1][qq]
          && "Given minimum parameter has to be piecewise <= maximum parameter!");
    // process parameter explanation
    for (unsigned int qq = 0; qq < std::min(paramSize_, _parameterExplanation.size()); ++qq)
      parameterExplanation_.push_back(_parameterExplanation[qq]);
    for (unsigned int qq = std::min(paramSize_, _parameterExplanation.size()); qq < paramSize_; ++qq)
      parameterExplanation_.push_back("parameter_component_" + Dune::Stuff::Common::toString(qq));
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
    return numComponents_;
  }

  virtual size_t numCoefficients() const
  {
    return numCoefficients_;
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
    RangeType tmpCoefficientValue;
    for (unsigned int qq = 0; qq < numComponents_; ++qq) {
      components_[qq]->evaluate(_x, tmpComponentValue);
      coefficients_[qq]->evaluate(_mu, tmpCoefficientValue);
      assert(tmpCoefficientValue.size() == 1);
      tmpComponentValue *= tmpCoefficientValue[0];
      _ret += tmpComponentValue;
    }
    if (numComponents_ > numCoefficients_) {
      components_[numComponents_]->evaluate(_x, tmpComponentValue);
      _ret += tmpComponentValue;
    }
  } // virtual void evaluate(const DomainType& _x, const ParamType& _mu, RangeType& _ret) const

private:
  size_t paramSize_;
  std::vector< ParamType > paramRange_;
  size_t numComponents_;
  size_t numCoefficients_;
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
