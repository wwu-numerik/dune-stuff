#ifndef DUNE_STUFF_FUNCTION_PRODUCT_HH
#define DUNE_STUFF_FUNCTION_PRODUCT_HH

#include <memory>

#include <dune/common/exceptions.hh>

#include <dune/stuff/common/color.hh>

#include "interface.hh"

namespace Dune {
namespace Stuff {


// forward, to allow for specialization
template< class LeftFactorDomainFieldImp, int dimDomainLeftFactor, class LeftFactorRangeFieldImp, int dimRangeLeftFactor,
          class RightFactorDomainFieldImp, int dimDomainRightFactor, class RightFactorRangeFieldImp, int dimRangeRightFactor >
class FunctionProduct{
public:
  FunctionProduct() = delete;
};


template< class DomainFieldImp, int domainDim, class RangeFieldImp >
class FunctionProduct<  DomainFieldImp, domainDim, RangeFieldImp, 1,
                DomainFieldImp, domainDim, RangeFieldImp, 1 >
  : public FunctionInterface< DomainFieldImp, domainDim, RangeFieldImp, 1 >
{
public:
  typedef FunctionProduct<  DomainFieldImp, domainDim, RangeFieldImp, 1,
                    DomainFieldImp, domainDim, RangeFieldImp, 1 >  ThisType;
  typedef FunctionInterface< DomainFieldImp, domainDim, RangeFieldImp, 1 >  BaseType;

  typedef FunctionInterface< DomainFieldImp, domainDim, RangeFieldImp, 1 >  LeftFactorType;
  typedef FunctionInterface< DomainFieldImp, domainDim, RangeFieldImp, 1 >  RightFactorType;

  typedef typename BaseType::DomainFieldType  DomainFieldType;
  static const int                            dimDomain = BaseType::dimDomain;
  typedef typename BaseType::DomainType       DomainType;

  typedef typename BaseType::RangeFieldType   RangeFieldType;
  static const int                            dimRange = BaseType::dimRange;
  typedef typename BaseType::RangeType        RangeType;

  typedef typename BaseType::ParamFieldType ParamFieldType;
  static const int                          maxParamDim = BaseType::maxParamDim;
  typedef typename BaseType::ParamType      ParamType;

  typedef typename BaseType::ComponentType    ComponentType;
  typedef typename BaseType::CoefficientType  CoefficientType;

  static std::string id()
  {
    return BaseType::id() + ".product";
  }

  FunctionProduct(const std::shared_ptr< const LeftFactorType > _leftFactor,
                  const std::shared_ptr< const RightFactorType > _rightFactor)
    : leftFactor_(_leftFactor)
    , rightFactor_(_rightFactor)
    , leftParametric_(leftFactor_->parametric())
    , rightParametric_(rightFactor_->parametric())
    , leftSeparable_(leftParametric_ ? leftFactor_->affineparametric() : false)
    , rightSeparable_(rightParametric_ ? rightFactor_->affineparametric() : false)
    , name_("product of '" + leftFactor_->name() + "' and '" + rightFactor_->name())
    , order_((leftFactor_->order() >= 0 && rightFactor_->order() >= 0)
             ? (leftFactor_->order() + rightFactor_->order())
             : -1)
  {
    // sanity checks
    if (leftParametric_ && rightParametric_)
      DUNE_THROW(Dune::InvalidStateException,
                 "\n" << Dune::Stuff::Common::colorStringRed("ERROR:")
                 << " only one factor may be parametric!");
    // build up components
    if (leftSeparable_) {
      for (size_t qq = 0; qq < leftFactor_->numComponents(); ++qq)
        components_.push_back(std::make_shared< ThisType >(leftFactor_->components()[qq], rightFactor_));
    } else if (rightSeparable_) {
      for (size_t qq = 0; qq < rightFactor_->numComponents(); ++qq)
        components_.push_back(std::make_shared< ThisType >(leftFactor_, rightFactor_->components()[qq]));
    }
  } // Product

  std::shared_ptr< const LeftFactorType >  leftFactor() const
  {
    return leftFactor_;
  }

  std::shared_ptr< const RightFactorType > rightFactor() const
  {
    return rightFactor_;
  }

  virtual bool parametric() const
  {
    return leftParametric_ || rightParametric_;
  }

  virtual std::string name() const
  {
    return name_;
  }

  virtual int order() const
  {
    return order_;
  }

  virtual void evaluate(const DomainType& x, RangeType& ret) const
  {
    if (parametric())
      DUNE_THROW(Dune::InvalidStateException,
                 "\n" << Dune::Stuff::Common::colorStringRed("ERROR:")
                 << " nonparametric evaluate() called for a parametric function!");
    else {
      leftFactor_->evaluate(x, ret);
      RangeType tmp(0);
      rightFactor_->evaluate(x, tmp);
      ret[0] *= tmp[0];
    }
  } // ... evaluate(...)

  virtual void evaluate(const DomainType& x, const ParamType& mu, RangeType& ret) const
  {
    if (!parametric())
      DUNE_THROW(Dune::InvalidStateException,
                 "\n" << Dune::Stuff::Common::colorStringRed("ERROR:")
                 << " parametric evaluate() called for a nonparametric function!");
    RangeType leftValue(0);
    RangeType rightValue(0);
    if (leftParametric_)
      leftFactor_->evaluate(x, mu, leftValue);
    else
      leftFactor_->evaluate(x, leftValue);
    if (rightParametric_)
      rightFactor_->evaluate(x, mu, rightValue);
    else
      rightFactor_->evaluate(x, rightValue);
    ret[0] = leftValue[0];
    ret[0] *= rightValue[0];
  } // ... evaluate(...)

  virtual size_t paramSize() const
  {
    if (leftParametric_)
      return leftFactor_->paramSize();
    else if (rightParametric_)
      return rightFactor_->paramSize();
    else
      return 0;
  } // ... paramSize(...)

  virtual const std::vector< ParamType >& paramRange() const
  {
    if (!parametric())
      DUNE_THROW(Dune::InvalidStateException,
                 "\n" << Dune::Stuff::Common::colorStringRed("ERROR:")
                 << " paramRange() called for a nonparametric function!");
    if (leftParametric_)
      return leftFactor_->paramRange();
    else
      return rightFactor_->paramRange();
  } // ... paramRange(...)

  virtual const std::vector< std::string >& paramExplanation() const
  {
    if (!parametric())
      DUNE_THROW(Dune::InvalidStateException,
                 "\n" << Dune::Stuff::Common::colorStringRed("ERROR:")
                 << " paramExplanation() called for a nonparametric function!");
    if (leftParametric_)
      return leftFactor_->paramExplanation();
    else
      return rightFactor_->paramExplanation();
  } // ... paramExplanation(...)

  virtual bool affineparametric() const
  {
    if (!parametric())
      DUNE_THROW(Dune::InvalidStateException,
                 "\n" << Dune::Stuff::Common::colorStringRed("ERROR:")
                 << " affineparametric() called for a nonparametric function!");
    return leftSeparable_ || rightSeparable_;
  } // ... affineparametric(...)

  virtual size_t numComponents() const
  {
    if (!affineparametric())
      DUNE_THROW(Dune::InvalidStateException,
                 "\n" << Dune::Stuff::Common::colorStringRed("ERROR:")
                 << " numComponents() called for a nonaffineparametric function!");
    return components_.size();
  } // ... numComponents(...)

  virtual const std::vector< std::shared_ptr< const ComponentType > >& components() const
  {
    if (!affineparametric())
      DUNE_THROW(Dune::InvalidStateException,
                 "\n" << Dune::Stuff::Common::colorStringRed("ERROR:")
                 << " components() called for a nonaffineparametric function!");
    return components_;
  } // ... components(...)

  virtual size_t numCoefficients() const
  {
    if (!affineparametric())
      DUNE_THROW(Dune::InvalidStateException,
                 "\n" << Dune::Stuff::Common::colorStringRed("ERROR:")
                 << " numCoefficients() called for a nonaffineparametric function!");
    if (leftSeparable_)
      return leftFactor_->numCoefficients();
    else
      return rightFactor_->numCoefficients();
  } // ... numCoefficients(...)

  virtual const std::vector< std::shared_ptr< const CoefficientType > >& coefficients() const
  {
    if (!affineparametric())
      DUNE_THROW(Dune::InvalidStateException,
                 "\n" << Dune::Stuff::Common::colorStringRed("ERROR:")
                 << " coefficients() called for a nonaffineparametric function!");
    if (leftSeparable_)
      return leftFactor_->coefficients();
    else
      return rightFactor_->coefficients();
  } // ... coefficients(...)

private:
  const std::shared_ptr< const LeftFactorType >  leftFactor_;
  const std::shared_ptr< const RightFactorType > rightFactor_;
  const bool leftParametric_;
  const bool rightParametric_;
  const bool leftSeparable_;
  const bool rightSeparable_;
  const std::string name_;
  const int order_;
  std::vector< std::shared_ptr< const ComponentType > > components_;
}; // class FunctionProduct< ..., ... >

} // namespace Stuff
} // namespace Dune

#endif // DUNE_STUFF_FUNCTION_PRODUCT_HH
