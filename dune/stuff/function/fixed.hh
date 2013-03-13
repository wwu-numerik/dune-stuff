#ifndef DUNE_STUFF_FUNCTION_FIXED_HH
#define DUNE_STUFF_FUNCTION_FIXED_HH

#include <memory>

#include <dune/common/exceptions.hh>

#include <dune/stuff/common/parameter.hh>
#include <dune/stuff/common/color.hh>

#include "interface.hh"

namespace Dune {
namespace Stuff {


template< class DomainFieldImp, int domainDim, class RangeFieldImp, int rangeDim >
class FunctionFixed
  : public FunctionInterface< DomainFieldImp, domainDim, RangeFieldImp, rangeDim >
{
public:
  typedef FunctionFixed< DomainFieldImp, domainDim, RangeFieldImp, rangeDim >     ThisType;
  typedef FunctionInterface< DomainFieldImp, domainDim, RangeFieldImp, rangeDim > BaseType;

  typedef typename BaseType::DomainFieldType  DomainFieldType;
  static const int                            dimDomain = BaseType::dimDomain;
  typedef typename BaseType::DomainType       DomainType;
  typedef typename BaseType::RangeFieldType   RangeFieldType;
  static const int                            dimRange = BaseType::dimRange;
  typedef typename BaseType::RangeType        RangeType;
  typedef typename BaseType::ParamType        ParamType;

  static std::string id()
  {
    return BaseType::id() + ".fixed";
  }

  FunctionFixed(const std::shared_ptr< const BaseType > parametricFunction,
                const ParamType fixedParameter = ParamType())
    : wrappedFunction_(parametricFunction)
    , fixedParam_(fixedParameter)
  {
    if (fixedParam_.size() != wrappedFunction_->paramSize())
      DUNE_THROW(Dune::InvalidStateException,
                 "\n" << Dune::Stuff::Common::colorStringRed("ERROR:")
                 << " given parameter has wrong size (is " << fixedParam_.size()
                 << ", should be " << wrappedFunction_->paramSize() << ")!");
  }

  virtual bool parametric() const
  {
    return false;
  }

  virtual std::string name() const
  {
    return "fixed '" + wrappedFunction_->name() + "' for parameter '" + Dune::Stuff::Common::Parameter::report(fixedParam_);
  }

  virtual int order() const
  {
    return wrappedFunction_->order();
  }

  virtual void evaluate(const DomainType& x, RangeType& ret) const
  {
    wrappedFunction_->evaluate(x, fixedParam_, ret);
  }

private:
  const std::shared_ptr< const BaseType > wrappedFunction_;
  const ParamType fixedParam_;
}; // class FunctionFixed


} // namespace Stuff
} // namespace Dune

#endif // DUNE_STUFF_FUNCTION_FIXED_HH
