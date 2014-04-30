// This file is part of the dune-stuff project:
//   https://users.dune-project.org/projects/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#ifndef DUNE_STUFF_FUNCTION_PRODUCT_HH
#define DUNE_STUFF_FUNCTION_PRODUCT_HH

#include <memory>

#include <dune/common/exceptions.hh>

#include "interfaces.hh"

namespace Dune {
namespace Stuff {
namespace Functions {


// forward, to allow for specialization
template< class DL, int dL, class RL, int rRL, int rCL,
          class DR, int dR, class RR, int rRR, int RCR >
class FunctionProduct{
public:
  FunctionProduct() = delete;
};


template< class DomainFieldImp, int domainDim, class RangeFieldImp >
class FunctionProduct<  DomainFieldImp, domainDim, RangeFieldImp, 1, 1,
                        DomainFieldImp, domainDim, RangeFieldImp, 1, 1 >
  : public FunctionInterface< DomainFieldImp, domainDim, RangeFieldImp, 1, 1 >
{
  typedef FunctionInterface< DomainFieldImp, domainDim, RangeFieldImp, 1, 1 >  BaseType;
  typedef FunctionProduct<  DomainFieldImp, domainDim, RangeFieldImp, 1, 1,
                            DomainFieldImp, domainDim, RangeFieldImp, 1, 1 > ThisType;
public:

  typedef FunctionInterface< DomainFieldImp, domainDim, RangeFieldImp, 1, 1 >  LeftFactorType;
  typedef FunctionInterface< DomainFieldImp, domainDim, RangeFieldImp, 1, 1 >  RightFactorType;

  typedef typename BaseType::DomainFieldType  DomainFieldType;
  static const int                            dimDomain = BaseType::dimDomain;
  typedef typename BaseType::DomainType       DomainType;

  typedef typename BaseType::RangeFieldType   RangeFieldType;
  static const int                            dimRange = BaseType::dimRange;
  typedef typename BaseType::RangeType        RangeType;

  static std::string static_id()
  {
    return BaseType::static_id() + ".product";
  }

  FunctionProduct(const std::shared_ptr< const LeftFactorType > _leftFactor,
                  const std::shared_ptr< const RightFactorType > _rightFactor)
    : leftFactor_(_leftFactor)
    , rightFactor_(_rightFactor)
    , name_("product of '" + leftFactor_->name() + "' and '" + rightFactor_->name())
    , order_((leftFactor_->order() >= 0 && rightFactor_->order() >= 0)
             ? (leftFactor_->order() + rightFactor_->order())
             : -1)
  {}

  std::shared_ptr< const LeftFactorType >  leftFactor() const
  {
    return leftFactor_;
  }

  std::shared_ptr< const RightFactorType > rightFactor() const
  {
    return rightFactor_;
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
    leftFactor_->evaluate(x, ret);
    RangeType tmp(0);
    rightFactor_->evaluate(x, tmp);
    ret[0] *= tmp[0];
  } // ... evaluate(...)

private:
  const std::shared_ptr< const LeftFactorType >  leftFactor_;
  const std::shared_ptr< const RightFactorType > rightFactor_;
  const int order_;
  const std::string name_;
}; // class FunctionProduct


} // namespace Functions
} // namespace Stuff
} // namespace Dune

#endif // DUNE_STUFF_FUNCTION_PRODUCT_HH
