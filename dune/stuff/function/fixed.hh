#ifndef DUNE_STUFF_FUNCTION_FIXED_HH
#define DUNE_STUFF_FUNCTION_FIXED_HH

#include <dune/common/shared_ptr.hh>
#include <dune/common/exceptions.hh>

#include "interface.hh"
#include <dune/stuff/common/parameter.hh>
#include <dune/stuff/common/color.hh>

namespace Dune {
namespace Stuff {
namespace Function {


template< class DomainFieldImp, int domainDim, class RangeFieldImp, int rangeDim >
class Fixed
  : public Interface< DomainFieldImp, domainDim, RangeFieldImp, rangeDim >
{
public:
  typedef Fixed< DomainFieldImp, domainDim, RangeFieldImp, rangeDim >     ThisType;
  typedef Interface< DomainFieldImp, domainDim, RangeFieldImp, rangeDim > BaseType;

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

  Fixed(const Dune::shared_ptr< const BaseType > parametricFunction,
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
  const Dune::shared_ptr< const BaseType > wrappedFunction_;
  const ParamType fixedParam_;
}; // class Fixed


} // namespace Function
} // namespace Stuff
} // namespace Dune

#endif // DUNE_STUFF_FUNCTION_FIXED_HH
